%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rascal_ast.h"

int yylex(void);
void yyerror(const char *s);
extern int yylineno;

Program* ast_root = NULL;
%}

%code requires {
    #include "rascal_ast.h"
}

%define parse.error verbose

%union {
    Program* prog;
    Block* block;
    VarDeclaration* varDecl;
    IdentifierList* idList;
    SubRotDeclaration* subRotDecl;
    SubRotBlock* subRotBlock;
    Command* cmd;
    Expression* expr;

    varType vType;
    Operator op;
    BooleanValue boolVal;

    int ival;
    char* sval;
}

%token PROGRAM TBEGIN END PROCEDURE FUNCTION VAR INTEGER BOOLEAN TFALSE TTRUE WHILE DO IF THEN ELSE READ WRITE AND OR NOT DIV DIF LTE GTE ASSIGN
%token <sval> ID
%token <ival> NUM

// Program
%type <prog> program

// Block
%type <block> block

// Variable Declarations Section
%type <varDecl> var_decl_sec_optional var_decl_sec var_decl var_decl_list
%type <idList> id_list
%type <vType> type;

// Subroutine Declarations Section
%type <subRotDecl> subr_decl_sec_optional subr_decl_sec subr_decl proc_decl func_decl
%type <varDecl> form_param_optional form_param_list form_param
%type <subRotBlock> subr_block;

// Commands Section
%type <cmd> compound_cmd cmd_list cmd assign_cmd proc_call_cmd cond_cmd else_part_optional loop_cmd read_cmd write_cmd

// Expression Section
%type <expr> expr_list_optional expr_list expr simple_expr term factor func_call
%type <op> relational;
%type <sval> variable;
%type <boolVal> logical;

%nonassoc '=' DIF '<' LTE '>' GTE
%left '+' '-' OR
%left '*' DIV AND
%left NOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

program
    : PROGRAM ID ';' block '.'
    { 
        $$ = newProgram($2, $4);
        ast_root = $$;
    }
    ;

block
    : var_decl_sec_optional subr_decl_sec_optional compound_cmd 
    {
        $$ = newBlock($1, $2, $3);
    }
    ;

var_decl_sec_optional
    : var_decl_sec                  {$$ = $1;}
    | /* empty */                   {$$ = NULL;}
    ;

var_decl_sec
    : VAR var_decl_list             {$$ = $2;}
    ;

var_decl_list
    : var_decl ';'                  {$$ = $1;}
    | var_decl_list var_decl ';'    {$$ = addVarDeclaration($1, $2);}
    ;

var_decl
    : id_list ':' type  
    {
        IdentifierList* it = $1;
        VarDeclaration* list = NULL;
        while (it) {
            VarDeclaration* node = newVarDeclaration($3, it->identifier);
            it->identifier = NULL; // identifier belongs to the new node
            list = addVarDeclaration(list, node);
            it = it->next;
        }
        freeIdentifierList($1);
        $$ = list;
    }
    ;

id_list
    : ID                            {$$ = newIdentifierList($1);}
    | id_list ',' ID                {$$ = addIdentifier($1, newIdentifierList($3));}
    ;

type
    : INTEGER                       {$$ = Int;}
    | BOOLEAN                       {$$ = Bool;}
    ;

subr_decl_sec_optional
    : subr_decl_sec                 {$$ = $1;}
    | /* empty */                   {$$ = NULL;}
    ;

subr_decl_sec
    : subr_decl ';'                 {$$ = $1;}
    | subr_decl_sec subr_decl ';'   {$$ = addSubRotDeclaration($1, $2);}
    ;

subr_decl
    : proc_decl                     {$$ = $1;}
    | func_decl                     {$$ = $1;}
    ;

proc_decl
    : PROCEDURE ID form_param_optional ';' subr_block           {$$ = newProcDeclaration($2, $3, $5);}
    ;

func_decl
    : FUNCTION ID form_param_optional ':' type ';' subr_block   {$$ = newFuncDeclaration($2, $3, $5, $7);}
    ;

form_param_optional
    : form_param                    {$$ = $1;}
    | /* empty */                   {$$ = NULL;}
    ;

