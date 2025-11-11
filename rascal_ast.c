#include "rascal_ast.h"
#include <stdlib.h>
#include <string.h>

// - Constructors -------------------------

// Program Node Constructor
Program* newProgram(char* identifier, Block* block) {
    Program* p = (Program*)malloc(sizeof(Program));
    p->identifier = identifier;
    p->block = block;
    return p;
}

// Block Node Constructor
Block* newBlock(VarDeclaration* varDeclarations, SubRotDeclaration* subRotDeclarations, Command* commandList) {
    Block* b = (Block*)malloc(sizeof(Block));
    b->varDeclarations = varDeclarations;
    b->subRotDeclarations = subRotDeclarations;
    b->commandList = commandList;
    return b;
}

// Variable Declaration Node Constructor
VarDeclaration* newVarDeclaration(varType type, char* identifier) {
    VarDeclaration* vd = (VarDeclaration*)malloc(sizeof(VarDeclaration));
    vd->type = type;
    vd->identifier = identifier;
    vd->next = NULL;
    return vd;
}

// Identifier Node Constructor
IdentifierList* newIdentifierList(char* identifier) {
    IdentifierList* il = (IdentifierList*)malloc(sizeof(IdentifierList));
    il->identifier = identifier;
    il->next = NULL;
    return il;
}

// Procedure Declaration Node Constructor
SubRotDeclaration* newProcDeclaration(char* identifier, VarDeclaration* formParams) {
    SubRotDeclaration* pd = (SubRotDeclaration*)malloc(sizeof(SubRotDeclaration));
    pd->type = Proc;
    pd->subrotU.procInfo.identifier = identifier;
    pd->subrotU.procInfo.formParams = formParams;
    pd->next = NULL;
    return pd;
}

// Function Declaration Node Constructor
SubRotDeclaration* newFuncDeclaration(char* identifier, VarDeclaration* formParams, varType returnType) {
    SubRotDeclaration* fd = (SubRotDeclaration*)malloc(sizeof(SubRotDeclaration));
    fd->type = Func;
    fd->subrotU.funcInfo.identifier = identifier;
    fd->subrotU.funcInfo.formParams = formParams;
    fd->subrotU.funcInfo.returnType = returnType;
    fd->next = NULL;
    return fd;
}

// Assign Command Node Constructor
Command* newAssignCommand(char* identifier, Expression* expression) {
    Command* ac = (Command*)malloc(sizeof(Command));
    ac->type = Assign;
    ac->cmdU.assignInfo.identifier = identifier;
    ac->cmdU.assignInfo.expression = expression;
    ac->next = NULL;
    return ac;
}

// Procedure Call Command Node Constructor
Command* newProcCallCommand(char* identifier, Expression* expressions) {
    Command* pc = (Command*)malloc(sizeof(Command));
    pc->type = ProcCall;
    pc->cmdU.procCallInfo.identifier = identifier;
    pc->cmdU.procCallInfo.expressions = expressions;
    pc->next = NULL;
    return pc;
}

// Conditional Command Node Constructor
Command* newCondCommand(Expression* condExpression, Command* cmdIf, Command* cmdElse) {
    Command* cc = (Command*)malloc(sizeof(Command));
    cc->type = Conditional;
    cc->cmdU.condInfo.condExpression = condExpression;
    cc->cmdU.condInfo.cmdIf = cmdIf;
    cc->cmdU.condInfo.cmdElse = cmdElse;
    cc->next = NULL;
    return cc;
}

// Looping Command Node Constructor
Command* newLoopCommand(Expression* loopExpression, Command* cmdLoop) {
    Command* lc = (Command*)malloc(sizeof(Command));
    lc->type = Loop;
    lc->cmdU.loopInfo.loopExpression = loopExpression;
    lc->cmdU.loopInfo.cmdLoop = cmdLoop;
    lc->next = NULL;
    return lc;
}

// Read Command Node Constructor
Command* newReadCommand(IdentifierList* identifiers) {
    Command* rc = (Command*)malloc(sizeof(Command));
    rc->type = Read;
    rc->cmdU.readInfo.identifiers = identifiers;
    rc->next = NULL;
    return rc;
}

// Write Command Node Constructor
Command* newWriteCommand(Expression* expressions) {
    Command* wc = (Command*)malloc(sizeof(Command));
    wc->type = Write;
    wc->cmdU.writeInfo.expressions = expressions;
    wc->next = NULL;
    return wc;
}

