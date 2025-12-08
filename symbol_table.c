#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

// Escopo atual da tabela
Scope *current_scope = NULL;

// Cria um novo escopo
void enter_scope() {
    Scope *s = (Scope*) malloc(sizeof(Scope));
    s->symbols = NULL;
    s->parent = current_scope;
    s->next_offset = 0;

    if (current_scope == NULL)
        s->level = 0;
    else
        s->level = current_scope->level + 1;

    current_scope = s;
}

// Remove o escopo atual
void leave_scope() {
    if (current_scope == NULL)
        return;

    Scope *old = current_scope;
    Symbol *sym = old->symbols;

    while (sym != NULL) {
        Symbol *next = sym->next;
        free(sym->name);
        free(sym);
        sym = next;
    }

    current_scope = old->parent;
    free(old);
}

// Busca apenas no escopo atual
Symbol* lookup_local(char *name) {
    if (current_scope == NULL)
        return NULL;

    Symbol *sym = current_scope->symbols;

    while (sym != NULL) {
        if (strcmp(sym->name, name) == 0)
            return sym;
        sym = sym->next;
    }

    return NULL;
}

// Busca hierárquica (escopo atual -> anteriores)
Symbol* lookup(char *name) {
    Scope *s = current_scope;

    while (s != NULL) {
        Symbol *sym = s->symbols;
        while (sym != NULL) {
            if (strcmp(sym->name, name) == 0)
                return sym;
            sym = sym->next;
        }
        s = s->parent;
    }

    return NULL;
}

// Instala um novo simbolo no escopo atual
Symbol* install(char *name, Category cat, Type type, int level) {
    if (current_scope == NULL)
        return NULL;

    Symbol *exist = lookup_local(name);
    if (exist != NULL) {
        printf("Erro semântico: identificador '%s' declarado duas vezes no mesmo escopo\n", name);
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

