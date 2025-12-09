#ifndef RASCAL_MEPA_H
#define RASCAL_MEPA_H

#include "rascal_ast.h"

// Keeps the code generation context
typedef struct CodeGenContext {
    FILE *mepaFile;
    int labelCount;
    int currentLevel;
} CodeGenContext;

// Executes the MEPA code generation
void generateCode(Program *root, const char *filename);

#endif