#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

// Symbol Types
typedef enum {
    TYPE_INT,
    TYPE_BOOL,
    TYPE_VOID,
    TYPE_UNKNOWN
} Type;

// Symbol Categories
typedef enum {
    CAT_VAR,
    CAT_PARAM,
    CAT_FUNCTION,
    CAT_PROCEDURE,
    CAT_PROGRAM
} Category;

// Symbol Struct
typedef struct Symbol {
    char *name;
    Category category;
    int level;
    Type type;
    int offset;
    struct Symbol *next;
} Symbol;

// Scope Struct
typedef struct Scope {
    Symbol *symbols;
    struct Scope *parent;
    int next_offset;
    int level;
} Scope;

// Pointer to current scope
extern Scope *current_scope;

// Symbol table management functions
void enter_scope();
void leave_scope();
Symbol* install(char *name, Category cat, Type type, int level);
Symbol* lookup(char *name);
Symbol* lookup_local(char *name);

#endif
