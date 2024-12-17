#include "symbol_table.h"

SymbolTable* create_symbol_table() {
    SymbolTable* table = malloc(sizeof(SymbolTable));
    if (!table) {
        perror("Failed to allocate memory for symbol table");
        exit(EXIT_FAILURE);
    }
    table->head = NULL;
    return table;
}

void add_symbol(SymbolTable* table, const char* name, const char* type) {
    Symbol* existing = lookup_symbol(table, name);
    if (existing) {
        fprintf(stderr, "Symbol '%s' already declared.\n", name);
        return;
    }

    Symbol* sym = malloc(sizeof(Symbol));
    if (!sym) {
        perror("Failed to allocate memory for symbol");
        exit(EXIT_FAILURE);
    }
    sym->name = strdup(name);
    sym->type = strdup(type);
    sym->next = table->head;
    table->head = sym;
}

Symbol* lookup_symbol(SymbolTable* table, const char* name) {
    Symbol* current = table->head;
    while(current) {
        if (strcmp(current->name, name) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

void free_symbol_table(SymbolTable* table) {
    Symbol* current = table->head;
    while(current) {
        Symbol* tmp = current;
        current = current->next;
        free(tmp->name);
        free(tmp->type);
        free(tmp);
    }
    free(table);
}