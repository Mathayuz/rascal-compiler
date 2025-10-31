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
        struct {struct SimpleExpression* simple;} simpExp;
        struct {struct SimpleExpression* left; relationalOperation relOperator; struct SimpleExpression* right;} relExp;
    } exprU;
    struct Expression* next; // To link in the list
} Expression;

// Simple Expression Node
typedef enum {SingleSimpleExpression, CompoundSimpleExpression} simpExprType;
typedef enum {Plus, Minus, Or} termOperation;

typedef struct SimpleExpression {
    simpExprType type;
    union {
        struct {struct Term* term;} singleSimpExpr;
        struct {struct Term* left; termOperation termOperator; struct SimpleExpression* right;} compoundSimpExpr;
    } simpExprU;
} SimpleExpression;

// Term Node
typedef enum {SingleTerm, CompoundTerm} termType;
typedef enum {Multiplication, Division, And} factorOperation;

typedef struct Term {
    termType type;
    union {
        struct {struct Factor* factor;} simpleTerm;
        struct {struct Factor* left; factorOperation factOperator; struct Term* right;} compoundTerm;
    } termU;
} Term;

// Factor Node
typedef enum {Variable, Number, Boolean, FunctionCall, Expression, Unary} factorType;
typedef enum {Negative, Not} unaryOperation;

typedef struct Factor {
    factorType type;
    union {
        struct {char* identifier;} var;
        struct {int value;} num;
        struct {int logicalValue;} boolean;
        struct {char* funcName; struct Expression* arguments /*List*/;} funcCall;
        struct {struct Expression* expression;} expr;
        struct {unaryOperation unOperator; struct Factor* operand;} unarFac;
    } facU;
} Factor;

#endif