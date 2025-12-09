#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

// Current scope of symbol table
Scope *current_scope = NULL;

// Creates a new scope
void enter_scope() {
    Scope *s = (Scope*) malloc(sizeof(Scope));
    s->symbols = NULL;
    s->parent = current_scope;
    s->next_offset = 0;

    if (!current_scope)
        s->level = 0;
    else
        s->level = current_scope->level + 1;

    current_scope = s;
}

// Destroys the current scope of symbol table
void leave_scope() {
    if (!current_scope) return;

    Scope *old = current_scope;
    Symbol *sym = old->symbols;

    while (sym) {
        Symbol *next = sym->next;
        free(sym->name);
        free(sym);
        sym = next;
    }

    current_scope = old->parent;
    free(old);
}

// Search only in the current scope
Symbol* lookup_local(char *name) {
    if (!current_scope) return NULL;

    Symbol *sym = current_scope->symbols;

    while (sym) {
        if (strcmp(sym->name, name) == 0)
            return sym;
        sym = sym->next;
    }

    return NULL;
}

// Hierarchical search (current scopes -> previous scopes)
Symbol* lookup(char *name) {
    Scope *s = current_scope;

    while (s) {
        Symbol *sym = s->symbols;
        while (sym) {
            if (strcmp(sym->name, name) == 0)
                return sym;
            sym = sym->next;
        }
        s = s->parent;
    }

    return NULL;
}

// Insert a new symbol in the current scope
Symbol* install(char *name, Category cat, Type type, int level) {
    if (!current_scope) return NULL;

    Symbol *exist = lookup_local(name);
    if (exist) {
        printf("\nSemantic error: identifier '%s' declared twice in the same scope.\n", name);
        exit(1);
    }

    Symbol *s = (Symbol*) malloc(sizeof(Symbol));
    s->name = strdup(name);
    s->category = cat;
    s->type = type;
    s->level = level;
    s->next = current_scope->symbols;

    if (cat == CAT_VAR) {
        s->offset = current_scope->next_offset++;
    } else {
        s->offset = -1; 
    }

    current_scope->symbols = s;

    return s;
}

