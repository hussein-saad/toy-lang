#include <stdlib.h>
#include <string.h>

#include "symbol_table.h"

extern FILE *error_file;
extern int error_count;

SymbolTable* create_symbol_table() {
    SymbolTable* table = malloc(sizeof(SymbolTable));
    if (!table) {
        fprintf(error_file, "Failed to allocate memory for symbol table\n");
        error_count++;
        exit(EXIT_FAILURE);
    }
    table->head = NULL;
    return table;
}

void add_symbol(SymbolTable* table, const char* name, const char* type) {
    Symbol* existing = lookup_symbol(table, name);
    if (existing) {
        fprintf(error_file, "Symbol '%s' already declared.\n", name);
        error_count++;
        return;
    }

    Symbol* sym = malloc(sizeof(Symbol));
    if (!sym) {
        fprintf(error_file, "Failed to allocate memory for symbol\n");
        error_count++;
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