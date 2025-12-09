#include "rascal_mepa.h"
#include <stdlib.h>
#include <string.h>

// MEPA Code Generation Functions
void generateCode(Program *root, const char *filename) {
    CodeGenContext ctx;
    ctx.mepaFile = fopen(filename, "w");
    ctx.labelCount = 0;
    ctx.currentLevel = 0;

    if (!ctx.mepaFile) {
        perror("Erro ao abrir arquivo de saída");
        return;
    }

    enter_scope(); 
    generateProgram(root, &ctx);
    leave_scope();

    fclose(ctx.mepaFile);
    printf("Código MEPA gerado em: %s\n", filename);
}

void generateProgram(Program* p, CodeGenContext* ctx) {
    if (!p) return;
    writeInstr(ctx, "INPP");
    install(p->identifier, CAT_PROGRAM, TYPE_VOID, ctx->currentLevel);

    // Block
    generateBlock(p->block, ctx);

    writeInstr(ctx, "PARA");
    writeInstr(ctx, "FIM");
}

void generateBlock(Block* b, CodeGenContext* ctx) {
    if (!b) return;
    // Variable Declaration Section
    generateVariableDeclaration(b->varDeclarations, ctx);
    int global_count = varListSize(b->varDeclarations);
    if (global_count > 0) {
        writeInstrIntArg(ctx, "AMEM", global_count);
    }

    // Subroutine Declaration Section
    if (b->subRotDeclarations) {
        int label_main = newLabel(ctx);
        writeInstrLabelArg(ctx, "DSVS", label_main);

        generateSubRotDeclaration(b->subRotDeclarations, ctx);

        writeLabel(ctx, label_main);
    }
    
    // Command List
    generateCommandList(b->commandList, ctx);
    
    // Deallocate Global Variables
    if (global_count > 0) {
        writeInstrIntArg(ctx, "DMEM", global_count);
    }
}

void generateVariableDeclaration(VarDeclaration *vd, CodeGenContext *ctx) {
    while (vd) {
        install(vd->identifier, CAT_VAR, (vd->type == Int ? TYPE_INT : TYPE_BOOL), ctx->currentLevel);
        vd = vd->next;
    }
}

void generateSubRotDeclaration(SubRotDeclaration* sd, CodeGenContext* ctx) {
    while (sd) {
        int label = newLabel(ctx);

        // Install Subroutine in Symbol Table
        Symbol* s;
        if (sd->type == Proc) {
            s = install(sd->subrotU.procInfo.identifier, CAT_PROCEDURE, TYPE_VOID, ctx->currentLevel);
        } else {
            s = install(sd->subrotU.funcInfo.identifier, CAT_FUNCTION, (sd->subrotU.funcInfo.returnType == Int ? TYPE_INT : TYPE_BOOL), ctx->currentLevel);
        }
        if (s) s->offset = label;

        // Enter Subroutine
        writeLabel(ctx, label);        
        ctx->currentLevel++;
        enter_scope();
        writeInstrIntArg(ctx, "ENPR", ctx->currentLevel);

        // Verify Parameters Offset
        VarDeclaration* params = (sd->type == Proc ? sd->subrotU.procInfo.formParams : sd->subrotU.funcInfo.formParams);
        int n_params = varListSize(params);

        VarDeclaration* p_iter = params;
        int i = 0;
        while (p_iter) {
            int offset = -5 - i;
            Symbol* param_sym = install(p_iter->identifier, CAT_PARAM, (p_iter->type == Int ? TYPE_INT : TYPE_BOOL), ctx->currentLevel);
            if(param_sym) param_sym->offset = offset;
            p_iter = p_iter->next;
            i++;
        }
        
        // Return, if is function
        if (sd->type == Func) {
            int ret_offset = - (5 + n_params);
            Symbol* ret_symbol = install(sd->subrotU.funcInfo.identifier, CAT_VAR, (sd->subrotU.funcInfo.returnType == Int ? TYPE_INT : TYPE_BOOL), ctx->currentLevel);
            if(ret_symbol) ret_symbol->offset = ret_offset;
        }

        // Create Subroutine Block
        if (sd->type == Proc) {
            generateSubRotBlock(sd->subrotU.procInfo.subRotBlock, ctx);
        } else {
            generateSubRotBlock(sd->subrotU.funcInfo.subRotBlock, ctx);
        }

        // Return from subroutine
        writeInstrIntArg(ctx, "RTPR", n_params);

        leave_scope();
        ctx->currentLevel--;

        sd = sd->next;
    }
}

