#include "rascal_mepa.h"
#include <stdlib.h>
#include <string.h>

// Private Auxiliary Functions
static int count_vars(VarDeclaration* list) {
    int count = 0;
    while (list) {
        count++;
        list = list->next;
    }
    return count;
}

static int get_proc_label(char* name) {
    Symbol* s = lookup(name);
    if (s) return s->offset;
    return -1;
}

// Auxiliary Write Functions
int get_new_label(CodeGenContext* ctx) {
    return ++(ctx->label_count);
}

void emit_label(CodeGenContext* ctx, int label) {
    fprintf(ctx->mepafile, "L%d: NADA\n", label);
}

void emit_no_arg(CodeGenContext* ctx, const char* op) {
    fprintf(ctx->mepafile, "\t%s\n", op);
}

void emit_arg_int(CodeGenContext* ctx, const char* op, int arg) {
    fprintf(ctx->mepafile, "\t%s %d\n", op, arg);
}

void emit_arg_2_int(CodeGenContext* ctx, const char* op, int arg1, int arg2) {
    fprintf(ctx->mepafile, "\t%s %d, %d\n", op, arg1, arg2);
}

void emit_arg_label(CodeGenContext* ctx, const char* op, int label) {
    fprintf(ctx->mepafile, "\t%s L%d\n", op, label);
}

// MEPA Code Generation Functions
void generate_code(Program *root, const char *filename) {
    CodeGenContext ctx;
    ctx.mepafile = fopen(filename, "w");
    ctx.label_count = 0;
    ctx.current_level = 0;

    if (!ctx.mepafile) {
        perror("Erro ao abrir arquivo de saída");
        return;
    }

    enter_scope(); 
    gen_program(root, &ctx);
    leave_scope();

    fclose(ctx.mepafile);
    printf("Código MEPA gerado em: %s\n", filename);
}

void gen_program(Program* p, CodeGenContext* ctx) {
    if (!p) return;
    // Program
    emit_no_arg(ctx, "INPP");
    install(p->identifier, CAT_PROGRAM, TYPE_VOID, ctx->current_level); // CONSIDERANDO O CAMPO LEVEL NA STRUCT SYMBOL QUE SERÁ INSTALADO

    // Block
    gen_block(p->block, ctx);

    emit_no_arg(ctx, "PARA");
    emit_no_arg(ctx, "FIM");
}

void gen_block(Block* b, CodeGenContext* ctx) {
    if (!b) return;
    // Variable Declaration Section
    int vars_count = count_vars(b->varDeclarations);
    if (vars_count > 0) {
        emit_arg_int(ctx, "AMEM", vars_count);
    }

    gen_var_decl(b->varDeclarations, ctx);

    // Subroutine Declaration Section
    if (b->subRotDeclarations) {
        int label_main = get_new_label(ctx);
        emit_arg_label(ctx, "DSVS", label_main);
        gen_subrot_decl(b->subRotDeclarations, ctx);
        emit_label(ctx, label_main);
    }

    gen_command_list(b->commandList, ctx);
    
    if (vars_count > 0) {
        emit_arg_int(ctx, "DMEM", vars_count);
    }
}

void gen_var_decl(VarDeclaration *vd, CodeGenContext *ctx) {
    while (vd) {
        install(vd->identifier, CAT_VAR, 
               (vd->type == Int ? TYPE_INT : TYPE_BOOL), 
               ctx->current_level); // CONSIDERANDO O CAMPO LEVEL NA STRUCT SYMBOL QUE SERÁ INSTALADO
        
        vd = vd->next;
    }
}