// Single Expression Node Constructor
Expression* newSingleExpression(SimpleExpression* simpleExpression) {
    Expression* se = (Expression*)malloc(sizeof(Expression));
    se->type = SingleExpression;
    se->exprU.singExp.simple = simpleExpression;
    se->next = NULL;
    return se;
}

// Relational Expression Node Constructor
Expression* newRelationalExpression(SimpleExpression* left, relationalOperation relOperator, SimpleExpression* right) {
    Expression* re = (Expression*)malloc(sizeof(Expression));
    re->type = RelationalExpression;
    re->exprU.relExp.left = left;
    re->exprU.relExp.relOperator = relOperator;
    re->exprU.relExp.right = right;
    re->next = NULL;
    return re;
}

// Single Simple Expression Node Constructor
SimpleExpression* newSingleSimpleExpression(Term* term) {
    SimpleExpression* sse = (SimpleExpression*)malloc(sizeof(SimpleExpression));
    sse->type = SingleSimpleExpression;
    sse->simpExprU.singleSimpExpr.term = term;
    return sse;
}

// Compound Simple Expression Node Constructor
SimpleExpression* newCompoundSimpleExpression(Term* left, termOperation termOperator, SimpleExpression* right) {
    SimpleExpression* cse = (SimpleExpression*)malloc(sizeof(SimpleExpression));
    cse->type = CompoundSimpleExpression;
    cse->simpExprU.compoundSimpExpr.left = left;
    cse->simpExprU.compoundSimpExpr.termOperator = termOperator;
    cse->simpExprU.compoundSimpExpr.right = right;
    return cse;
}

// Simple Term Node Constructor
Term* newSingleTerm(Factor* factor) {
    Term* st = (Term*)malloc(sizeof(Term));
    st->type = SingleTerm;
    st->termU.singleTerm.factor = factor;
    return st;
}

// Compound Term Node Constructor
Term* newCompoundTerm(Factor* left, factorOperation factOperator, Term* right) {
    Term* ct = (Term*)malloc(sizeof(Term));
    ct->type = CompoundTerm;
    ct->termU.compoundTerm.left = left;
    ct->termU.compoundTerm.factOperator = factOperator;
    ct->termU.compoundTerm.right = right;
    return ct;
}

// Variable Factor Node Constructor
Factor* newVariableFactor(char* identifier) {
    Factor* vf = (Factor*)malloc(sizeof(Factor));
    vf->type = Variable;
    vf->facU.var.identifier = identifier;
    return vf;
}

// Number Factor Node Constructor
Factor* newNumberFactor(int value) {
    Factor* nf = (Factor*)malloc(sizeof(Factor));
    nf->type = Number;
    nf->facU.num.value = value;
    return nf;
}

// Boolean Factor Node Constructor
Factor* newBooleanFactor(int logicalValue) {
    Factor* bf = (Factor*)malloc(sizeof(Factor));
    bf->type = Boolean;
    bf->facU.boolean.logicalValue = logicalValue;
    return bf;
}

// Function Call Factor Node Constructor
Factor* newFuncCallFactor(char* funcName, Expression* arguments) {
    Factor* fcf = (Factor*)malloc(sizeof(Factor));
    fcf->type = FunctionCall;
    fcf->facU.funcCall.funcName = funcName;
    fcf->facU.funcCall.arguments = arguments;
    return fcf;
}

// Expression Factor Node Constructor
Factor* newExpressionFactor(Expression* expression) {
    Factor* ef = (Factor*)malloc(sizeof(Factor));
    ef->type = ExpressionFT;
    ef->facU.expr.expression = expression;
    return ef;
}

// Unary Operator Factor Node Constructor
Factor* newUnaryOperatorFactor(unaryOperation unOperator, Factor* operand) {
    Factor* uf = (Factor*)malloc(sizeof(Factor));
    uf->type = Unary;
    uf->facU.unarFac.unOperator = unOperator;
    uf->facU.unarFac.operand = operand;
    return uf;
}

// - Add to List Functions ----------------

// Variable Declaration AddToList Function
VarDeclaration* addVarDeclaration(VarDeclaration* list, VarDeclaration* newVarDecl) {
    if(!list) return newVarDecl;
    VarDeclaration* it = list;
    while(it->next) it = it->next;
    it->next = newVarDecl;
    return list;
}

