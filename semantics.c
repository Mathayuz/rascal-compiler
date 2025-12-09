#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "semantics.h"
#include "rascal_ast.h"
#include "symbol_table.h"

// Global list containing all subroutines declared in the program
static SubRotDeclaration *globalSubrotList = NULL;

// Auxliar function for printing semantic error
static void semanticError(const char *msg) {
    printf("\nSemantic error: %s\n", msg);
    exit(1);
}

// Auxiliar function for conversion
static Type varTypeToType(varType vt) {
    switch (vt) {
        case Int:  return TYPE_INT;
        case Bool: return TYPE_BOOL;
        default:   return TYPE_UNKNOWN;
    }
}

// Internal Declarations
static void checkProgram(Program *p);
static void checkBlock(Block *b);
static void checkVarDeclarations(VarDeclaration *list, int asParams);
static void predeclareSubroutines(SubRotDeclaration *list);
static void checkSubroutines(SubRotDeclaration *list);
static void checkSubroutine(SubRotDeclaration *srd);
static void checkSubroutineBlock(SubRotBlock *srb, const char *funcName, int isFunction, int *returnCount);
static void checkCommandList(Command *list, const char *currentFuncName, int *returnCount);
static void checkCommand(Command *cmd, const char *currentFuncName, int *returnCount);
static void checkAssignCommand(Command *cmd, const char *currentFuncName, int *returnCount);
static void checkProcCallCommand(Command *cmd);
static void checkConditionalCommand(Command *cmd, const char *currentFuncName, int *returnCount);
static void checkLoopCommand(Command *cmd, const char *currentFuncName, int *returnCount);
static void checkReadCommand(Command *cmd);
static void checkWriteCommand(Command *cmd);
static void checkExpressionList(Expression *list, VarDeclaration *formals);
static Type checkExpression(Expression *e);
static Type checkBinaryExpression(Expression *e);
static Type checkUnaryExpression(Expression *e);
static Type checkVariableExpression(Expression *e);
static Type checkFunctionCallExpression(Expression *e);

// Main Semantic Analysis Function
void semanticCheck(Program *program) {
    if (!program) semanticError("null program.\n");

    enter_scope();
    checkProgram(program);
    leave_scope();
}

// Program
static void checkProgram(Program *p) {
    // Install program name as global symbol
    install(p->identifier, CAT_PROGRAM, TYPE_VOID, current_scope->level);

    if (!p->block) semanticError("program without block.\n");

    // Save reference to subroutines for later lookup
    globalSubrotList = p->block->subRotDeclarations;

    // Global declarations
    checkVarDeclarations(p->block->varDeclarations, 0);

    // Pre-declare all functions and procedures
    predeclareSubroutines(p->block->subRotDeclarations);

    // Analyze subroutines bodies
    checkSubroutines(p->block->subRotDeclarations);

    // Analyze main block commands
    checkBlock(p->block);
}

// Block
static void checkBlock(Block *b) {
    if (!b) return;

    int dummy_return = 0;
    checkCommandList(b->commandList, NULL, &dummy_return);
}

// Variable and parameter declarations
static void checkVarDeclarations(VarDeclaration *list, int asParams) {
    VarDeclaration *v = list;
    while (v) {
        install(v->identifier,
                asParams ? CAT_PARAM : CAT_VAR,
                varTypeToType(v->type), current_scope->level);
        v = v->next;
    }
}

// Subroutine pre-declaration
static void predeclareSubroutines(SubRotDeclaration *list) {
    SubRotDeclaration *s = list;

    while (s != NULL) {
        if (s->type == Proc) {
            install(s->subrotU.procInfo.identifier, CAT_PROCEDURE, TYPE_VOID, current_scope->level);

        } else {
            install(s->subrotU.funcInfo.identifier, CAT_FUNCTION, varTypeToType(s->subrotU.funcInfo.returnType), current_scope->level);
        }

        s = s->next;
    }
}


// Check all subroutines
static void checkSubroutines(SubRotDeclaration *list) {
    while (list) {
        checkSubroutine(list);
        list = list->next;
    }
}


