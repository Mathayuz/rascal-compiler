#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "semantics.h"
#include "rascal_ast.h"
#include "symbol_table.h"

extern int yylineno;

// Lista global contendo todas as sub-rotinas declaradas no programa
static SubRotDeclaration *global_subrot_list = NULL;

// Função auxiliar para erro semântico
static void semantic_error(const char *msg) {
    printf("Erro semântico na linha %d: %s\n", yylineno, msg);
    exit(1);
}

static Type varType_to_type(varType vt) {
    switch (vt) {
        case Int:  return TYPE_INT;
        case Bool: return TYPE_BOOL;
        default:   return TYPE_UNKNOWN;
    }
}

// Declarações internas
static void check_program(Program *p);
static void check_block(Block *b);

static void check_var_declarations(VarDeclaration *list, int as_params);

static void predeclare_subroutines(SubRotDeclaration *list);
static void check_subroutines(SubRotDeclaration *list);
static void check_subroutine(SubRotDeclaration *srd);

static void check_subroutine_block(SubRotBlock *srb,
                                   const char *func_name,
                                   int is_function,
                                   int *return_count);

static void check_command_list(Command *list,
                               const char *current_func_name,
                               int *return_count);

static void check_command(Command *cmd,
                          const char *current_func_name,
                          int *return_count);

static void check_assign_command(Command *cmd,
                                 const char *current_func_name,
                                 int *return_count);

static void check_proc_call_command(Command *cmd);
static void check_conditional_command(Command *cmd,
                                      const char *current_func_name,
                                      int *return_count);
static void check_loop_command(Command *cmd,
                               const char *current_func_name,
                               int *return_count);
static void check_read_command(Command *cmd);
static void check_write_command(Command *cmd);

static void check_expression_list(Expression *list,
                                  VarDeclaration *formals);

static Type check_expression(Expression *e);
static Type check_binary_expression(Expression *e);
static Type check_unary_expression(Expression *e);
static Type check_variable_expression(Expression *e);
static Type check_function_call_expression(Expression *e);


// Função principal da análise semântica
void semantic_check(Program *program) {
    if (program == NULL) {
        semantic_error("programa nulo");
    }

    enter_scope();

    check_program(program);

    leave_scope();
}


// PROGRAMA
static void check_program(Program *p) {
    /* Instala nome do programa como símbolo global */
    install(p->identifier, CAT_PROGRAM, TYPE_VOID, current_scope->level);

    if (p->block == NULL) {
        semantic_error("programa sem bloco");
    }

    /* Guarda referência às sub-rotinas para lookup posterior */
    global_subrot_list = p->block->subRotDeclarations;

    /* Declarações globais */
    check_var_declarations(p->block->varDeclarations, 0);

    /* Pré-declara todas as funções e procedures */
    predeclare_subroutines(p->block->subRotDeclarations);

    /* Analisa corpo das sub-rotinas */
    check_subroutines(p->block->subRotDeclarations);

    /* Analisa comandos do bloco principal */
    check_block(p->block);
}


// BLOCO
static void check_block(Block *b) {
    if (b == NULL) return;

    int dummy_return = 0;
    check_command_list(b->commandList, NULL, &dummy_return);
}


// DECLARAÇÕES DE VARIÁVEIS E PARÂMETROS
static void check_var_declarations(VarDeclaration *list, int as_params) {
    VarDeclaration *v = list;
    while (v) {
        install(v->identifier,
                as_params ? CAT_PARAM : CAT_VAR,
                varType_to_type(v->type), current_scope->level);
        v = v->next;
    }
}


// PRÉ-DECLARAÇÃO DE SUBROTINAS
static void predeclare_subroutines(SubRotDeclaration *list) {
    SubRotDeclaration *s = list;

    while (s != NULL) {
        if (s->type == Proc) {
            install(s->subrotU.procInfo.identifier,
                    CAT_PROCEDURE,
                    TYPE_VOID, current_scope->level);

        } else {
            install(s->subrotU.funcInfo.identifier,
                    CAT_FUNCTION,
                    varType_to_type(s->subrotU.funcInfo.returnType), current_scope->level);
        }

        s = s->next;
    }
}


// ANÁLISE COMPLETA DAS SUBROTINAS
static void check_subroutines(SubRotDeclaration *list) {
    while (list) {
        check_subroutine(list);
        list = list->next;
    }
}