void gen_subrot_decl(SubRotDeclaration* sd, CodeGenContext* ctx) {
    while (sd) {
        int label = get_new_label(ctx);
        // Type
        if (sd->type == Proc) {
            Symbol* s = install(sd->subrotU.procInfo.identifier, CAT_PROCEDURE, TYPE_VOID, ctx->current_level);
            if(s) s->offset = label;
        } else {
            Symbol* s = install(sd->subrotU.funcInfo.identifier, CAT_FUNCTION, (sd->subrotU.funcInfo.returnType == Int ? TYPE_INT : TYPE_BOOL), ctx->current_level);
            if(s) s->offset = label;
        }
        emit_label(ctx, label);

        // Finish setup for Activation Frame in the subroutine

        // ENPR: Previous Context
        ctx->current_level++;
        emit_arg_int(ctx, "ENPR", ctx->current_level);
        enter_scope();
        // Parameters
        VarDeclaration* params = (sd->type == Proc ? sd->subrotU.procInfo.formParams : sd->subrotU.funcInfo.formParams);
        int n_params = count_params(params);
        int i = 0;
        VarDeclaration* p_iter = params;
        while (p_iter) {
            int offset = - (n_params + 4) + i;
            Symbol* s = install(p_iter->identifier, CAT_PARAM, 
                                (p_iter->type == Int ? TYPE_INT : TYPE_BOOL), 
                                ctx->current_level);
            if(s) s->offset = offset;

            p_iter = p_iter->next;
            i++;
        }
        
        // Return, if is function
        if (sd->type == Func) {
            int ret_offset = - (n_params + 5);
            Symbol* s = install(sd->subrotU.funcInfo.identifier, CAT_VAR, (sd->subrotU.funcInfo.returnType == Int ? TYPE_INT : TYPE_BOOL), ctx->current_level);
            if(s) s->offset = ret_offset;
        }

        // Create instructions
        if (sd->type == Proc) {
            gen_block(sd->subrotU.procInfo.subRotBlock, ctx);
        } else {
            gen_block(sd->subrotU.funcInfo.subRotBlock, ctx);
        }

        // Return from subroutine
        emit_arg_2_int(ctx, "RTPR", ctx->current_level, n_params);

        leave_scope();
        ctx->current_level--;

        sd = sd->next;
    }
}

void gen_command_list(Command* c, CodeGenContext* ctx) {
    while (c) {
        gen_command(c, ctx);
        c = c->next;
    }
}

void gen_command(Command* c, CodeGenContext* ctx) {
    if (!c) return;
    switch (c->type) {
        case Assign:      gen_assign(c, ctx); break;
        case ProcCall:    gen_proc_call(c, ctx); break;
        case Conditional: gen_if(c, ctx); break;
        case Loop:        gen_while(c, ctx); break;
        case Read:        gen_read(c, ctx); break;
        case Write:       gen_write(c, ctx); break;
    }
}

void gen_assign(Command* c, CodeGenContext* ctx) {
    gen_expression(c->cmdU.assignInfo.expression, ctx);
    Symbol* s = lookup(c->cmdU.assignInfo.identifier);
    emit_arg_2_int(ctx, "ARMZ", s->level, s->offset);
}

void gen_proc_call(Command* c, CodeGenContext* ctx) {
    char* name = c->cmdU.procCallInfo.identifier;
    Symbol* s = lookup(name);

    Expression* arg = c->cmdU.procCallInfo.expressionList;
    while (arg) {
        gen_expression(arg, ctx);
        arg = arg->next;
    }

    // Start setup for Activation Frame in the caller
    fprintf(ctx->mepafile, "\tCHPR L%d, %d\n", s->offset, ctx->current_level);
}

void gen_if(Command* c, CodeGenContext* ctx) {
    int label_else = get_new_label(ctx);
    int label_end = get_new_label(ctx);

    // Conditional Expression
    gen_expression(c->cmdU.condInfo.condExpression, ctx);
    emit_arg_label(ctx, "DSVF", label_else);

    // If:
    gen_command(c->cmdU.condInfo.cmdIf, ctx);
    emit_arg_label(ctx, "DSVS", label_end);

    // Else:
    emit_label(ctx, label_else);
    if (c->cmdU.condInfo.cmdElse) {
        gen_command(c->cmdU.condInfo.cmdElse, ctx);
    }

    // After
    emit_label(ctx, label_end);
}

void gen_while(Command* c, CodeGenContext* ctx) {
    int label_loop = get_new_label(ctx);
    int label_end = get_new_label(ctx);

    emit_label(ctx, label_loop);

    // Loop Conditional Expression
    gen_expression(c->cmdU.loopInfo.loopExpression, ctx);
    emit_arg_label(ctx, "DSVF", label_end);

    // Loop Body
    gen_command(c->cmdU.loopInfo.cmdLoop, ctx);

    // Loop Check
    emit_arg_label(ctx, "DSVS", label_loop);

    emit_label(ctx, label_end);
}

