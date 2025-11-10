#ifndef RASCAL_AST_H
#define RASCAL_AST_H

#include <stdio.h>

// Program Node
typedef struct Program {
    char* identifier;
    struct Block* block;
} Program;

// Block Node
typedef struct Block {
    struct VarDeclaration* varDeclarations; // Linked List
    struct SubRotDeclaration* subRotDeclarations; // Linked List
    struct Command* commandList; // Linked List
} Block;

// Variable Declarations Node
typedef enum {Int, Bool} varType;

typedef struct VarDeclaration {
    varType type;
    char* identifier;
    struct VarDeclaration* next; // To link in the list
} VarDeclaration;

// Identifier List Node
typedef struct IdentifierList {
    char* identifier;
    struct IdentifierList* next; // To link in the list
} IdentifierList;

// Subroutine Declarations Node
typedef enum {Proc, Func} subRotType;

typedef struct SubRotDeclaration {
    subRotType type;
    union {
        struct {char* identifier; struct VarDeclaration* formParams /*List*/;} procInfo;                        // Procedure
        struct {char* identifier; struct VarDeclaration* formParams /*List*/; varType returnType;} funcInfo;    // Function
    } subrotU;
    struct SubRotDeclaration* next; // To link in the list
} SubRotDeclaration;

// Command List Node
typedef enum {Assign, ProcCall, Conditional, Loop, Read, Write} cmdType;

typedef struct Command {
    cmdType type;
    union {
        struct {char* identifier; struct Expression* expression;} assignInfo;                                   // Assign Command
        struct {char* identifier; struct Expression* expressions /*List*/;} procCallInfo;                       // Procedure Call Command
        struct {struct Expression* condExpression; struct Command* cmdIf; struct Command* cmdElse;} condInfo;   // Conditional Command
        struct {struct Expression* loopExpression; struct Command* cmdLoop;} loopInfo;                          // Looping Command
        struct {struct IdentifierList* identifiers /*List*/;} readInfo;                                         // Read Command
        struct {struct Expression* expressions /*List*/;} writeInfo;                                            // Write Command
    } cmdU;
    struct Command* next; // To link in the list
} Command;


// Expression Node
typedef enum {SingleExpression, RelationalExpression} exprType;
typedef enum {Equal, Different, Less, LessEqual, Greater, GreaterEqual} relationalOperation;

typedef struct Expression {
    exprType type;
    union {
        struct {struct SimpleExpression* simple;} simpExp;                                                                // Simple Expression
        struct {struct SimpleExpression* left; relationalOperation relOperator; struct SimpleExpression* right;} relExp;  // Relational Expression
    } exprU;
    struct Expression* next; // To link in the list
} Expression;

// Simple Expression Node
typedef enum {SingleSimpleExpression, CompoundSimpleExpression} simpExprType;
typedef enum {Plus, Minus, Or} termOperation;

typedef struct SimpleExpression {
    simpExprType type;
    union {
        struct {struct Term* term;} singleSimpExpr;                                                               // Single Simple Expression
        struct {struct Term* left; termOperation termOperator; struct SimpleExpression* right;} compoundSimpExpr; // Compound Simple Expression
    } simpExprU;
} SimpleExpression;

// Term Node
typedef enum {SingleTerm, CompoundTerm} termType;
typedef enum {Multiplication, Division, And} factorOperation;

typedef struct Term {
    termType type;
    union {
        struct {struct Factor* factor;} simpleTerm;                                                     // Simple Term
        struct {struct Factor* left; factorOperation factOperator; struct Term* right;} compoundTerm;   // Coumpound Term
    } termU;
} Term;

// Factor Node
typedef enum {Variable, Number, Boolean, FunctionCall, ExpressionFT, Unary} factorType;
typedef enum {Negative, Not} unaryOperation;