// ANÁLISE DE SUBROTINA INDIVIDUAL
static void check_subroutine(SubRotDeclaration *srd) {

    if (srd->type == Proc) {
        char *name = srd->subrotU.procInfo.identifier;
        VarDeclaration *params = srd->subrotU.procInfo.formParams;
        SubRotBlock *body = srd->subrotU.procInfo.subRotBlock;

        enter_scope();

        /* Parâmetros */
        check_var_declarations(params, 1);

        /* Variáveis locais */
        if (body) check_var_declarations(body->varDeclarations, 0);

        /* Comandos */
        int dummy_ret = 0;
        check_subroutine_block(body, NULL, 0, &dummy_ret);

        leave_scope();

    } else {
        char *name = srd->subrotU.funcInfo.identifier;
        VarDeclaration *params = srd->subrotU.funcInfo.formParams;
        SubRotBlock *body = srd->subrotU.funcInfo.subRotBlock;
        Type ret_type = varType_to_type(srd->subrotU.funcInfo.returnType);

        enter_scope();

        /* Variável implícita de retorno */
        install(name, CAT_VAR, ret_type, current_scope->level);

        /* Parâmetros */
        check_var_declarations(params, 1);

        /* Variáveis locais */
        if (body) check_var_declarations(body->varDeclarations, 0);

        /* Comandos */
        int return_count = 0;
        check_subroutine_block(body, name, 1, &return_count);

        leave_scope();

        if (return_count == 0)
            semantic_error("função sem retorno");
        if (return_count > 1)
            semantic_error("função possui mais de um retorno");
    }
}


// ANÁLISE DO BLOCO DE UMA SUBROTINA
static void check_subroutine_block(SubRotBlock *srb,
                                   const char *func_name,
                                   int is_function,
                                   int *return_count) {
    if (!srb) return;

    check_command_list(srb->commands, func_name, return_count);
}


// LISTA DE COMANDOS
static void check_command_list(Command *list,
                               const char *current_func_name,
                               int *return_count) {
    while (list) {
        check_command(list, current_func_name, return_count);
        list = list->next;
    }
}


// COMANDO INDIVIDUAL
static void check_command(Command *cmd,
                          const char *current_func_name,
                          int *return_count) {
    if (!cmd) return;

    switch (cmd->type) {
        case Assign:
            check_assign_command(cmd, current_func_name, return_count);
            break;

        case ProcCall:
            check_proc_call_command(cmd);
            break;

        case Conditional:
            check_conditional_command(cmd, current_func_name, return_count);
            break;

        case Loop:
            check_loop_command(cmd, current_func_name, return_count);
            break;

        case Read:
            check_read_command(cmd);
            break;

        case Write:
            check_write_command(cmd);
            break;

        default:
            break;
    }
}


// ATRIBUIÇÃO
static void check_assign_command(Command *cmd,
                                 const char *current_func_name,
                                 int *return_count) {
    char *id = cmd->cmdU.assignInfo.identifier;
    Expression *expr = cmd->cmdU.assignInfo.expression;

    Symbol *sym = lookup(id);
    if (!sym)
        semantic_error("atribuição para identificador não declarado");

    if (sym->category != CAT_VAR && sym->category != CAT_PARAM)
        semantic_error("lado esquerdo da atribuição deve ser variável ou parâmetro");

    Type lhs = sym->type;
    Type rhs = check_expression(expr);

    if (lhs != rhs)
        semantic_error("tipos incompatíveis em atribuição");

    /* Contabiliza retorno em função */
    if (current_func_name &&
        strcmp(id, current_func_name) == 0) {
        (*return_count)++;
    }
}


// CHAMADA DE PROCEDURE
static void check_proc_call_command(Command *cmd) {
    char *name = cmd->cmdU.procCallInfo.identifier;
    Expression *args = cmd->cmdU.procCallInfo.expressionList;

    Symbol *sym = lookup(name);
    if (!sym)
        semantic_error("chamada de procedure não declarada");

    if (sym->category != CAT_PROCEDURE)
        semantic_error("identificador chamado como procedure não é procedure");

    /* Busca declaração na AST */
    SubRotDeclaration *s = global_subrot_list;
    while (s) {
        if (s->type == Proc &&
            strcmp(s->subrotU.procInfo.identifier, name) == 0)
            break;
        s = s->next;
    }

    if (!s)
        semantic_error("declaração da procedure não encontrada");

    check_expression_list(args, s->subrotU.procInfo.formParams);
}


// CONDICIONAL
static void check_conditional_command(Command *cmd,
                                      const char *current_func_name,
                                      int *return_count)
{
    Expression *cond = cmd->cmdU.condInfo.condExpression;

    // A condição deve ser booleana
    if (check_expression(cond) != TYPE_BOOL)
        semantic_error("expressão condicional do if deve ser booleana");

    // Vamos medir retornos separados em cada ramo
    int before = *return_count;

    // Ramo IF
    check_command_list(cmd->cmdU.condInfo.cmdIf,
                       current_func_name, return_count);
    int if_returns = (*return_count > before);

    // Ramo ELSE
    int else_returns = 0;
    if (cmd->cmdU.condInfo.cmdElse) {
        int before2 = *return_count;
        check_command_list(cmd->cmdU.condInfo.cmdElse,
                           current_func_name, return_count);
        else_returns = (*return_count > before2);
    }

    // Se for função e ambos os ramos retornam, isso conta como UM retorno garantido
    if (current_func_name && if_returns && else_returns) {
        (*return_count) = before + 1;  // colapsa os dois caminhos em 1 retorno
    }
}


