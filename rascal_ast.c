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
SubRotDeclaration* newProcDeclaration(char* identifier, VarDeclaration* formParams, SubRotBlock* subRotBlock) {
    SubRotDeclaration* pd = (SubRotDeclaration*)malloc(sizeof(SubRotDeclaration));
    pd->type = Proc;
    pd->subrotU.procInfo.identifier = identifier;
    pd->subrotU.procInfo.formParams = formParams;
    pd->subrotU.procInfo.subRotBlock = subRotBlock;
    pd->next = NULL;
    return pd;
}

// Function Declaration Node Constructor
SubRotDeclaration* newFuncDeclaration(char* identifier, VarDeclaration* formParams, varType returnType, SubRotBlock* subRotBlock) {
    SubRotDeclaration* fd = (SubRotDeclaration*)malloc(sizeof(SubRotDeclaration));
    fd->type = Func;
    fd->subrotU.funcInfo.identifier = identifier;
    fd->subrotU.funcInfo.formParams = formParams;
    fd->subrotU.funcInfo.returnType = returnType;
    fd->subrotU.funcInfo.subRotBlock = subRotBlock;
    fd->next = NULL;
    return fd;
}

// Subroutine Block Node Constructor
SubRotBlock* newSubRotBlock(VarDeclaration* varDeclarations, Command* commands) {
    SubRotBlock* srb = (SubRotBlock*)malloc(sizeof(SubRotBlock));
    srb->varDeclarations = varDeclarations;
    srb->commands = commands;
    return srb;
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
Command* newProcCallCommand(char* identifier, Expression* expressionList) {
    Command* pc = (Command*)malloc(sizeof(Command));
    pc->type = ProcCall;
    pc->cmdU.procCallInfo.identifier = identifier;
    pc->cmdU.procCallInfo.expressionList = expressionList;
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
Command* newWriteCommand(Expression* expressionList) {
    Command* wc = (Command*)malloc(sizeof(Command));
    wc->type = Write;
    wc->cmdU.writeInfo.expressionList = expressionList;
    wc->next = NULL;
    return wc;
}

// Binary Expression Node Constructor
Expression* newBinaryExpression(Expression* left, Operator operator, Expression* right) {
    Expression* e = (Expression*)malloc(sizeof(Expression));
    e->type = Binary;
    e->exprU.binExpr.left = left;
    e->exprU.binExpr.operator = operator;
    e->exprU.binExpr.right = right;
    e->next = NULL;
    return e;
}

// Unary Expression Node Constructor
Expression* newUnaryExpression(Operator operator, Expression* right) {
    Expression* e = (Expression*)malloc(sizeof(Expression));
    e->type = Unary;
    e->exprU.unyExpr.operator = operator;
    e->exprU.unyExpr.right = right;
    e->next = NULL;
    return e;
}

// Variable Expression Node Constructor
Expression* newVariableExpression(char* identifier) {
    Expression* e = (Expression*)malloc(sizeof(Expression));
    e->type = Var;
    e->exprU.varExpr.identifier = identifier;
    e->next = NULL;
    return e;
}

// Constant Integer Expression Node Constructor
Expression* newConstantIntegerExpression(int number) {
    Expression* e = (Expression*)malloc(sizeof(Expression));
    e->type = ConstInt;
    e->exprU.intExpr.number = number;
    e->next = NULL;
    return e;
}

// Constant Boolean Expression Node Constructor
Expression* newConstantBooleanExpression(BooleanValue boolean) {
    Expression* e = (Expression*)malloc(sizeof(Expression));
    e->type = ConstBool;
    e->exprU.boolExpr.boolean = boolean;
    e->next = NULL;
    return e;
}

// Function Call Expression Node Constructor
Expression* newFunctionCallExpression(char* identifier, Expression* expressionList) {
    Expression* e = (Expression*)malloc(sizeof(Expression));
    e->type = FuncCall;
    e->exprU.funCallExpr.identifier = identifier;
    e->exprU.funCallExpr.expressionList = expressionList;
    e->next = NULL;
    return e;
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

// Free Abstract Syntax Tree Function
void freeAstRoot(Program* r) {
    if (!r) return;
    freeProgram(r);
}

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
                freeSubRotBlock(srd->subrotU.procInfo.subRotBlock);
                break;
            case Func:
                free(srd->subrotU.funcInfo.identifier);
                freeVarDeclaration(srd->subrotU.funcInfo.formParams);
                freeSubRotBlock(srd->subrotU.funcInfo.subRotBlock);
                break;
        }
        free(srd);
        srd = next;
    }
}

// Free Subroutine Block Node Function
void freeSubRotBlock(SubRotBlock* srb) {
    freeVarDeclaration(srb->varDeclarations);
    freeCommand(srb->commands);
    free(srb);
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
                freeExpression(c->cmdU.procCallInfo.expressionList);
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
                freeExpression(c->cmdU.writeInfo.expressionList);
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
            case Binary:
                freeExpression(e->exprU.binExpr.left);
                freeExpression(e->exprU.binExpr.right);
                break;
            case Unary:
                freeExpression(e->exprU.unyExpr.right);
                break;
            case Var:
                free(e->exprU.varExpr.identifier);
                break;
            case FuncCall:
                free(e->exprU.funCallExpr.identifier);
                freeExpression(e->exprU.funCallExpr.expressionList);
                break;
            case ConstInt:
                break;
            case ConstBool:
                break;
        }
        free(e);
        e = next;
    }
}

