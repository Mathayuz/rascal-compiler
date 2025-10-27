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

%token PROGRAM BEGIN END PROCEDURE FUNCTION VAR INTEGER BOOLEAN FALSE TRUE WHILE DO IF THEN ELSE READ WRITE AND OR NOT DIV DIF LTE GTE ASSIGN ERROR
%token <sval> ID
%token <dval> NUM

%left NOT
%left AND
%left OR
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
    : list_id ':' type
    ;

list_id
    : ID
    | list_id ',' ID
    ;

type
    : INTEGER
    | BOOLEAN
    ;

%%