// Identifier AddToList Function
IdentifierList* addIdentifier(IdentifierList* list, IdentifierList* newId) {
    if(!list) return newId;
    IdentifierList* it = list;
    while(it->next) it = it->next;
    it->next = newId;
    return list;
}

// Subroutine Declaration AddToList Function
SubRotDeclaration* addSubRotDeclaration(SubRotDeclaration* list, SubRotDeclaration* newSubRotDecl) {
    if(!list) return newSubRotDecl;
    SubRotDeclaration* it = list;
    while(it->next) it = it->next;
    it->next = newSubRotDecl;
    return list;
}

// Command AddToList Function
Command* addCommand(Command* list, Command* newCmd) {
    if(!list) return newCmd;
    Command* it = list;
    while(it->next) it = it->next;
    it->next = newCmd;
    return list;
}

// Expression AddToList Function
Expression* addExpression(Expression* list, Expression* newExpr) {
    if(!list) return newExpr;
    Expression* it = list;
    while(it->next) it = it->next;
    it->next = newExpr;
    return list;
}

// - Free ---------------------------------

// Free Program Node Function
void freeProgram(Program* p) {
    if (!p) return;
    free(p->identifier);
    freeBlock(p->block);
    free(p);
}

// Free Block Node Function
void freeBlock(Block* b) {
    if (!b) return;
    freeVarDeclaration(b->varDeclarations);
    freeSubRotDeclaration(b->subRotDeclarations);
    freeCommand(b->commandList);
    free(b);
}

// Free Var Declaration Node Function
void freeVarDeclaration(VarDeclaration* vd) {
    while (vd) {
        VarDeclaration* next = vd->next;
        free(vd->identifier);
        free(vd);
        vd = next;
    }
}

// Free Identifier Node Function
void freeIdentifierList(IdentifierList* il) {
    while (il) {
        IdentifierList* next = il->next;
        free(il->identifier);
        free(il);
        il = next;
    }
}

// Free Subroutine Declaration Node Function
void freeSubRotDeclaration(SubRotDeclaration* srd) {
    while (srd) {
        SubRotDeclaration* next = srd->next;
        switch (srd->type) {
            case Proc:
                free(srd->subrotU.procInfo.identifier);
                freeVarDeclaration(srd->subrotU.procInfo.formParams);
                break;
            case Func:
                free(srd->subrotU.funcInfo.identifier);
                freeVarDeclaration(srd->subrotU.funcInfo.formParams);
                break;
        }
        free(srd);
        srd = next;
    }
}

// Free Command Node Function
void freeCommand(Command* c) {
    while (c) {
        Command* next = c->next;
        switch (c->type) {
            case Assign:
                free(c->cmdU.assignInfo.identifier);
                freeExpression(c->cmdU.assignInfo.expression);
                break;
            case ProcCall:
                free(c->cmdU.procCallInfo.identifier);
                freeExpression(c->cmdU.procCallInfo.expressions);
                break;
            case Conditional:
                freeExpression(c->cmdU.condInfo.condExpression);
                freeCommand(c->cmdU.condInfo.cmdIf);
                freeCommand(c->cmdU.condInfo.cmdElse);
                break;
            case Loop:
                freeExpression(c->cmdU.loopInfo.loopExpression);
                freeCommand(c->cmdU.loopInfo.cmdLoop);
                break;
            case Read:
                freeIdentifierList(c->cmdU.readInfo.identifiers);
                break;
            case Write:
                freeExpression(c->cmdU.writeInfo.expressions);
                break;
        }
        free(c);
        c = next;
    }
}

// Free Expression Node Function
void freeExpression(Expression* e) {
    while (e) {
        Expression* next = e->next;
        switch (e->type) {
            case SingleExpression:
                freeSimpleExpression(e->exprU.singExp.simple);    
                break;
            case RelationalExpression:
                freeSimpleExpression(e->exprU.relExp.left);
                freeSimpleExpression(e->exprU.relExp.right);
                break;
        }
        free(e);
        e = next;
    }
}

// Free Simple Expression Node Function
void freeSimpleExpression(SimpleExpression* se) {
    if (!se) return;
    switch (se->type) {
        case SingleSimpleExpression:
            freeTerm(se->simpExprU.singleSimpExpr.term);
            break;
        case CompoundSimpleExpression:
            freeTerm(se->simpExprU.compoundSimpExpr.left);
            freeSimpleExpression(se->simpExprU.compoundSimpExpr.right);
            break;
    }
    free(se);
}