// - Print --------------------------------

// Auxiliary Function For Indentation
void printIndent(FILE* out, int level) {
    for (int i = 0; i < level; i++) {
        fprintf(out, "|  ");
    }
}

// Abstract Syntax Tree Print Function
void printAstRoot(Program* ast_root, FILE* out) {
    if (!ast_root) return;
    printProgram(ast_root, out, 0);
}

// Program Node Print Function
void printProgram(const Program* program, FILE* out, int level) {
    if (!program) return;
    printIndent(out, level);
    fprintf(out, "[Program Node]\n");
    printIndent(out, level + 1);
    fprintf(out, "Identifier: %s\n", program->identifier);
    printIndent(out, level + 1);
    fprintf(out, "Block:\n");
    printBlock(program->block, out, level + 2);
}

// Block Node Print Function
void printBlock(const Block* block, FILE* out, int level) {
    if (!block) return;
    printIndent(out, level);
    fprintf(out, "[Block Node]\n");

    if (block->varDeclarations) {
        printIndent(out, level + 1);
        fprintf(out, "Var Decls:\n");
        printVarDeclaration(block->varDeclarations, out, level + 2);
    } else {
        printIndent(out, level+1);
        fprintf(out, "Var Decls: (empty)\n");
    }

    if (block->subRotDeclarations) {
        printIndent(out, level+1);
        fprintf(out, "SubRots:\n");
        printSubRotDeclaration(block->subRotDeclarations, out, level+2);
    }

    if (block->commandList) {
        printIndent(out, level+1);
        fprintf(out, "Commands:\n");
        printCommand(block->commandList, out, level+2);
    }
}

// Variable Declaration Node Print Function
void printVarDeclaration(const VarDeclaration* varDeclaration, FILE* out, int level) {
    for (; varDeclaration; varDeclaration = varDeclaration->next) {
        printIndent(out, level);
        fprintf(out, "[VarDecl] Type: %s, ID: %s\n", 
            (varDeclaration->type == Int ? "Integer" : "Boolean"),
            varDeclaration->identifier);
    }
}

// Identifier List Node Print Function
void printIdentifierList(const IdentifierList* identifierList, FILE* out) {
    fprintf(out, "[");
    int first = 1;
    for (; identifierList; identifierList = identifierList->next) {
        if (!first) fprintf(out, ", ");
        fprintf(out, "%s", identifierList->identifier);
        first = 0;
    }
    fprintf(out, "]");
}

// Subroutine Declaration Node Print Function
void printSubRotDeclaration(const SubRotDeclaration* subRotDeclaration, FILE* out, int level) {
    for (; subRotDeclaration; subRotDeclaration = subRotDeclaration->next) {
        printIndent(out, level);
        if (subRotDeclaration->type == Proc) {
            fprintf(out, "[Procedure Decl] ID: %s\n", subRotDeclaration->subrotU.procInfo.identifier);
            
            printIndent(out, level + 1);
            fprintf(out, "Parameters:\n");
            printVarDeclaration(subRotDeclaration->subrotU.procInfo.formParams, out, level + 2);

            printIndent(out, level + 1);
            fprintf(out, "Body:\n");
            if (subRotDeclaration->subrotU.procInfo.subRotBlock) {
                 printSubRotBlock(subRotDeclaration->subrotU.procInfo.subRotBlock, out, level + 2);
            }
        } else {
            fprintf(out, "[Function Decl] ID: %s, Return: %s\n", 
                subRotDeclaration->subrotU.funcInfo.identifier,
                (subRotDeclaration->subrotU.funcInfo.returnType == Int ? "Int" : "Bool"));
            
            printIndent(out, level + 1);
            fprintf(out, "Parameters:\n");
            printVarDeclaration(subRotDeclaration->subrotU.funcInfo.formParams, out, level+2);

            printIndent(out, level+1);
            fprintf(out, "Body:\n");
            if (subRotDeclaration->subrotU.funcInfo.subRotBlock) {
                 printSubRotBlock(subRotDeclaration->subrotU.funcInfo.subRotBlock, out, level+2);
            }
        }
    }
}

// Subroutine Block Node Print Function
void printSubRotBlock(const SubRotBlock* subRotBlock, FILE* out, int level) {
    if (!subRotBlock) return;
    printIndent(out, level);
    fprintf(out, "[SubRoutine Block]\n");
    
    if (subRotBlock->varDeclarations) {
        printIndent(out, level + 1);
        fprintf(out, "Local Vars:\n");
        printVarDeclaration(subRotBlock->varDeclarations, out, level + 2);
    }
    
    printIndent(out, level + 1);
    fprintf(out, "Cmd List:\n");
    printCommand(subRotBlock->commands, out, level + 2);
}