// WHILE
static void check_loop_command(Command *cmd,
                               const char *current_func_name,
                               int *return_count) {
    if (check_expression(cmd->cmdU.loopInfo.loopExpression) != TYPE_BOOL)
        semantic_error("expressão condicional do while deve ser booleana");

    check_command_list(cmd->cmdU.loopInfo.cmdLoop,
                       current_func_name, return_count);
}


// READ
static void check_read_command(Command *cmd) {
    IdentifierList *id = cmd->cmdU.readInfo.identifiers;

    while (id) {
        Symbol *sym = lookup(id->identifier);

        if (!sym)
            semantic_error("identificador do read não declarado");

        if (sym->category != CAT_VAR && sym->category != CAT_PARAM)
            semantic_error("argumento de read deve ser variável ou parâmetro");

        id = id->next;
    }
}


// WRITE
static void check_write_command(Command *cmd) {
    Expression *e = cmd->cmdU.writeInfo.expressionList;

    while (e) {
        check_expression(e);
        e = e->next;
    }
}


// EXPRESSÕES E PARÂMETROS
static void check_expression_list(Expression *list,
                                  VarDeclaration *formals) {
    Expression *arg = list;
    VarDeclaration *param = formals;

    while (arg && param) {
        if (check_expression(arg) != varType_to_type(param->type))
            semantic_error("tipo do argumento não corresponde ao parâmetro");

        arg = arg->next;
        param = param->next;
    }

    if (arg || param)
        semantic_error("número de argumentos não corresponde ao de parâmetros");
}


// EXPRESSÃO GENÉRICA
static Type check_expression(Expression *e) {
    if (!e)
        semantic_error("expressão nula");

    switch (e->type) {
        case Binary:   return check_binary_expression(e);
        case Unary:    return check_unary_expression(e);
        case Var:      return check_variable_expression(e);
        case ConstInt: return TYPE_INT;
        case ConstBool:return TYPE_BOOL;
        case FuncCall: return check_function_call_expression(e);
        default:
            semantic_error("expressão desconhecida");
    }

    return TYPE_UNKNOWN;
}


// EXPRESSÃO BINÁRIA
static Type check_binary_expression(Expression *e) {

    Type lt = check_expression(e->exprU.binExpr.left);
    Type rt = check_expression(e->exprU.binExpr.right);
    Operator op = e->exprU.binExpr.operator;

    switch (op) {

        case Plus:
        case Minus:
        case Multiplication:
        case Division:
            if (lt != TYPE_INT || rt != TYPE_INT)
                semantic_error("operações aritméticas requerem inteiros");
            return TYPE_INT;

        case Less:
        case LessEqual:
        case Greater:
        case GreaterEqual:
            if (lt != TYPE_INT || rt != TYPE_INT)
                semantic_error("operações relacionais requerem inteiros");
            return TYPE_BOOL;

        case Equal:
        case Different:
            if (lt != rt)
                semantic_error("comparações requerem tipos iguais");
            return TYPE_BOOL;

        case And:
        case Or:
            if (lt != TYPE_BOOL || rt != TYPE_BOOL)
                semantic_error("operações lógicas requerem booleanos");
            return TYPE_BOOL;

        default:
            semantic_error("operador binário inválido");
    }

    return TYPE_UNKNOWN;
}


// EXPRESSÃO UNÁRIA
static Type check_unary_expression(Expression *e) {

    Operator op = e->exprU.unyExpr.operator;
    Type rt = check_expression(e->exprU.unyExpr.right);

    if (op == Not) {
        if (rt != TYPE_BOOL)
            semantic_error("not requer booleano");
        return TYPE_BOOL;
    }

    if (op == Minus) {
        if (rt != TYPE_INT)
            semantic_error("menos unário requer inteiro");
        return TYPE_INT;
    }

    semantic_error("operador unário inválido");
    return TYPE_UNKNOWN;
}


// EXPRESSÃO DE VARIÁVEL
static Type check_variable_expression(Expression *e) {

    Symbol *sym = lookup(e->exprU.varExpr.identifier);

    if (!sym)
        semantic_error("uso de variável não declarada");

    if (sym->category != CAT_VAR && sym->category != CAT_PARAM)
        semantic_error("somente variáveis ou parâmetros podem aparecer em expressões");

    return sym->type;
}

// CHAMADA DE FUNÇÃO
static Type check_function_call_expression(Expression *e) {

    char *name = e->exprU.funCallExpr.identifier;
    Expression *args = e->exprU.funCallExpr.expressionList;

    Symbol *sym = lookup(name);
    if (!sym)
        semantic_error("chamada de função não declarada");

    if (sym->category != CAT_FUNCTION)
        semantic_error("identificador chamado como função não é função");

    /* Procura a função na AST para pegar os parâmetros */
    SubRotDeclaration *s = global_subrot_list;
    while (s) {
        if (s->type == Func &&
            strcmp(s->subrotU.funcInfo.identifier, name) == 0)
            break;
        s = s->next;
    }

    if (!s)
        semantic_error("declaração da função não encontrada");

    check_expression_list(args, s->subrotU.funcInfo.formParams);

    return sym->type;
}