void generateSubRotBlock(SubRotBlock* sb, CodeGenContext* ctx) {
    if (!sb) return;
    // Allocate Local Variables of Subroutine Block
    generateVariableDeclaration(sb->varDeclarations, ctx); 
    int local_count = varListSize(sb->varDeclarations);
    if (local_count > 0) {
        writeInstrIntArg(ctx, "AMEM", local_count);
    }

    // Commands of Subroutine Block
    generateCommandList(sb->commands, ctx);

    // Deallocate Local Variables
    if (local_count > 0) {
        writeInstrIntArg(ctx, "DMEM", local_count);
    }
}

void generateCommandList(Command* c, CodeGenContext* ctx) {
    while (c) {
        generateCommand(c, ctx);
        c = c->next;
    }
}

void generateCommand(Command* c, CodeGenContext* ctx) {
    if (!c) return;
    switch (c->type) {
        case Assign:      generateAssignCmd(c, ctx); break;
        case ProcCall:    generateProcedureCallCmd(c, ctx); break;
        case Conditional: generateConditionalCmd(c, ctx); break;
        case Loop:        generateLoopCmd(c, ctx); break;
        case Read:        generateReadCmd(c, ctx); break;
        case Write:       generateWriteCmd(c, ctx); break;
    }
}

void generateAssignCmd(Command* c, CodeGenContext* ctx) {
    generateExpression(c->cmdU.assignInfo.expression, ctx);
    Symbol* s = lookup(c->cmdU.assignInfo.identifier);
    writeInstr2IntArg(ctx, "ARMZ", s->level, s->offset);
}

void generateProcedureCallCmd(Command* c, CodeGenContext* ctx) {
    char* name = c->cmdU.procCallInfo.identifier;
    Symbol* s = lookup(name);

    generateReverseExpressions(c->cmdU.procCallInfo.expressionList, ctx);

    fprintf(ctx->mepaFile, "\tCHPR R%d, %d\n", s->offset, ctx->currentLevel);
}

void generateReverseExpressions(Expression* expr, CodeGenContext* ctx) {
    if (!expr) return;
    generateReverseExpressions(expr->next, ctx);
    generateExpression(expr, ctx);
}

void generateConditionalCmd(Command* c, CodeGenContext* ctx) {
    int label_else = newLabel(ctx);
    int label_end = newLabel(ctx);

    // Conditional Expression
    generateExpression(c->cmdU.condInfo.condExpression, ctx);
    writeInstrLabelArg(ctx, "DSVF", label_else);

    // If:
    generateCommand(c->cmdU.condInfo.cmdIf, ctx);
    writeInstrLabelArg(ctx, "DSVS", label_end);

    // Else:
    writeLabel(ctx, label_else);
    if (c->cmdU.condInfo.cmdElse) {
        generateCommand(c->cmdU.condInfo.cmdElse, ctx);
    }

    // After
    writeLabel(ctx, label_end);
}

void generateLoopCmd(Command* c, CodeGenContext* ctx) {
    int label_loop = newLabel(ctx);
    int label_end = newLabel(ctx);

    writeLabel(ctx, label_loop);

    // Loop Conditional Expression
    generateExpression(c->cmdU.loopInfo.loopExpression, ctx);
    writeInstrLabelArg(ctx, "DSVF", label_end);

    // Loop Body
    generateCommand(c->cmdU.loopInfo.cmdLoop, ctx);

    // Loop Check
    writeInstrLabelArg(ctx, "DSVS", label_loop);

    writeLabel(ctx, label_end);
}

void generateReadCmd(Command* c, CodeGenContext* ctx) {
    IdentifierList* id = c->cmdU.readInfo.identifiers;
    while(id) {
        writeInstr(ctx, "LEIT");
        
        Symbol* s = lookup(id->identifier);
        if (s) writeInstr2IntArg(ctx, "ARMZ", s->level, s->offset);
        id = id->next;
    }
}

