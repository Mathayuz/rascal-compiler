#ifndef RASCAL_MEPA_H
#define RASCAL_MEPA_H

#include "rascal_ast.h"
#include "symbol_table.h"
#include <stdio.h>

// Armazena o contexto da geração de código
typedef struct CodeGenContext {
    FILE *mepaFile;
    int labelCount;
    int currentLevel;
} CodeGenContext;

// MEPA Code Generation Functions
void generateCode(Program *root, const char *filename);
void generateProgram(Program* p, CodeGenContext* ctx);
void generateBlock(Block* b, CodeGenContext* ctx);
void generateVariableDeclaration(VarDeclaration* vd, CodeGenContext* ctx);
void generateSubRotDeclaration(SubRotDeclaration* sd, CodeGenContext* ctx);
void generateSubRotBlock(SubRotBlock* sb, CodeGenContext* ctx);
void generateCommandList(Command* c, CodeGenContext* ctx);
void generateCommand(Command* c, CodeGenContext* ctx);
void generateAssignCmd(Command* c, CodeGenContext* ctx);
void generateProcedureCallCmd(Command* c, CodeGenContext* ctx);
void generateReverseExpressions(Expression* expr, CodeGenContext* ctx);
void generateConditionalCmd(Command* c, CodeGenContext* ctx);
void generateLoopCmd(Command* c, CodeGenContext* ctx);
void generateReadCmd(Command* c, CodeGenContext* ctx);
void generateWriteCmd(Command* c, CodeGenContext* ctx);
void generateExpression(Expression* e, CodeGenContext* ctx);
void generateBinaryExpr(Expression* e, CodeGenContext* ctx);
void generateUnaryexpr(Expression* e, CodeGenContext* ctx);
void generateIntExpr(Expression* e, CodeGenContext* ctx);
void generateBooleanExpr(Expression* e, CodeGenContext* ctx);
void generateVarExpr(Expression* e, CodeGenContext* ctx);
void generateFunctionCallExpr(Expression* e, CodeGenContext* ctx);

// Auxiliary Write Functions
int varListSize(VarDeclaration* list);
int newLabel(CodeGenContext* ctx);
void writeLabel(CodeGenContext* ctx, int label);
void writeInstr(CodeGenContext* ctx, const char* op);
void writeInstrIntArg(CodeGenContext* ctx, const char* op, int arg);
void writeInstr2IntArg(CodeGenContext* ctx, const char* op, int arg1, int arg2);
void writeInstrLabelArg(CodeGenContext* ctx, const char* op, int label);

#endif