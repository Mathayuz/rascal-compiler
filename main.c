#include <stdio.h>
#include <stdlib.h>
#include "rascal_parser.tab.h"
#include "rascal_ast.h"

extern int yylineno;

int main() {
    if (yyparse() == 0 && ast_root != NULL) {
        printf("Printing AST:\n");
        printAstRoot(ast_root, stdout);
        freeAstRoot(ast_root);
    } else {
        fprintf(stderr, "Parsing failed.\n");
    }
    return 0;
}