form_param
    : '(' form_param_list ')'       {$$ = $2;}
    ;

form_param_list
    : var_decl                      {$$ = $1;}
    | form_param_list ';' var_decl  {$$ = addVarDeclaration($1, $3);}
    ;

subr_block
    : var_decl_sec_optional compound_cmd                        {$$ = newSubRotBlock($1, $2);}
    ;

compound_cmd
    : TBEGIN cmd_list END           {$$ = $2;}
    ;

cmd_list
    : cmd                           {$$ = $1;}
    | cmd_list ';' cmd              {$$ = addCommand($1, $3);}
    ;

cmd
    : assign_cmd                    {$$ = $1;}
    | proc_call_cmd                 {$$ = $1;}
    | cond_cmd                      {$$ = $1;}
    | loop_cmd                      {$$ = $1;}
    | read_cmd                      {$$ = $1;}
    | write_cmd                     {$$ = $1;}
    | compound_cmd                  {$$ = $1;}
    ;

assign_cmd
    : ID ASSIGN expr                {$$ = newAssignCommand($1, $3);}
    ;

proc_call_cmd
    : ID '(' expr_list_optional ')' {$$ = newProcCallCommand($1, $3);}
    ;

cond_cmd
    : IF expr THEN cmd else_part_optional                       {$$ = newCondCommand($2, $4, $5);}
    ;

else_part_optional
    : ELSE cmd                                                  {$$ = $2;}
    | /* empty */ %prec LOWER_THAN_ELSE                         {$$ = NULL;}
    ;

loop_cmd
    : WHILE expr DO cmd             {$$ = newLoopCommand($2, $4);}
    ;

read_cmd
    : READ '(' id_list ')'          {$$ = newReadCommand($3);}
    ;

write_cmd
    : WRITE '(' expr_list ')'       {$$ = newWriteCommand($3);}
    ;

expr_list_optional
    : expr_list                     {$$ = $1;}
    | /* empty */                   {$$ = NULL;}
    ;

expr_list
    : expr                          {$$ = $1;}
    | expr_list ',' expr            {$$ = addExpression($1, $3);}
    ;

expr
    : simple_expr                                               {$$ = $1;}
    | simple_expr relational simple_expr                        {$$ = newBinaryExpression($1, $2, $3);}
    ;

relational
    : '='                           {$$ = Equal;}
    | DIF                           {$$ = Different;}
    | '<'                           {$$ = Less;}
    | LTE                           {$$ = LessEqual;}
    | '>'                           {$$ = Greater;}
    | GTE                           {$$ = GreaterEqual;}
    ;

simple_expr
    : term                          {$$ = $1;}
    | '+' term                      {$$ = $2;}
    | '-' term                      {$$ = newUnaryExpression(Minus, $2);}
    | simple_expr '+' term          {$$ = newBinaryExpression($1, Plus, $3);}
    | simple_expr '-' term          {$$ = newBinaryExpression($1, Minus, $3);}
    | simple_expr OR term           {$$ = newBinaryExpression($1, Or, $3);}
    ;

term
    : factor                        {$$ = $1;}
    | term '*' factor               {$$ = newBinaryExpression($1, Multiplication, $3);}
    | term DIV factor               {$$ = newBinaryExpression($1, Division, $3);}
    | term AND factor               {$$ = newBinaryExpression($1, And, $3);}
    ;

factor
    : variable                      {$$ = newVariableExpression($1);}
    | NUM                           {$$ = newConstantIntegerExpression($1);}
    | logical                       {$$ = newConstantBooleanExpression($1);}
    | func_call                     {$$ = $1;}
    | '(' expr ')'                  {$$ = $2;}
    | NOT factor                    {$$ = newUnaryExpression(Not, $2);}
    ;

variable
    : ID                            {$$ = $1;}
    ;

logical
    : TTRUE                         {$$ = BoolTrue;}
    | TFALSE                        {$$ = BoolFalse;}
    ;

func_call
    : ID '(' expr_list_optional ')' {$$ = newFunctionCallExpression($1, $3);}
    ;

%%

void yyerror(const char *s){
    printf("Erro Sintático na linha %d: %s\n", yylineno, s);
}