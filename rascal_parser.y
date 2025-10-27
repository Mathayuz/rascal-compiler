%{
#include <stdio.h>

int yylex(void);
void yyerror(const char *s);
extern int yylineno;
%}

%define parse.error verbose

%union {
    int dval;
    char* sval;
}

%token PROGRAM BEGIN END PROCEDURE FUNCTION VAR INTEGER BOOLEAN FALSE TRUE WHILE DO IF THEN ELSE READ WRITE AND OR NOT DIV DIF LTE GTE ASSIGN
%token <sval> ID
%token <dval> NUM

%left NOT
%left AND
%left OR
%nonassoc DIF LTE GTE
%left '+' '-'
%left '*' DIV
%nonassoc IF
%nonassoc ELSE

%%

program
    : PROGRAM ID ';' block '.'
    ;

block
    : var_decl_sec_optional subr_decl_sec_optional compound_cmd 
    ;

var_decl_sec_optional
    : var_decl_sec
    | /* empty */
    ;

var_decl_sec
    : VAR var_decl_list
    ;

var_decl_list
    : var_decl ';'
    | var_decl_list var_decl ';'
    ;

var_decl
    : id_list ':' type
    ;

id_list
    : ID
    | id_list ',' ID
    ;

type
    : INTEGER
    | BOOLEAN
    ;

subr_decl_sec_optional
    : subr_decl_sec
    | /* empty */
    ;

subr_decl_sec
    : subr_decl
    | subr_decl_sec subr_decl
    ;


subr_decl
    : proc_decl ';'
    | func_decl ';'
    ;

proc_decl
    : PROCEDURE ID form_param_optional ';' subr_block
    ;

func_decl
    : FUNCTION ID form_param_optional ':' type ';' subr_block
    ;

form_param_optional
    : form_param
    | /* empty */
    ;

subr_block
    : var_decl_sec_optional compound_cmd
    ;

form_param
    : '(' form_param_list ')'
    ;

form_param_list
    : param_decl
    | form_param_list ';' param_decl
    ;

param_decl
    : id_list ':' type
    ;

compound_cmd
    : BEGIN cmd_list END
    ;

cmd_list
    : cmd
    | cmd_list ';' cmd
    ;

cmd
    : assign_cmd
    | proc_call_cmd
    | cond_cmd
    | loop_cmd
    | read_cmd
    | write_cmd
    | compound_cmd
    ;

assign_cmd
    : ID ASSIGN expr
    ;

proc_call_cmd
    : ID '(' expr_list_optional ')'
    ;

expr_list_optional
    : expr_list
    | /* empty */
    ;

cond_cmd
    : IF expr THEN cmd else_part_optional
    ;

else_part_optional
    : ELSE cmd
    | /* empty */
    ;

loop_cmd
    : WHILE expr DO cmd
    ;

read_cmd
    : READ '(' id_list ')'
    ;

write_cmd
    : WRITE '(' expr_list ')'
    ;

expr_list
    : expr
    | expr_list ',' expr
    ;

expr
    : simple_expr rel_expr_optional
    ;

rel_expr_optional
    : relational simple_expr
    | /* empty */
    ;

relational
    : '='
    | DIF
    | '<'
    | LTE
    | '>'
    | GTE
    ;

simple_expr
    : term op_term_list
    ;

op_term_list
    : op_term
    | op_term_list op_term
    ;

op_term
    : '+' term
    | '-' term
    | OR term
    ;

term
    : factor op_factor_list
    ;

op_factor_list
    : op_factor
    | op_factor_list op_factor
    ;

op_factor
    : '*' factor
    | DIV factor
    | AND factor
    ;

factor
    : variable
    | NUM
    | logical
    | func_call
    | '(' expr ')'
    | NOT factor
    | '-' factor
    ;

variable
    : ID
    ;

logical
    : TRUE
    | FALSE
    ;

func_call
    : ID '(' expr_list_optional ')'
    ;

%%

void yyerror(const char *s){
    printf("ERRO SINTÁTICO na linha %d: %s\n", yylineno, s);
}

int main(void) {
    yyparse();
    printf("\n");
}