// Individual subroutine
static void checkSubroutine(SubRotDeclaration *srd) {
    if (srd->type == Proc) {
        VarDeclaration *params = srd->subrotU.procInfo.formParams;
        SubRotBlock *body = srd->subrotU.procInfo.subRotBlock;

        enter_scope();

        // Parameters
        checkVarDeclarations(params, 1);

        // Local variables
        if (body) checkVarDeclarations(body->varDeclarations, 0);

        // Commands
        int dummy_ret = 0;
        checkSubroutineBlock(body, NULL, 0, &dummy_ret);

        leave_scope();

    } else {
        char *name = srd->subrotU.funcInfo.identifier;
        VarDeclaration *params = srd->subrotU.funcInfo.formParams;
        SubRotBlock *body = srd->subrotU.funcInfo.subRotBlock;
        Type ret_type = varTypeToType(srd->subrotU.funcInfo.returnType);

        enter_scope();

        // Implicit return variable
        install(name, CAT_VAR, ret_type, current_scope->level);

        // Parameters
        checkVarDeclarations(params, 1);

        // local variables
        if (body) checkVarDeclarations(body->varDeclarations, 0);

        // Commands
        int return_count = 0;
        checkSubroutineBlock(body, name, 1, &return_count);

        leave_scope();

        if (return_count == 0)
            semanticError("function without return.\n");
        if (return_count > 1)
            semanticError("function has more than one return.\n");
    }
}


// Subroutine block
static void checkSubroutineBlock(SubRotBlock *srb, const char *funcName, int isFunction, int *returnCount) {
    if (!srb) return;

    checkCommandList(srb->commands, funcName, returnCount);
}


// Command list
static void checkCommandList(Command *list, const char *currentFuncName, int *returnCount) {
    while (list) {
        checkCommand(list, currentFuncName, returnCount);
        list = list->next;
    }
}


// Individual command
static void checkCommand(Command *cmd, const char *currentFuncName, int *returnCount) {
    if (!cmd) return;

    switch (cmd->type) {
        case Assign:
            checkAssignCommand(cmd, currentFuncName, returnCount);
            break;

        case ProcCall:
            checkProcCallCommand(cmd);
            break;

        case Conditional:
            checkConditionalCommand(cmd, currentFuncName, returnCount);
            break;

        case Loop:
            checkLoopCommand(cmd, currentFuncName, returnCount);
            break;

        case Read:
            checkReadCommand(cmd);
            break;

        case Write:
            checkWriteCommand(cmd);
            break;

        default:
            break;
    }
}


// Assignment
static void checkAssignCommand(Command *cmd, const char *currentFuncName, int *returnCount) {
    char *id = cmd->cmdU.assignInfo.identifier;
    Expression *expr = cmd->cmdU.assignInfo.expression;

    Symbol *sym = lookup(id);
    if (!sym) semanticError("assignment for undeclared identifier.\n");

    if (sym->category != CAT_VAR && sym->category != CAT_PARAM)
        semanticError("left side of the assignment must be a variable or parameter.\n");

    Type lhs = sym->type;
    Type rhs = checkExpression(expr);

    if (lhs != rhs)
        semanticError("incompatible types in assignment.\n");

    // Count returns for functions
    if (currentFuncName && strcmp(id, currentFuncName) == 0) {
        (*returnCount)++;
    }
}


// Procedure Call
static void checkProcCallCommand(Command *cmd) {
    char *name = cmd->cmdU.procCallInfo.identifier;
    Expression *args = cmd->cmdU.procCallInfo.expressionList;

    Symbol *sym = lookup(name);
    if (!sym) semanticError("not declared procedure call.\n");

    if (sym->category != CAT_PROCEDURE)
        semanticError("identifier called as a procedure is not a procedure.\n");

    // Search declaration in AST
    SubRotDeclaration *s = globalSubrotList;
    while (s) {
        if (s->type == Proc && strcmp(s->subrotU.procInfo.identifier, name) == 0)
            break;
        s = s->next;
    }

    if (!s) semanticError("declaration of the procedure was not found.\n");

    checkExpressionList(args, s->subrotU.procInfo.formParams);
}


// Conditional
static void checkConditionalCommand(Command *cmd, const char *currentFuncName, int *returnCount) {
    Expression *cond = cmd->cmdU.condInfo.condExpression;

    // Condition must be boolean
    if (checkExpression(cond) != TYPE_BOOL)
        semanticError("IF's conditional expression must result in a boolean result.\n");

    // Measure returns separately in each branch
    int before = *returnCount;

    // If branch
    checkCommandList(cmd->cmdU.condInfo.cmdIf, currentFuncName, returnCount);
    int if_returns = (*returnCount > before);

    // Else branch
    int else_returns = 0;
    if (cmd->cmdU.condInfo.cmdElse) {
        int before2 = *returnCount;
        checkCommandList(cmd->cmdU.condInfo.cmdElse, currentFuncName, returnCount);
        else_returns = (*returnCount > before2);
    }

    // If it's a function and both branches return, count as one guaranteed return
    if (currentFuncName && if_returns && else_returns) {
        (*returnCount) = before + 1;  // Collapse both paths into 1 return
    }
}


// While
static void checkLoopCommand(Command *cmd, const char *currentFuncName, int *returnCount) {
    if (checkExpression(cmd->cmdU.loopInfo.loopExpression) != TYPE_BOOL)
        semanticError("WHILE's conditional expression must result in a boolean result.\n");

    checkCommandList(cmd->cmdU.loopInfo.cmdLoop, currentFuncName, returnCount);
}