// Free Term Node Function
void freeTerm(Term* t) {
    if (!t) return;
    switch (t->type) {
        case SingleTerm:
            freeFactor(t->termU.singleTerm.factor);
            break;
        case CompoundTerm:
            freeFactor(t->termU.compoundTerm.left);
            freeTerm(t->termU.compoundTerm.right);
            break;
    }
    free(t);
}

// Free Factor Node Function
void freeFactor(Factor* f) {
    if (!f) return;
    switch (f->type) {
        case Variable:
            free(f->facU.var.identifier);
            break;
        case Number:
            break;
        case Boolean:
            break;
        case FunctionCall:
            free(f->facU.funcCall.funcName);
            freeExpression(f->facU.funcCall.arguments);
            break;
        case ExpressionFT:
            freeExpression(f->facU.expr.expression);
            break;
        case Unary:
            freeFactor(f->facU.unarFac.operand);
            break;
    }
    free(f);
}

// - Print --------------------------------

// Program Node Print Function
void printProgram(const Program* program, FILE* out) {
    fprintf(out, "(program %s\n", program->identifier);
    printBlock(program->block, out);
    fprintf(out, ")\n");
}

// Block Node Print Function
void printBlock(const Block* block, FILE* out) {
    fprintf(out, "(block \n(variable delcarations ");
    printVarDeclaration(block->varDeclarations, out);
    fprintf(out, ")\n(subroutine declarations ");
    printSubRotDeclaration(block->subRotDeclarations, out);
    fprintf(out, ")\n(command list ");
    printCommand(block->commandList, out);
    fprintf(out, ")\n)\n");
}

// Variable Delcaration Node Print Function
void printVarDeclaration(const VarDeclaration* varDecaration, FILE* out) {
    for (; varDecaration; varDecaration = varDecaration->next) {
        switch (varDecaration->type) {
            case Int:
                fprintf(out, "(int %s)", varDecaration->identifier);
                break;
            case Bool:
                fprintf(out, "(boolean %s)", varDecaration->identifier);
                break;
        }
    }
}

// Identifier List Node Print Function
void printIdentifierList(const IdentifierList* identifierList, FILE* out) {
    fprintf(out, "(");
    for (; identifierList; identifierList = identifierList->next) {
        fprintf(out, "identifier %s ", identifierList->identifier);
    }
    fprintf(out, ")");
}

// Subroutine Declaration Node Print Function
void printSubRotDeclaration(const SubRotDeclaration* subRotDeclaration, FILE* out) {
    for (; subRotDeclaration; subRotDeclaration = subRotDeclaration->next) {
        switch (subRotDeclaration->type) {
            case Proc:
                fprintf(out, "(procedure %s (formal parameters ", subRotDeclaration->subrotU.procInfo.identifier);
                printVarDeclaration(subRotDeclaration->subrotU.procInfo.formParams, out);
                fprintf(out, ") )");
                break;
            case Func:
                switch (subRotDeclaration->subrotU.funcInfo.returnType) {
                    case Int:
                        fprintf(out, "(function %s -> int (formal parameters ", subRotDeclaration->subrotU.funcInfo.identifier);
                        break;
                    case Bool:
                        fprintf(out, "(function %s -> bool (formal parameters ", subRotDeclaration->subrotU.funcInfo.identifier);
                        break;
                }
                printVarDeclaration(subRotDeclaration->subrotU.funcInfo.formParams, out);
                fprintf(out, ") )");
                break;
        }
    }
}

// Command Node Print Function
void printCommand(const Command* command, FILE* out) {
    for (; command; command = command->next) {
        switch (command->type) {
            case Assign:
                fprintf(out, "(%s assign command: ", command->cmdU.assignInfo.identifier);
                printExpression(command->cmdU.assignInfo.expression, out);
                fprintf(out, ")\n");
                break;
            case ProcCall:
                fprintf(out, "(%s procedure call command: ", command->cmdU.procCallInfo.identifier);
                printExpression(command->cmdU.procCallInfo.expressions, out);
                fprintf(out, ")\n");
                break;
            case Conditional:
                fprintf(out, "(conditional command - if (");
                printExpression(command->cmdU.condInfo.condExpression, out);
                fprintf(out, ")\ndo (");
                printCommand(command->cmdU.condInfo.cmdIf, out);
                fprintf(out, ")\nelse (");
                printCommand(command->cmdU.condInfo.cmdElse, out);
                fprintf(out, ")\n");
                break;
            case Loop:
                fprintf(out, "(loop command - while (");
                printExpression(command->cmdU.loopInfo.loopExpression, out);
                fprintf(out, ")\ndo(");
                printCommand(command->cmdU.loopInfo.cmdLoop, out);
                fprintf(out, ")\n");
                break;
            case Read:
                fprintf(out, "(read command: ");
                printIdentifierList(command->cmdU.readInfo.identifiers, out);
                fprintf(out, ")\n");
                break;
            case Write:
                fprintf(out, "(write command: ");
                printExpression(command->cmdU.writeInfo.expressions, out);
                fprintf(out, ")\n");
                break;
        }
    }
}