void gen_read(Command* c, CodeGenContext* ctx) {
    IdentifierList* id = c->cmdU.readInfo.identifiers;
    while(id) {
        emit_no_arg(ctx, "LEIT");
        
        Symbol* s = lookup(id->identifier);
        if (s) {
            emit_arg_2_int(ctx, "ARMZ", s->level, s->offset);
        }
        id = id->next;
    }
}

void gen_write(Command* c, CodeGenContext* ctx) {
    Expression* e = c->cmdU.writeInfo.expressionList;
    while (e) {
        gen_expression(e, ctx);
        emit_no_arg(ctx, "IMPR");
        e = e->next;
    }
}

void gen_expression(Expression* e, CodeGenContext* ctx) {
    if (!e) return;
    switch (e->type) {
        case Binary:    gen_binary_expr(e, ctx); break;
        case Unary:     gen_unary_expr(e, ctx); break;
        case Var:       gen_var_usage(e, ctx); break;
        case ConstInt:  gen_literal_int(e, ctx); break;
        case ConstBool: gen_literal_bool(e, ctx); break;
        case FuncCall:  gen_func_call(e, ctx); break;
    }
}

void gen_binary_expr(Expression* e, CodeGenContext* ctx) {
    gen_expression(e->exprU.binExpr.left, ctx);
    gen_expression(e->exprU.binExpr.right, ctx);
    
    switch (e->exprU.binExpr.operator) {
        case Plus:           emit_no_arg(ctx, "SOMA"); break;
        case Minus:          emit_no_arg(ctx, "SUBT"); break;
        case Multiplication: emit_no_arg(ctx, "MULT"); break;
        case Division:       emit_no_arg(ctx, "DIVI"); break;
        case Equal:          emit_no_arg(ctx, "CMIG"); break;
        case Different:      emit_no_arg(ctx, "CMDG"); break;
        case Less:           emit_no_arg(ctx, "CMME"); break;
        case LessEqual:      emit_no_arg(ctx, "CMEG"); break;
        case Greater:        emit_no_arg(ctx, "CMMA"); break;
        case GreaterEqual:   emit_no_arg(ctx, "CMAG"); break;
        case And:            emit_no_arg(ctx, "CONJ"); break;
        case Or:             emit_no_arg(ctx, "DISJ"); break;
        default: break;
    }
}

void gen_unary_expr(Expression* e, CodeGenContext* ctx) {
    gen_expression(e->exprU.unyExpr.right, ctx);
    switch (e->exprU.unyExpr.operator) {
        case Minus: emit_no_arg(ctx, "INVR"); break;
        case Not:   emit_no_arg(ctx, "NEGA"); break;
        default: break;
    }
}

void gen_var_usage(Expression* e, CodeGenContext* ctx) {
    Symbol* s = lookup(e->exprU.varExpr.identifier);
    emit_arg_2_int(ctx, "CRVL", s->level, s->offset);
}

void gen_literal_int(Expression* e, CodeGenContext* ctx) {
    emit_arg_int(ctx, "CRCT", e->exprU.intExpr.number);
}

void gen_literal_bool(Expression* e, CodeGenContext* ctx) {
    emit_arg_int(ctx, "CRCT", (e->exprU.boolExpr.boolean == BoolTrue ? 1 : 0));
}

void gen_func_call(Expression* e, CodeGenContext* ctx) {
    char* name = e->exprU.funCallExpr.identifier;
    Symbol* s = lookup(name);

    // Return Adress
    emit_arg_int(ctx, "AMEM", 1);

    // Start setup for Activation Frame in the caller
    Expression* arg = e->exprU.funCallExpr.expressionList;
    while (arg) {
        gen_expression(arg, ctx);
        arg = arg->next;
    }

    fprintf(ctx->mepafile, "\tCHPR L%d, %d\n", s->offset, ctx->current_level);
}