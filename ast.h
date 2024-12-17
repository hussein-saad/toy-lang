#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Enum for different AST node types
typedef enum {
    AST_PROGRAM,
    AST_DECLARATION,
    AST_ASSIGNMENT,
    AST_PRINT,
    AST_IF,
    AST_WHILE,
    AST_BLOCK,
    AST_BINARY_OP,
    AST_LITERAL,
    AST_IDENTIFIER
} ASTNodeType;

// Enum for binary operators
typedef enum {
    OP_PLUS,
    OP_MINUS,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_GT,
    OP_LT,
    OP_GE,
    OP_LE,
    OP_EQ,
    OP_NEQ
} BinaryOp;

// Enum for literal types
typedef enum {
    LIT_INT,
    LIT_FLOAT,
    LIT_CHAR
} LiteralType;

// Structure for AST Nodes
typedef struct ASTNode {
    ASTNodeType type;
    union {
        // Program node: list of statements
        struct {
            struct ASTNode** statements;
            int stmt_count;
        } program;

        // Declaration node
        struct {
            char* var_type;
            char* var_name;
            struct ASTNode* initializer;
        } declaration;

        // Assignment node
        struct {
            char* var_name;
            struct ASTNode* value;
        } assignment;

        // Print node
        struct {
            struct ASTNode* expression;
        } print;

        // If node
        struct {
            struct ASTNode* condition;
            struct ASTNode* then_branch;
            struct ASTNode* else_branch;
        } if_stmt;

        // While node
        struct {
            struct ASTNode* condition;
            struct ASTNode* body;
        } while_stmt;

        // Block node
        struct {
            struct ASTNode** statements;
            int stmt_count;
        } block;

        // Binary operation node
        struct {
            BinaryOp op;
            struct ASTNode* left;
            struct ASTNode* right;
        } binary_op;

        // Literal node
        struct {
            LiteralType lit_type;
            union {
                int int_val;
                float float_val;
                char char_val;
            } value;
        } literal;

        // Identifier node
        char* identifier;
    } data;
} ASTNode;

ASTNode* create_program_node();
void add_statement(ASTNode* program, ASTNode* stmt);
ASTNode* create_declaration_node(const char* var_type, const char* var_name, ASTNode* initializer);
ASTNode* create_assignment_node(const char* var_name, ASTNode* value);
ASTNode* create_print_node(ASTNode* expression);
ASTNode* create_if_node(ASTNode* condition, ASTNode* then_branch, ASTNode* else_branch);
ASTNode* create_while_node(ASTNode* condition, ASTNode* body);
ASTNode* node_to_block(ASTNode* node);
ASTNode* create_binary_op_node(BinaryOp op, ASTNode* left, ASTNode* right);
ASTNode* create_literal_node(LiteralType type, void* value);
ASTNode* create_identifier_node(const char* name);

void print_ast(ASTNode* node, int indent);
void free_ast(ASTNode* node);



#endif /* AST_H */