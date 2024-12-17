%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symbol_table.h"

FILE *output_file;
FILE *error_file;
extern FILE *yyin;
void yyerror(const char* s);
int yylex();

ASTNode* ast_root = NULL;

SymbolTable* symbol_table;
int error_count = 0;


int node_is_literal_int_zero(ASTNode* node);
int node_is_literal_float_zero(ASTNode* node);
%}

%union {
    int int_val;
    float float_val;
    char char_val;
    char* id;
    ASTNode* ast;
    BinaryOp bin_op;
}

%token PRINT IF ELSE WHILE
%token TYPE_INT TYPE_FLOAT TYPE_CHAR
%token <id> IDENTIFIER
%token <int_val> NUMBER
%token <float_val> FLOAT_NUMBER
%token <char_val> CHAR_LITERAL
%token PLUS MINUS MULT DIV ASSIGN
%token SEMICOLON LPAREN RPAREN LBRACE RBRACE
%token GT LT GE LE EQ NEQ

%type <ast> program statement declaration assignment print_stmt if_stmt while_stmt block expr term factor float_expr float_term float_factor char_expr

%%

program: 
    program statement 
    {
        if (!ast_root) {
            ast_root = create_program_node();
        }
        add_statement(ast_root, $2);
        $$ = ast_root;
    }
  | statement
    {
        if (!ast_root) {
            ast_root = create_program_node();
        }
        add_statement(ast_root, $1);
        $$ = ast_root;
    }
  ;

statement: 
    declaration   { $$ = $1; }
  | assignment    { $$ = $1; }
  | print_stmt    { $$ = $1; }
  | if_stmt       { $$ = $1; }
  | while_stmt    { $$ = $1; }
  | block         { $$ = $1; }
  ;

declaration:
    TYPE_INT IDENTIFIER SEMICOLON 
    { 
        add_symbol(symbol_table, $2, "int");
        $$ = create_declaration_node("int", $2, NULL);
        free($2);
    }
  | TYPE_FLOAT IDENTIFIER SEMICOLON 
    { 
        add_symbol(symbol_table, $2, "float");
        $$ = create_declaration_node("float", $2, NULL);
        free($2);
    }
  | TYPE_CHAR IDENTIFIER SEMICOLON 
    { 
        add_symbol(symbol_table, $2, "char");
        $$ = create_declaration_node("char", $2, NULL);
        free($2);
    }
  | TYPE_INT IDENTIFIER ASSIGN expr SEMICOLON 
    { 
        add_symbol(symbol_table, $2, "int");
        $$ = create_declaration_node("int", $2, $4);
        free($2);
    }
  | TYPE_FLOAT IDENTIFIER ASSIGN float_expr SEMICOLON 
    { 
        add_symbol(symbol_table, $2, "float");
        $$ = create_declaration_node("float", $2, $4);
        free($2);
    }
  | TYPE_CHAR IDENTIFIER ASSIGN char_expr SEMICOLON 
    { 
        add_symbol(symbol_table, $2, "char");
        $$ = create_declaration_node("char", $2, $4);
        free($2);
    }
  ;

assignment:
    IDENTIFIER ASSIGN expr SEMICOLON 
    { 
        Symbol* sym = lookup_symbol(symbol_table, $1);
        if (!sym) {
            fprintf(error_file, "Undefined variable '%s'\n", $1);
            error_count++;  
            $$ = NULL;
        } else {
            $$ = create_assignment_node($1, $3);
        }
        free($1);
    }
  | IDENTIFIER ASSIGN float_expr SEMICOLON 
    { 
        Symbol* sym = lookup_symbol(symbol_table, $1);
        if (!sym) {
            fprintf(error_file, "Undefined variable '%s'\n", $1);
            error_count++;  
            $$ = NULL;
        } else {
            $$ = create_assignment_node($1, $3);
        }
        free($1);
    }
  | IDENTIFIER ASSIGN char_expr SEMICOLON 
    { 
        Symbol* sym = lookup_symbol(symbol_table, $1);
        if (!sym) {
            fprintf(error_file, "Undefined variable '%s'\n", $1);
            error_count++;  
            $$ = NULL;
        } else {
            $$ = create_assignment_node($1, $3);
        }
        free($1);
    }
  ;

expr: 
    expr PLUS term    { $$ = create_binary_op_node(OP_PLUS, $1, $3); }
  | expr MINUS term   { $$ = create_binary_op_node(OP_MINUS, $1, $3); }
  | expr GT term      { $$ = create_binary_op_node(OP_GT, $1, $3); }
  | expr LT term      { $$ = create_binary_op_node(OP_LT, $1, $3); }
  | expr GE term      { $$ = create_binary_op_node(OP_GE, $1, $3); }
  | expr LE term      { $$ = create_binary_op_node(OP_LE, $1, $3); }
  | expr EQ term      { $$ = create_binary_op_node(OP_EQ, $1, $3); }
  | expr NEQ term     { $$ = create_binary_op_node(OP_NEQ, $1, $3); }
  | term              { $$ = $1; }
  ;

