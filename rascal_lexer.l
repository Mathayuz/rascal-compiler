%{
#include <stdio.h>
#include <string.h>

// Enumerar inicialmente para Debug, fica mais limpo retirar posteriormente
enum {
    TOK_PROGRAM=256, TOK_BEGIN=257, TOK_END=258, TOK_PROC=259, TOK_FUNC=260,
    TOK_VAR=261, TOK_INT=262, TOK_BOOL=263, TOK_FALSE=264, TOK_TRUE=265,
    TOK_WHILE=266, TOK_DO=267, TOK_IF=268, TOK_THEN=269, TOK_ELSE=270,
    TOK_READ=271, TOK_WRITE=272, TOK_AND=273, TOK_OR=274, TOK_NOT=275,
    TOK_DIV=276, TOK_DIF=277, TOK_MENIG=278, TOK_MAIIG=279, TOK_ATRI=280,
    TOK_NUM=281, TOK_ID=282, TOK_ERR=283};

char lexema[100];
%}

%option noyywrap
%option yylineno

espaco     [ \t\n]
digito      [0-9]
letra       [a-zA-Z]
id          {letra}({letra}|{digito}|_)*

%%

{espaco}+          /* Ignorar espaços, tabulações e quebras de linha */

{digito}+           {strcpy(lexema, yytext); return TOK_NUM;}

"program"           {strcpy(lexema, yytext); return TOK_PROGRAM;}
"begin"             {strcpy(lexema, yytext); return TOK_BEGIN;}
"end"               {strcpy(lexema, yytext); return TOK_END;}
"procedure"         {strcpy(lexema, yytext); return TOK_PROC;}
"function"          {strcpy(lexema, yytext); return TOK_FUNC;}
"var"               {strcpy(lexema, yytext); return TOK_VAR;}
"integer"           {strcpy(lexema, yytext); return TOK_INT;}
"boolean"           {strcpy(lexema, yytext); return TOK_BOOL;}
"false"             {strcpy(lexema, yytext); return TOK_FALSE;}
"true"              {strcpy(lexema, yytext); return TOK_TRUE;}
"while"             {strcpy(lexema, yytext); return TOK_WHILE;}
"do"                {strcpy(lexema, yytext); return TOK_DO;}
"if"                {strcpy(lexema, yytext); return TOK_IF;}
"then"              {strcpy(lexema, yytext); return TOK_THEN;}
"else"              {strcpy(lexema, yytext); return TOK_ELSE;}
"read"              {strcpy(lexema, yytext); return TOK_READ;}
"write"             {strcpy(lexema, yytext); return TOK_WRITE;}
"and"               {strcpy(lexema, yytext); return TOK_AND;}
"or"                {strcpy(lexema, yytext); return TOK_OR;}
"not"               {strcpy(lexema, yytext); return TOK_NOT;}
"div"               {strcpy(lexema, yytext); return TOK_DIV;}

"<>"                {strcpy(lexema, yytext); return TOK_DIF;}
"<="                {strcpy(lexema, yytext); return TOK_MENIG;}
">="                {strcpy(lexema, yytext); return TOK_MAIIG;}
":="                {strcpy(lexema, yytext); return TOK_ATRI;}

"("                 {strcpy(lexema, yytext); return '(';}
")"                 {strcpy(lexema, yytext); return ')';}
";"                 {strcpy(lexema, yytext); return ';';}
"="                 {strcpy(lexema, yytext); return '=';}
"<"                 {strcpy(lexema, yytext); return '<';}
">"                 {strcpy(lexema, yytext); return '>';}
"+"                 {strcpy(lexema, yytext); return '+';}
"-"                 {strcpy(lexema, yytext); return '-';}
"*"                 {strcpy(lexema, yytext); return '*';}
":"                 {strcpy(lexema, yytext); return ':';}
","                 {strcpy(lexema, yytext); return ',';}
"."                 {strcpy(lexema, yytext); return '.';}

{id}                {strcpy(lexema, yytext); return TOK_ID;}

.                   {strcpy(lexema, yytext); return TOK_ERR;}

%%

// Função temporária para testar o Lexer
int main (int argc, char *argv[]) {
    int tok;
    yyin = fopen(argv[1], "r");
    while (tok=yylex()) {
        printf("<%s, %d>\t(linha: %d)\n", lexema, tok, yylineno);
    }
    fclose(yyin);
}