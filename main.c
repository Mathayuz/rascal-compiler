#include <stdio.h>
#include <stdlib.h>

#include "rascal_parser.tab.h"
#include "rascal_ast.h"
#include "semantics.h"     // << ADICIONADO
#include "rascal_mepa.h"

extern int yylineno;
extern FILE *yyin;
extern Program *ast_root;   // vem do rascal_ast.h

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_entrada>\n", argv[0]);
        return 1;
    }

    FILE *myfile = fopen(argv[1], "r");
    if (!myfile) {
        fprintf(stderr, "Erro ao abrir o arquivo: %s\n", argv[1]);
        return 1;
    }
    yyin = myfile;

    /* === PARSING === */
    if (yyparse() != 0 || ast_root == NULL) {
        fprintf(stderr, "Parsing failed.\n");
        fclose(myfile);
        return 1;
    }

    printf("Parsing OK.\n");

    /* === SEMANTIC ANALYSIS === */
    semantic_check(ast_root);
    printf("Semantic analysis OK.\n");

    /* === PRINT AST === */
    printf("\nPrinting AST:\n");
    printAstRoot(ast_root, stdout);

    freeAstRoot(ast_root);
    fclose(myfile);
    return 0;
}


// int main(int argc, char *argv[]) {
//     // ROTINA PARA TESTAR A GERAÇÃO DE CÓDIGO MEPA
//     if (argc < 2) {
//         fprintf(stderr, "Uso: %s <arquivo_entrada>\n", argv[0]);
//         return 1;
//     }

//     FILE *myfile = fopen(argv[1], "r");
//     if (!myfile) {
//         fprintf(stderr, "Erro ao abrir o arquivo: %s\n", argv[1]);
//         return 1;
//     }
//     yyin = myfile;

//     if (yyparse() != 0 || ast_root == NULL) {
//         fprintf(stderr, "Parsing failed.\n");
//         fclose(myfile);
//         return 1;
//     }

//     printf("Parsing OK.\n");

//     printf("\n--- AST ---\n");
//     printAstRoot(ast_root, stdout);
//     printf("\n-----------\n");

//     printf("Gerando código MEPA para: %s\n", "saida.mep");
//     generate_code(ast_root, "saida.mep");

//     freeAstRoot(ast_root);
//     fclose(myfile);
//     return 0;
// }
