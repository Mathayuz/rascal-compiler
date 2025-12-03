%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rascal_parser.tab.h"
#include "rascal_ast.h"
%}

%option noyywrap yylineno

SPACES      [ \t\n\r]+
DIGIT       [0-9]
LETTER      [a-zA-Z]
ID          {LETTER}({LETTER}|{DIGIT}|_)*
NUM         {DIGIT}*

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

{NUM}               {yylval.dval = strtod(yytext, NULL); return NUM;}
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

{SPACES}            /* Ignorar espaços, tabulações e quebras de linha */

.                   {printf("ERRO LÉXICO na linha %d: símbolo ilegal %c\n", yylineno, yytext[0]);}

%%