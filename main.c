#include <stdio.h>
#include <stdlib.h>
#include "rascal_parser.tab.h"
#include "rascal_ast.h"

extern int yylineno;
extern FILE *yyin;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_entrada> [arquivo_saida]\n", argv[0]);
        return 1;
    }

    FILE *myfile = fopen(argv[1], "r");
    if (!myfile) {
        fprintf(stderr, "Erro ao abrir o arquivo: %s\n", argv[1]);
        return 1;
    }
    yyin = myfile;

    if (yyparse() == 0 && ast_root != NULL) {
        printf("Printing AST:\n");
        printAstRoot(ast_root, stdout);
        freeAstRoot(ast_root);
    } else {
        fprintf(stderr, "Parsing failed.\n");
    }

    fclose(myfile);
    return 0;
}