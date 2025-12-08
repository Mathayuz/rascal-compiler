#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

// Tipos primitivos
typedef enum {
    TYPE_INT,
    TYPE_BOOL,
    TYPE_VOID,
    TYPE_UNKNOWN
} Type;

// Categorias de identificadores
typedef enum {
    CAT_VAR,
    CAT_PARAM,
    CAT_FUNCTION,
    CAT_PROCEDURE,
    CAT_PROGRAM
} Category;

// Estrutura de um simbolo
typedef struct Symbol {
    char *name;
    Category category;
    int level;
    Type type;
    int offset;
    struct Symbol *next;
} Symbol;

// Estrutura de um escopo
typedef struct Scope {
    Symbol *symbols;
    struct Scope *parent;
    int next_offset;
    int level;
} Scope;

// Ponteiro para o escopo atual
extern Scope *current_scope;

// Funções da tabela de simbolos
void enter_scope();
void leave_scope();
Symbol* install(char *name, Category cat, Type type, int level);
Symbol* lookup(char *name);
Symbol* lookup_local(char *name);

#endif