// Expression Node Print Function
void printExpression(const Expression* expression, FILE* out) {
    for (; expression; expression = expression->next) {
        switch (expression->type) {
            case SingleExpression:
                printSimpleExpression(expression->exprU.singExp.simple, out);
                break;
            case RelationalExpression:
                fprintf(out, "(relational expression: ");
                printSimpleExpression(expression->exprU.relExp.left, out);
                const char *op;
                switch (expression->exprU.relExp.relOperator) {
                    case Equal:
                        op = "=";
                        break;
                    case Different:
                        op = "!=";
                        break;
                    case Less:
                        op = "<";
                        break;
                    case LessEqual:
                        op = "<=";
                        break;
                    case Greater:
                        op = ">";
                        break;
                    case GreaterEqual:
                        op = ">=";
                        break;
                }
                fprintf(out, " %s ", op);
                printSimpleExpression(expression->exprU.relExp.right, out);
                fprintf(out, ")");
                break;
        }
    }
}

// Simple Expression Node Print Function
void printSimpleExpression(const SimpleExpression* simpleExpression, FILE* out) {
    switch (simpleExpression->type) {
        case SingleSimpleExpression:
            printTerm(simpleExpression->simpExprU.singleSimpExpr.term, out);
            break;
        case CompoundSimpleExpression:
            fprintf(out, "(compound simple expression: ");
            printTerm(simpleExpression->simpExprU.compoundSimpExpr.left, out);
            const char *op;
            switch (simpleExpression->simpExprU.compoundSimpExpr.termOperator) {
                case Plus:
                    op = "+";
                    break;
                case Minus:
                    op = "-";
                    break;
                case Or:
                    op = "OR";
                    break;
            }
            fprintf(out, " %s ", op);
            printTerm(simpleExpression->simpExprU.compoundSimpExpr.right, out);
            fprintf(out, ")");
            break;
    }
}

// Term Node Print Function
void printTerm(const Term* term, FILE* out) {
    switch (term->type) {
        case SingleTerm:
            printFactor(term->termU.singleTerm.factor, out);
            break;
        case CompoundTerm:
            fprintf(out, "(compound term: ");
            printFactor(term->termU.compoundTerm.left, out);
            const char *op;
            switch (term->termU.compoundTerm.factOperator) {
                case Multiplication:
                    op = "*";
                    break;
                case Division:
                    op = "/";
                    break;
                case And:
                    op = "AND";
                    break;
            }
            fprintf(out, " %s ", op);
            printFactor(term->termU.compoundTerm.right, out);
            fprintf(out, ")");
            break;
    }
}

// Factor Node Print Function
void printFactor(const Factor* factor, FILE* out) {
    switch (factor->type) {
        case Variable:
            fprintf(out, "(variable factor: %s)", factor->facU.var.identifier);
            break;
        case Number:
            fprintf(out, "(number factor: %i)", factor->facU.num.value);
            break;
        case Boolean:
            fprintf(out, "(boolean factor: %i)", factor->facU.boolean.logicalValue);
            break;
        case FunctionCall:
            fprintf(out, "(%s function call factor - arguments: ", factor->facU.funcCall.funcName);
            printExpression(factor->facU.funcCall.arguments, out);
            fprintf(out, ")");
            break;
        case ExpressionFT:
            fprintf(out, "(expression factor: ");
            printExpression(factor->facU.expr.expression, out);
            fprintf(out, ")");
            break;
        case Unary:
            fprintf(out, "(unary operation factor: ");
            const char *op;
            switch (factor->facU.unarFac.unOperator) {
                case Negative:
                    op = "-";
                    break;
                case Not:
                    op = "NOT";
                    break;
            }
            fprintf(out, "%s ", op);
            printFactor(factor->facU.unarFac.operand, out);
            fprintf(out, ")");
            break;
    }
}