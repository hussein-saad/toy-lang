#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Symbol {
    char* name;
    char* type;
    struct Symbol* next;
} Symbol;

typedef struct SymbolTable {
    Symbol* head;
} SymbolTable;

SymbolTable* create_symbol_table();
void add_symbol(SymbolTable* table, const char* name, const char* type);
Symbol* lookup_symbol(SymbolTable* table, const char* name);
void free_symbol_table(SymbolTable* table);

#endif /* SYMBOL_TABLE_H */