void generateWriteCmd(Command* c, CodeGenContext* ctx) {
    Expression* e = c->cmdU.writeInfo.expressionList;
    while (e) {
        generateExpression(e, ctx);
        writeInstr(ctx, "IMPR");
        e = e->next;
    }
}

void generateExpression(Expression* e, CodeGenContext* ctx) {
    if (!e) return;
    switch (e->type) {
        case Binary:    generateBinaryExpr(e, ctx); break;
        case Unary:     generateUnaryexpr(e, ctx); break;
        case Var:       generateVarExpr(e, ctx); break;
        case ConstInt:  generateIntExpr(e, ctx); break;
        case ConstBool: generateBooleanExpr(e, ctx); break;
        case FuncCall:  generateFunctionCallExpr(e, ctx); break;
    }
}

void generateBinaryExpr(Expression* e, CodeGenContext* ctx) {
    generateExpression(e->exprU.binExpr.left, ctx);
    generateExpression(e->exprU.binExpr.right, ctx);
    
    switch (e->exprU.binExpr.operator) {
        case Plus:           writeInstr(ctx, "SOMA"); break;
        case Minus:          writeInstr(ctx, "SUBT"); break;
        case Multiplication: writeInstr(ctx, "MULT"); break;
        case Division:       writeInstr(ctx, "DIVI"); break;
        case Equal:          writeInstr(ctx, "CMIG"); break;
        case Different:      writeInstr(ctx, "CMDG"); break;
        case Less:           writeInstr(ctx, "CMME"); break;
        case LessEqual:      writeInstr(ctx, "CMEG"); break;
        case Greater:        writeInstr(ctx, "CMMA"); break;
        case GreaterEqual:   writeInstr(ctx, "CMAG"); break;
        case And:            writeInstr(ctx, "CONJ"); break;
        case Or:             writeInstr(ctx, "DISJ"); break;
        default: break;
    }
}

void generateUnaryexpr(Expression* e, CodeGenContext* ctx) {
    generateExpression(e->exprU.unyExpr.right, ctx);
    switch (e->exprU.unyExpr.operator) {
        case Minus: writeInstr(ctx, "INVR"); break;
        case Not:   writeInstr(ctx, "NEGA"); break;
        default: break;
    }
}

void generateVarExpr(Expression* e, CodeGenContext* ctx) {
    Symbol* s = lookup(e->exprU.varExpr.identifier);
    writeInstr2IntArg(ctx, "CRVL", s->level, s->offset);
}

void generateIntExpr(Expression* e, CodeGenContext* ctx) {
    writeInstrIntArg(ctx, "CRCT", e->exprU.intExpr.number);
}

void generateBooleanExpr(Expression* e, CodeGenContext* ctx) {
    writeInstrIntArg(ctx, "CRCT", (e->exprU.boolExpr.boolean == BoolTrue ? 1 : 0));
}

void generateFunctionCallExpr(Expression* e, CodeGenContext* ctx) {
    char* name = e->exprU.funCallExpr.identifier;
    Symbol* s = lookup(name);

    writeInstrIntArg(ctx, "AMEM", 1);

    generateReverseExpressions(e->exprU.funCallExpr.expressionList, ctx);

    fprintf(ctx->mepaFile, "\tCHPR R%d, %d\n", s->offset, ctx->currentLevel);
}

// Auxiliary Write Functions
int varListSize(VarDeclaration* list) {
    int count = 0;
    while (list) {
        count++;
        list = list->next;
    }
    return count;
}

int newLabel(CodeGenContext* ctx) {
    return ++(ctx->labelCount);
}

void writeLabel(CodeGenContext* ctx, int label) {
    fprintf(ctx->mepaFile, "R%d: NADA\n", label);
}

void writeInstr(CodeGenContext* ctx, const char* op) {
    fprintf(ctx->mepaFile, "\t%s\n", op);
}

void writeInstrIntArg(CodeGenContext* ctx, const char* op, int arg) {
    fprintf(ctx->mepaFile, "\t%s %d\n", op, arg);
}

void writeInstr2IntArg(CodeGenContext* ctx, const char* op, int arg1, int arg2) {
    fprintf(ctx->mepaFile, "\t%s %d, %d\n", op, arg1, arg2);
}

void writeInstrLabelArg(CodeGenContext* ctx, const char* op, int label) {
    fprintf(ctx->mepaFile, "\t%s R%d\n", op, label);
}