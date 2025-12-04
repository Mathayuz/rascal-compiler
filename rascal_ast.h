#ifndef RASCAL_AST_H
#define RASCAL_AST_H

#include <stdio.h>

// Foward Declarations
typedef struct Program Program;
typedef struct Block Block;
typedef struct VarDeclaration VarDeclaration;
typedef struct IdentifierList IdentifierList;
typedef struct SubRotDeclaration SubRotDeclaration;
typedef struct SubRotBlock SubRotBlock;
typedef struct Command Command;
typedef struct Expression Expression;

// Enumerations
typedef enum {Int, Bool} varType;
typedef enum {Proc, Func} subRotType;
typedef enum {Assign, ProcCall, Conditional, Loop, Read, Write} cmdType;
typedef enum {Binary, Unary, Var, ConstInt, ConstBool, FuncCall} exprType;
typedef enum {Equal, Different, Less, LessEqual, Greater, GreaterEqual, Plus, Minus, Or, Multiplication, Division, And, Not} Operator;
typedef enum {BoolFalse, BoolTrue} BooleanValue;

// Program Node
struct Program {
    char* identifier;
    struct Block* block;
};

// Block Node
struct Block {
    struct VarDeclaration* varDeclarations;         // Linked List
    struct SubRotDeclaration* subRotDeclarations;   // Linked List
    struct Command* commandList;                    // Linked List
};

struct VarDeclaration {
    varType type;
    char* identifier;
    struct VarDeclaration* next;                    // To link in the list
};

// Identifier List Node
struct IdentifierList {
    char* identifier;
    struct IdentifierList* next;                    // To link in the list
};

// Subroutine Declarations Node
struct SubRotDeclaration {
    subRotType type;
    union {
        struct {char* identifier; struct VarDeclaration* formParams /*List*/; struct SubRotBlock* subRotBlock;} procInfo;                        // Procedure
        struct {char* identifier; struct VarDeclaration* formParams /*List*/; varType returnType; struct SubRotBlock* subRotBlock;} funcInfo;    // Function
    } subrotU;
    struct SubRotDeclaration* next;                                                                                                              // To link in the list
};

// Subroutine Block Node
struct SubRotBlock {
    struct VarDeclaration* varDeclarations;
    struct Command* commands /*List*/;
};

// Command List Node
struct Command {
    cmdType type;
    union {
        struct {char* identifier; struct Expression* expression;} assignInfo;                                   // Assign Command
        struct {char* identifier; struct Expression* expressionList;} procCallInfo;                             // Procedure Call Command
        struct {struct Expression* condExpression; struct Command* cmdIf; struct Command* cmdElse;} condInfo;   // Conditional Command
        struct {struct Expression* loopExpression; struct Command* cmdLoop;} loopInfo;                          // Looping Command
        struct {struct IdentifierList* identifiers /*List*/;} readInfo;                                         // Read Command
        struct {struct Expression* expressionList;} writeInfo;                                                  // Write Command
    } cmdU;
    struct Command* next;                                                                                       // To link in the list
};

// Expression List Node
struct Expression {
    exprType type;
    union {
        struct {struct Expression* left; Operator operator; struct Expression* right;} binExpr;                 // Binary Expression
        struct {Operator operator; struct Expression* right;} unyExpr;                                          // Unary Expression
        struct {char* identifier;} varExpr;                                                                     // Variable Expression
        struct {int number;} intExpr;                                                                           // Constant Integer Expression
        struct {BooleanValue boolean;} boolExpr;                                                                // Constant Boolean Expression
        struct {char* identifier; struct Expression* expressionList;} funCallExpr;                              // Function Call Expression
    } exprU;
    struct Expression* next;                                                                                    // To link in the list
};

// Node Constructors
Program* newProgram(char* identifier, Block* block);
Block* newBlock(VarDeclaration* varDeclarations, SubRotDeclaration* subRotDeclarations, Command* commandList);
VarDeclaration* newVarDeclaration(varType type, char* identifier);
IdentifierList* newIdentifierList(char* identifier);
SubRotDeclaration* newProcDeclaration(char* identifier, VarDeclaration* formParams, SubRotBlock* subRotBlock);
SubRotDeclaration* newFuncDeclaration(char* identifier, VarDeclaration* formParams, varType returnType, SubRotBlock* subRotBlock);
SubRotBlock* newSubRotBlock(VarDeclaration* varDeclarations, Command* commands);
Command* newAssignCommand(char* identifier, Expression* expression);
Command* newProcCallCommand(char* identifier, Expression* expressionList);
Command* newCondCommand(Expression* condExpression, Command* cmdIf, Command* cmdElse);
Command* newLoopCommand(Expression* loopExpression, Command* cmdLoop);
Command* newReadCommand(IdentifierList* identifiers);
Command* newWriteCommand(Expression* expressionList);
Expression* newBinaryExpression(Expression* left, Operator operator, Expression* right);
Expression* newUnaryExpression(Operator operator, Expression* right);
Expression* newVariableExpression(char* identifier);
Expression* newConstantIntegerExpression(int number);
Expression* newConstantBooleanExpression(BooleanValue boolean);
Expression* newFunctionCallExpression(char* identifier, Expression* expressionList);

// Add to Linked List Functions
VarDeclaration* addVarDeclaration(VarDeclaration* list, VarDeclaration* newVarDecl);
IdentifierList* addIdentifier(IdentifierList* list, IdentifierList* newId);
SubRotDeclaration* addSubRotDeclaration(SubRotDeclaration* list, SubRotDeclaration* newSubRotDecl);
Command* addCommand(Command* list, Command* newCmd);
Expression* addExpression(Expression* list, Expression* newExpr);

// Free Functions
void freeAstRoot(Program* r);
void freeProgram(Program* p);
void freeBlock(Block* b);
void freeVarDeclaration(VarDeclaration* vd);
void freeIdentifierList(IdentifierList* il);
void freeSubRotDeclaration(SubRotDeclaration* srd);
void freeSubRotBlock(SubRotBlock* srb);
void freeCommand(Command* c);
void freeExpression(Expression* e);

// Print Functions
void printAstRoot(Program* ast_root, FILE* out);
void printProgram(const Program* program, FILE* out, int level);
void printBlock(const Block* block, FILE* out, int level);
void printVarDeclaration(const VarDeclaration* varDeclaration, FILE* out, int level);
void printIdentifierList(const IdentifierList* identifierList, FILE* out);
void printSubRotDeclaration(const SubRotDeclaration* subRotDeclaration, FILE* out, int level);
void printSubRotBlock(const SubRotBlock* subRotBlock, FILE* out, int level);
void printCommand(const Command* command, FILE* out, int level);
void printExpression(const Expression* expression, FILE* out, int level);

/* Global Variable for AST Root */
extern Program* ast_root;

#endif