// Command Node Print Function
void printCommand(const Command* command, FILE* out, int level) {
    for (; command; command = command->next) {
        printIndent(out, level);
        switch (command->type) {
            case Assign:
                fprintf(out, "[Assign Cmd]\n");
                printIndent(out, level + 1);
                fprintf(out, "Target ID: %s\n", command->cmdU.assignInfo.identifier);
                printIndent(out, level + 1);
                fprintf(out, "Expr:\n");
                printExpression(command->cmdU.assignInfo.expression, out, level + 2);
                break;

            case ProcCall:
                fprintf(out, "[ProcCall Cmd]\n");
                printIndent(out, level + 1);
                fprintf(out, "Proc ID: %s\n", command->cmdU.procCallInfo.identifier);
                printIndent(out, level + 1);
                fprintf(out, "Args:\n");
                printExpression(command->cmdU.procCallInfo.expressionList, out, level + 2);
                break;

            case Conditional:
                fprintf(out, "[If Cmd]\n");
                printIndent(out, level + 1);
                fprintf(out, "Condition:\n");
                printExpression(command->cmdU.condInfo.condExpression, out, level + 2);
                
                printIndent(out, level + 1);
                fprintf(out, "Then Branch:\n");
                printCommand(command->cmdU.condInfo.cmdIf, out, level + 2);
                
                if (command->cmdU.condInfo.cmdElse) {
                    printIndent(out, level + 1);
                    fprintf(out, "Else Branch:\n");
                    printCommand(command->cmdU.condInfo.cmdElse, out, level + 2);
                }
                break;

            case Loop:
                fprintf(out, "[While Cmd]\n");
                printIndent(out, level + 1);
                fprintf(out, "Condition:\n");
                printExpression(command->cmdU.loopInfo.loopExpression, out, level + 2);
                printIndent(out, level + 1);
                fprintf(out, "Body:\n");
                printCommand(command->cmdU.loopInfo.cmdLoop, out, level + 2);
                break;

            case Read:
                fprintf(out, "[Read Cmd] Targets: ");
                printIdentifierList(command->cmdU.readInfo.identifiers, out);
                fprintf(out, "\n");
                break;

            case Write:
                fprintf(out, "[Write Cmd] Exprs:\n");
                printExpression(command->cmdU.writeInfo.expressionList, out, level + 1);
                break;
        }
    }
}

// Expression Node Print Function
void printExpression(const Expression* expression, FILE* out, int level) {
    for (; expression; expression = expression->next) {
        printIndent(out, level);
        switch (expression->type) {
            case Binary:
                fprintf(out, "[Binary Expr]\n");
                printIndent(out, level + 1);
                fprintf(out, "Op: ");
                switch (expression->exprU.binExpr.operator) {
                    case Equal: fprintf(out, "=\n"); break;
                    case Different: fprintf(out, "<>\n"); break;
                    case Less: fprintf(out, "<\n"); break;
                    case LessEqual: fprintf(out, "<=\n"); break;
                    case Greater: fprintf(out, ">\n"); break;
                    case GreaterEqual: fprintf(out, ">=\n"); break;
                    case Plus: fprintf(out, "+\n"); break;
                    case Minus: fprintf(out, "-\n"); break;
                    case Or: fprintf(out, "OR\n"); break;
                    case Multiplication: fprintf(out, "*\n"); break;
                    case Division: fprintf(out, "DIV\n"); break;
                    case And: fprintf(out, "AND\n"); break;
                    case Not: fprintf(out, "INVALID (NOT)\n"); break;
                }
                printIndent(out, level + 1);
                fprintf(out, "Left:\n");
                printExpression(expression->exprU.binExpr.left, out, level + 2);
                printIndent(out, level + 1);
                fprintf(out, "Right:\n");
                printExpression(expression->exprU.binExpr.right, out, level + 2);
                break;

            case Unary:
                fprintf(out, "[Unary Expr]\n");
                printIndent(out, level + 1);
                fprintf(out, "Op: ");
                switch (expression->exprU.unyExpr.operator) {
                    case Minus: fprintf(out, "-\n"); break;
                    case Plus: fprintf(out, "+\n"); break;
                    case Not: fprintf(out, "NOT\n"); break;
                    default: fprintf(out, "?\n"); break;
                }
                printIndent(out, level + 1);
                fprintf(out, "Operand:\n");
                printExpression(expression->exprU.unyExpr.right, out, level + 2);
                break;

            case Var:
                fprintf(out, "[Var Expr] ID: %s\n", expression->exprU.varExpr.identifier);
                break;

            case ConstInt:
                fprintf(out, "[ConstInt Expr] Value: %d\n", expression->exprU.intExpr.number);
                break;

            case ConstBool:
                fprintf(out, "[ConstBool Expr] Value: %s\n", 
                    expression->exprU.boolExpr.boolean == BoolTrue ? "true" : "false");
                break;

            case FuncCall:
                fprintf(out, "[FuncCall Expr] ID: %s\n", expression->exprU.funCallExpr.identifier);
                printIndent(out, level + 1);
                fprintf(out, "Args:\n");
                printExpression(expression->exprU.funCallExpr.expressionList, out, level + 2);
                break;
        }
    }
}