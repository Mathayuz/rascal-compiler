#include <stdio.h>
#include <stdlib.h>

#include "rascal_parser.tab.h"
#include "rascal_ast.h"
#include "semantics.h"
#include "rascal_mepa.h"

extern int yylineno;
extern FILE *yyin;
extern Program *ast_root;

int main(int argc, char *argv[]) {
    // Verify arguments
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    // Open file
    FILE *myfile = fopen(argv[1], "r");
    if (!myfile) {
        fprintf(stderr, "Erro ao abrir o arquivo: %s\n", argv[1]);
        return 1;
    }
    yyin = myfile;

    // Lexer through Parser with Abstract Syntax Tree Building
    if (yyparse() != 0 || ast_root == NULL) {
        fprintf(stderr, "Erro na análise Léxica/Sintática.\n");
        fclose(myfile);
        return 1;
    }
    printf("Análise Sintática Ok.\n");

    // Print Abstract Syntax Tree
    printf("\nPrinting AST:\n");
    printAstRoot(ast_root, stdout);

    // Semantic Analysis
    semantic_check(ast_root);
    printf("Semantic analysis OK.\n");

    // Generate Object MEPA Code
    generateCode(ast_root, argv[2]);

    // Free Abstract Syntax Tree and close file
    freeAstRoot(ast_root);
    fclose(myfile);

    return 0;
}