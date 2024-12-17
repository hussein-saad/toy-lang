#include "symbol_table.h"

SymbolTable* create_symbol_table() {
    SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!table) {
        fprintf(stderr, "Memory allocation failed for SymbolTable\n");
        exit(1);
    }
    table->head = NULL;
    return table;
}

void add_symbol(SymbolTable* table, const char* name, const char* type) {
    if (lookup_symbol(table, name)) {
        fprintf(stderr, "Symbol '%s' already declared.\n", name);
        return;
    }
    Symbol* sym = (Symbol*)malloc(sizeof(Symbol));
    if (!sym) {
        fprintf(stderr, "Memory allocation failed for Symbol\n");
        exit(1);
    }
    sym->name = strdup(name);
    sym->type = strdup(type);
    sym->next = table->head;
    table->head = sym;
}

Symbol* lookup_symbol(SymbolTable* table, const char* name) {
    Symbol* current = table->head;
    while (current) {
        if (strcmp(current->name, name) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

void free_symbol_table(SymbolTable* table) {
    Symbol* current = table->head;
    while (current) {
        Symbol* temp = current;
        current = current->next;
        free(temp->name);
        free(temp->type);
        free(temp);
    }
    free(table);
}