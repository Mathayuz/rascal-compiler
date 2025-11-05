#include "rascal_ast.h"

// - Constructors -------------------------

// Program Node Constructor
Program* newProgram(char* identifier, Block* block) {
    Program* p = (Program*)malloc(sizeof(Program));
    p->identifier = identifier;
    p->block = block;
    return p;
}

// Block Node Constructor
Block* newBlock(VarDeclaration* varDeclarations, SubRotDeclaration* subRotDeclarations, Command* commandList) {
    Block* b = (Block*)malloc(sizeof(Block));
    b->varDeclarations = varDeclarations;
    b->subRotDeclarations = subRotDeclarations;
    b->commandList = commandList;
    return b;
}

// Variable Declaration Node Constructor and AddListFunc
VarDeclaration* newVarDeclaration(varType type, char* identifier) {
    VarDeclaration* vd = (VarDeclaration*)malloc(sizeof(VarDeclaration));
    vd->type = type;
    vd->identifier = identifier;
    vd->next = NULL;
    return vd;
}

VarDeclaration* addVarDeclaration(VarDeclaration* list, VarDeclaration* newVarDecl) {
    if(!list) return newVarDecl;
    Command* it = list;
    while(it->next) it = it->next;
    it->next = newVarDecl;
    return list;
}

// - Free ---------------------------------

// - Print --------------------------------