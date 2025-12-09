%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rascal_parser.tab.h"
#include "rascal_ast.h"

void yyerror(const char *s);
%}

%option noyywrap yylineno nounput noinput

SPACES      [ \t\n\r]+
DIGIT       [0-9]
NUM         {DIGIT}+
LETTER      [a-zA-Z]
ID          {LETTER}({LETTER}|{DIGIT}|_)*

%%

"program"           {return PROGRAM;}
"begin"             {return TBEGIN;}
"end"               {return END;}
"procedure"         {return PROCEDURE;}
"function"          {return FUNCTION;}
"var"               {return VAR;}
"integer"           {return INTEGER;}
"boolean"           {return BOOLEAN;}
"false"             {return TFALSE;}
"true"              {return TTRUE;}
"while"             {return WHILE;}
"do"                {return DO;}
"if"                {return IF;}
"then"              {return THEN;}
"else"              {return ELSE;}
"read"              {return READ;}
"write"             {return WRITE;}
"and"               {return AND;}
"or"                {return OR;}
"not"               {return NOT;}
"div"               {return DIV;}

{NUM}               {yylval.ival = atoi(yytext); return NUM;}
{ID}                { yylval.sval = strdup(yytext); return ID; }

"<>"                {return DIF;}
"<="                {return LTE;}
">="                {return GTE;}
":="                {return ASSIGN;}

"("                 {return '(';}
")"                 {return ')';}
";"                 {return ';';}
"="                 {return '=';}
"<"                 {return '<';}
">"                 {return '>';}
"+"                 {return '+';}
"-"                 {return '-';}
"*"                 {return '*';}
":"                 {return ':';}
","                 {return ',';}
"."                 {return '.';}

{SPACES}            /* Ignore spaces, tabs, and line breaks. */

.                   {printf("\nLexical error in line %d: ilegal symbol in '%s'\n", yylineno, yytext); lexical_errors = 1;}

%%