// Read
static void checkReadCommand(Command *cmd) {
    IdentifierList *id = cmd->cmdU.readInfo.identifiers;

    while (id) {
        Symbol *sym = lookup(id->identifier);

        if (!sym)
            semanticError("read identifier was not declared.\n");

        if (sym->category != CAT_VAR && sym->category != CAT_PARAM)
            semanticError("argument of READ must be a variable or a parameter.\n");

        id = id->next;
    }
}


// Write
static void checkWriteCommand(Command *cmd) {
    Expression *e = cmd->cmdU.writeInfo.expressionList;

    while (e) {
        checkExpression(e);
        e = e->next;
    }
}


// EXPRESSÕES E PARÂMETROS
static void checkExpressionList(Expression *list, VarDeclaration *formals) {
    Expression *arg = list;
    VarDeclaration *param = formals;

    while (arg && param) {
        if (checkExpression(arg) != varTypeToType(param->type))
            semanticError("argument type does not match the parameter.\n");

        arg = arg->next;
        param = param->next;
    }

    if (arg || param)
        semanticError("number of arguments does not match the number of parameters.\n");
}


// Generic expression
static Type checkExpression(Expression *e) {
    if (!e) semanticError("null expression.\n");

    switch (e->type) {
        case Binary:   return checkBinaryExpression(e);
        case Unary:    return checkUnaryExpression(e);
        case Var:      return checkVariableExpression(e);
        case ConstInt: return TYPE_INT;
        case ConstBool:return TYPE_BOOL;
        case FuncCall: return checkFunctionCallExpression(e);
        default:
            semanticError("unknown expression.\n");
    }

    return TYPE_UNKNOWN;
}


// Binary expression
static Type checkBinaryExpression(Expression *e) {
    Type lt = checkExpression(e->exprU.binExpr.left);
    Type rt = checkExpression(e->exprU.binExpr.right);
    Operator op = e->exprU.binExpr.operator;

    switch (op) {

        case Plus:
        case Minus:
        case Multiplication:
        case Division:
            if (lt != TYPE_INT || rt != TYPE_INT)
                semanticError("arithmetic operations require integers.\n");
            return TYPE_INT;

        case Less:
        case LessEqual:
        case Greater:
        case GreaterEqual:
            if (lt != TYPE_INT || rt != TYPE_INT)
                semanticError("relational operations require integers.\n");
            return TYPE_BOOL;

        case Equal:
        case Different:
            if (lt != rt)
                semanticError("comparison operations require identical types.\n");
            return TYPE_BOOL;

        case And:
        case Or:
            if (lt != TYPE_BOOL || rt != TYPE_BOOL)
                semanticError("logical operations require booleans.\n");
            return TYPE_BOOL;

        default:
            semanticError("invalid binary operator.\n");
    }

    return TYPE_UNKNOWN;
}


// Unary expression
static Type checkUnaryExpression(Expression *e) {

    Operator op = e->exprU.unyExpr.operator;
    Type rt = checkExpression(e->exprU.unyExpr.right);

    if (op == Not) {
        if (rt != TYPE_BOOL)
            semanticError("NOT operation requires boolean.\n");
        return TYPE_BOOL;
    }

    if (op == Minus) {
        if (rt != TYPE_INT)
            semanticError("MINUS operation requires integer.\n");
        return TYPE_INT;
    }

    semanticError("invalid unary operator.\n");
    return TYPE_UNKNOWN;
}


// Variable expression
static Type checkVariableExpression(Expression *e) {
    Symbol *sym = lookup(e->exprU.varExpr.identifier);
    if (!sym) semanticError("use of not declared variable.\n");

    if (sym->category != CAT_VAR && sym->category != CAT_PARAM)
        semanticError("only variables or parameters can appear in expressions.\n");

    return sym->type;
}

// Function call
static Type checkFunctionCallExpression(Expression *e) {
    char *name = e->exprU.funCallExpr.identifier;
    Expression *args = e->exprU.funCallExpr.expressionList;

    Symbol *sym = lookup(name);
    if (!sym) semanticError("not declared function call.\n");

    if (sym->category != CAT_FUNCTION)
        semanticError("identifier called as a function is not a function.\n");

    // Search function in AST to get parameters
    SubRotDeclaration *s = globalSubrotList;
    while (s) {
        if (s->type == Func && strcmp(s->subrotU.funcInfo.identifier, name) == 0)
            break;
        s = s->next;
    }

    if (!s) semanticError("function was not found.\n");

    checkExpressionList(args, s->subrotU.funcInfo.formParams);

    return sym->type;
}