typedef struct Factor {
    factorType type;
    union {
        struct {char* identifier;} var;                                             // Variable Factor
        struct {int value;} num;                                                    // Number Factor
        struct {int logicalValue;} boolean;                                         // Boolean Factor
        struct {char* funcName; struct Expression* arguments /*List*/;} funcCall;   // Function Call Factor
        struct {struct Expression* expression;} expr;                               // Expression Factor
        struct {unaryOperation unOperator; struct Factor* operand;} unarFac;        // Unary Operation Factor
    } facU;
} Factor;

// Node Constructors
Program* newProgram(char* identifier, Block* block);

Block* newBlock(VarDeclaration* varDeclarations, SubRotDeclaration* subRotDeclarations, Command* commandList);

VarDeclaration* newVarDeclaration(varType type, char* identifier);

IdentifierList* newIdentifierList(char* identifier);

SubRotDeclaration* newProcDeclaration(char* identifier, VarDeclaration* formParams);
SubRotDeclaration* newFuncDeclaration(char* identifier, VarDeclaration* formParams, varType returnType);

Command* newAssignCommand(char* identifier, Expression* expression);
Command* newProcCallCommand(char* identifier, Expression* expressions);
Command* newCondCommand(Expression* condExpression, Command* cmdIf, Command* cmdElse);
Command* newLoopCommand(Expression* loopExpression, Command* cmdLoop);
Command* newReadCommand(IdentifierList* identifiers);
Command* newWriteCommand(Expression* expressions);

Expression* newSimpleExpression(SimpleExpression* simpleExpression);
Expression* newRelationalExpression(SimpleExpression* left, relationalOperation relOperator, SimpleExpression* right);

SimpleExpression* newSingleSimpleExpression(Term* term);
SimpleExpression* newCompoundSimpleExpression(Term* left, termOperation termOperator, SimpleExpression* right);

Term* newSimpleTerm(Factor* factor);
Term* newCompoundTerm(Factor* left, factorOperation factOperator, Term* right);

Factor* newVariableFactor(char* identifier);
Factor* newNumberFactor(int value);
Factor* newBooleanFactor(int logicalValue);
Factor* newFuncCallFactor(char* funcName, Expression* arguments);
Factor* newExpressionFactor(Expression* expression);
Factor* newUnaryOperatorFactor(unaryOperation unOperator, Factor* operand);

// Add to Linked List Functions
VarDeclaration* addVarDeclaration(VarDeclaration* list, VarDeclaration* newVarDecl);

IdentifierList* addIdentifier(IdentifierList* list, IdentifierList* newId);

SubRotDeclaration* addSubRotDeclaration(SubRotDeclaration* list, SubRotDeclaration* newSubRotDecl);

Command* addCommand(Command* list, Command* newCmd);

Expression* addExpression(Expression* list, Expression* newExpr);

// Print Functions
void printProgram(const Program* program, FILE* out);
void printBlock(const Block* block, FILE* out);
void printVarDeclaration(const VarDeclaration* varDecaration, FILE* out);
void printIdentifierList(const IdentifierList* identifierList, FILE* out);
void printSubRotDeclaration(const SubRotDeclaration* subRotDeclaration, FILE* out);
void printCommand(const Command* command, FILE* out);
void printExpression(const Expression* expression, FILE* out);
void printSimpleExpression(const SimpleExpression* simpleExpression, FILE* out);
void printTerm(const Term* term, FILE* out);
void printFactor(const Factor* factor, FILE* out);

// Free Functions
void freeProgram(const Program* p);
void freeBlock(const Block* b);
void freeVarDeclaration(const VarDeclaration* vd);
void freeIdentifierList(const IdentifierList* il);
void freeSubRotDeclaration(const SubRotDeclaration* srd);
void freeCommand(const Command* c);
void freeExpression(const Expression* e);
void freeSimpleExpression(const SimpleExpression* se);
void freeTerm(const Term* t);
void freeFactor(const Factor* f);

#endif