term:
    term MULT factor  { $$ = create_binary_op_node(OP_MULTIPLY, $1, $3); }
  | term DIV factor   { 
        if (node_is_literal_int_zero($3) || node_is_literal_float_zero($3)) {
            fprintf(error_file, "Division by zero\n");
            $$ = create_literal_node(LIT_INT, &(int){0}); // Or handle as needed
        } else {
            $$ = create_binary_op_node(OP_DIVIDE, $1, $3); 
        }
    }
  | factor            { $$ = $1; }
  ;

factor:
    NUMBER           { 
        $$ = create_literal_node(LIT_INT, &(yyvsp[0].int_val)); 
    }
  | FLOAT_NUMBER     { 
        $$ = create_literal_node(LIT_FLOAT, &(yyvsp[0].float_val)); 
    }
  | CHAR_LITERAL     { 
        $$ = create_literal_node(LIT_CHAR, &(yyvsp[0].char_val)); 
    }
  | IDENTIFIER       { 
        $$ = create_identifier_node($1); 
        free($1); 
    }
  | LPAREN expr RPAREN { 
        $$ = $2; 
    }
  ;

print_stmt:
    PRINT expr SEMICOLON 
    { 
        $$ = create_print_node($2);
    }
  | PRINT float_expr SEMICOLON 
    { 
        $$ = create_print_node($2);
    }
  | PRINT char_expr SEMICOLON 
    { 
        $$ = create_print_node($2);
    }
  ;

if_stmt:
    IF LPAREN expr RPAREN statement 
    { 
        $$ = create_if_node($3, $5, NULL);
    }
  | IF LPAREN expr RPAREN statement ELSE statement 
    { 
        $$ = create_if_node($3, $5, $7);
    }
  ;

while_stmt:
    WHILE LPAREN expr RPAREN statement 
    { 
        $$ = create_while_node($3, $5);
    }
  ;

block:
    LBRACE program RBRACE 
    { 
        $$ = node_to_block($2);
    }
  ;

float_expr:
    float_expr PLUS float_term     { $$ = create_binary_op_node(OP_PLUS, $1, $3); }
  | float_expr MINUS float_term    { $$ = create_binary_op_node(OP_MINUS, $1, $3); }
  | float_term                     { $$ = $1; }
  ;

float_term:
    float_term MULT float_factor   { $$ = create_binary_op_node(OP_MULTIPLY, $1, $3); }
  | float_term DIV float_factor    { 
        if (node_is_literal_float_zero($3)) {
            fprintf(error_file, "Division by zero\n");
            $$ = create_literal_node(LIT_FLOAT, &(float){0.0f});
        } else {
            $$ = create_binary_op_node(OP_DIVIDE, $1, $3); 
        }
    }
  | float_factor                   { $$ = $1; }
  ;

float_factor:
    FLOAT_NUMBER                   { 
        $$ = create_literal_node(LIT_FLOAT, &(yyvsp[0].float_val)); 
    }
  | IDENTIFIER                     { 
        $$ = create_identifier_node($1); 
        free($1); 
    }
  | LPAREN float_expr RPAREN      { 
        $$ = $2; 
    }
  ;

char_expr:
    CHAR_LITERAL                   { 
        $$ = create_literal_node(LIT_CHAR, &(yyvsp[0].char_val)); 
    }
  | IDENTIFIER                     { 
        $$ = create_identifier_node($1); 
        free($1); 
    }
  ;

%%

int node_is_literal_int_zero(ASTNode* node) {
    return (node->type == AST_LITERAL && node->data.literal.lit_type == LIT_INT && node->data.literal.value.int_val == 0);
}

int node_is_literal_float_zero(ASTNode* node) {
    return (node->type == AST_LITERAL && node->data.literal.lit_type == LIT_FLOAT && node->data.literal.value.float_val == 0.0f);
}

void yyerror(const char* s) {
    error_count++;
    fprintf(error_file, "Error: %s\n", s);
}

int main(void) {
    symbol_table = create_symbol_table();

    FILE *input = fopen("input.txt", "r");
    if (!input) {
        fprintf(stderr, "Error opening input.txt\n");
        exit(EXIT_FAILURE);
    }
    yyin = input;

    output_file = fopen("output.txt", "w");
    if (!output_file) {
        fprintf(stderr, "Error opening output.txt\n");
        fclose(input);
        exit(EXIT_FAILURE);
    }

    error_file = fopen("error.txt", "w");
    if (!error_file) {
        fprintf(stderr, "Error opening error.txt\n");
        fclose(input);
        fclose(output_file);
        exit(EXIT_FAILURE);
    }

    ast_root = NULL;

    int result = yyparse();

    if (result == 0 && ast_root && error_count == 0) {
        FILE* original_stdout = stdout;
        stdout = output_file;
        print_ast(ast_root, 0);
        stdout = original_stdout;
    } else {
        fprintf(error_file, "Parsing failed with %d error(s).\n", error_count);
    }

    free_ast(ast_root);
    free_symbol_table(symbol_table);
    
    fclose(input);
    fclose(output_file);
    fclose(error_file);

    return result;
}