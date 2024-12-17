#include "ast.h"

ASTNode* create_program_node() {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Failed to allocate memory for program node");
        exit(EXIT_FAILURE);
    }
    node->type = AST_PROGRAM;
    node->data.program.statements = NULL;
    node->data.program.stmt_count = 0;
    return node;
}

void add_statement(ASTNode* program, ASTNode* stmt) {
    if (program->type != AST_PROGRAM && program->type != AST_BLOCK) {
        fprintf(stderr, "add_statement called on non-program/block node\n");
        return;
    }

    ASTNode** new_statements = realloc(program->data.program.statements, sizeof(ASTNode*) * (program->data.program.stmt_count + 1));
    if (!new_statements) {
        perror("Failed to realloc statements");
        exit(EXIT_FAILURE);
    }
    program->data.program.statements = new_statements;
    program->data.program.statements[program->data.program.stmt_count] = stmt;
    program->data.program.stmt_count += 1;
}

ASTNode* create_declaration_node(const char* var_type, const char* var_name, ASTNode* initializer) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Failed to allocate memory for declaration node");
        exit(EXIT_FAILURE);
    }
    node->type = AST_DECLARATION;
    node->data.declaration.var_type = strdup(var_type);
    node->data.declaration.var_name = strdup(var_name);
    node->data.declaration.initializer = initializer;
    return node;
}

ASTNode* create_assignment_node(const char* var_name, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Failed to allocate memory for assignment node");
        exit(EXIT_FAILURE);
    }
    node->type = AST_ASSIGNMENT;
    node->data.assignment.var_name = strdup(var_name);
    node->data.assignment.value = value;
    return node;
}

ASTNode* create_print_node(ASTNode* expression) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Failed to allocate memory for print node");
        exit(EXIT_FAILURE);
    }
    node->type = AST_PRINT;
    node->data.print.expression = expression;
    return node;
}

ASTNode* create_if_node(ASTNode* condition, ASTNode* then_branch, ASTNode* else_branch) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Failed to allocate memory for if node");
        exit(EXIT_FAILURE);
    }
    node->type = AST_IF;
    node->data.if_stmt.condition = condition;
    node->data.if_stmt.then_branch = then_branch;
    node->data.if_stmt.else_branch = else_branch;
    return node;
}

ASTNode* create_while_node(ASTNode* condition, ASTNode* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Failed to allocate memory for while node");
        exit(EXIT_FAILURE);
    }
    node->type = AST_WHILE;
    node->data.while_stmt.condition = condition;
    node->data.while_stmt.body = body;
    return node;
}

ASTNode* node_to_block(ASTNode* node) {
    if (node->type != AST_PROGRAM) {
        fprintf(stderr, "node_to_block called on non-program node\n");
        return NULL;
    }

    ASTNode* block = malloc(sizeof(ASTNode));
    if (!block) {
        perror("Failed to allocate memory for block node");
        exit(EXIT_FAILURE);
    }
    block->type = AST_BLOCK;
    block->data.block.statements = node->data.program.statements;
    block->data.block.stmt_count = node->data.program.stmt_count;

    // Reset the program node's statements to avoid double free
    node->data.program.statements = NULL;
    node->data.program.stmt_count = 0;

    return block;
}

ASTNode* create_binary_op_node(BinaryOp op, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Failed to allocate memory for binary operation node");
        exit(EXIT_FAILURE);
    }
    node->type = AST_BINARY_OP;
    node->data.binary_op.op = op;
    node->data.binary_op.left = left;
    node->data.binary_op.right = right;
    return node;
}

ASTNode* create_literal_node(LiteralType type, void* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Failed to allocate memory for literal node");
        exit(EXIT_FAILURE);
    }
    node->type = AST_LITERAL;
    node->data.literal.lit_type = type;

    switch (type) {
        case LIT_INT:
            node->data.literal.value.int_val = *(int*)value;
            break;
        case LIT_FLOAT:
            node->data.literal.value.float_val = *(float*)value;
            break;
        case LIT_CHAR:
            node->data.literal.value.char_val = *(char*)value;
            break;
        default:
            fprintf(stderr, "Unknown LiteralType in create_literal_node\n");
            break;
    }

    return node;
}

ASTNode* create_identifier_node(const char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Failed to allocate memory for identifier node");
        exit(EXIT_FAILURE);
    }
    node->type = AST_IDENTIFIER;
    node->data.identifier = strdup(name);
    return node;
}

void print_ast(ASTNode* node, int indent) {
    if (!node) return;

    for (int i = 0; i < indent; i++) printf("  ");

    switch (node->type) {
        case AST_PROGRAM:
            printf("Program:\n");
            for (int i = 0; i < node->data.program.stmt_count; i++) {
                print_ast(node->data.program.statements[i], indent + 1);
            }
            break;
        case AST_DECLARATION:
            printf("Declaration: %s %s\n", node->data.declaration.var_type, node->data.declaration.var_name);
            if (node->data.declaration.initializer) {
                print_ast(node->data.declaration.initializer, indent + 1);
            }
            break;
        case AST_ASSIGNMENT:
            printf("Assignment: %s =\n", node->data.assignment.var_name);
            print_ast(node->data.assignment.value, indent + 1);
            break;
        case AST_PRINT:
            printf("Print:\n");
            print_ast(node->data.print.expression, indent + 1);
            break;
        case AST_IF:
            printf("If:\nCondition:\n");
            print_ast(node->data.if_stmt.condition, indent + 1);
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("Then:\n");
            print_ast(node->data.if_stmt.then_branch, indent + 1);
            if (node->data.if_stmt.else_branch) {
                for (int i = 0; i < indent + 1; i++) printf("  ");
                printf("Else:\n");
                print_ast(node->data.if_stmt.else_branch, indent + 1);
            }
            break;
        case AST_WHILE:
            printf("While:\nCondition:\n");
            print_ast(node->data.while_stmt.condition, indent + 1);
            printf("Body:\n");
            print_ast(node->data.while_stmt.body, indent + 1);
            break;
        case AST_BLOCK:
            printf("Block:\n");
            for (int i = 0; i < node->data.block.stmt_count; i++) {
                print_ast(node->data.block.statements[i], indent + 1);
            }
            break;
        case AST_BINARY_OP:
            printf("BinaryOp: ");
            switch (node->data.binary_op.op) {
                case OP_PLUS: printf("+"); break;
                case OP_MINUS: printf("-"); break;
                case OP_MULTIPLY: printf("*"); break;
                case OP_DIVIDE: printf("/"); break;
                case OP_GT: printf(">"); break;
                case OP_LT: printf("<"); break;
                case OP_GE: printf(">="); break;
                case OP_LE: printf("<="); break;
                case OP_EQ: printf("=="); break;
                case OP_NEQ: printf("!="); break;
                default: printf("UnknownOp"); break;
            }
            printf("\n");
            print_ast(node->data.binary_op.left, indent + 1);
            print_ast(node->data.binary_op.right, indent + 1);
            break;
        case AST_LITERAL:
            printf("Literal: ");
            switch (node->data.literal.lit_type) {
                case LIT_INT: printf("%d\n", node->data.literal.value.int_val); break;
                case LIT_FLOAT: printf("%f\n", node->data.literal.value.float_val); break;
                case LIT_CHAR: printf("'%c'\n", node->data.literal.value.char_val); break;
                default: printf("UnknownLiteral\n"); break;
            }
            break;
        case AST_IDENTIFIER:
            printf("Identifier: %s\n", node->data.identifier);
            break;
        default:
            printf("Unknown AST node type\n");
    }
}

void free_ast(ASTNode* node) {
    if (!node) return;
    switch (node->type) {
        case AST_PROGRAM:
            for(int i = 0; i < node->data.program.stmt_count; i++) {
                free_ast(node->data.program.statements[i]);
            }
            free(node->data.program.statements);
            break;
        case AST_DECLARATION:
            free(node->data.declaration.var_type);
            free(node->data.declaration.var_name);
            free_ast(node->data.declaration.initializer);
            break;
        case AST_ASSIGNMENT:
            free(node->data.assignment.var_name);
            free_ast(node->data.assignment.value);
            break;
        case AST_PRINT:
            free_ast(node->data.print.expression);
            break;
        case AST_IF:
            free_ast(node->data.if_stmt.condition);
            free_ast(node->data.if_stmt.then_branch);
            if (node->data.if_stmt.else_branch)
                free_ast(node->data.if_stmt.else_branch);
            break;
        case AST_WHILE:
            free_ast(node->data.while_stmt.condition);
            free_ast(node->data.while_stmt.body);
            break;
        case AST_BLOCK:
            for(int i = 0; i < node->data.block.stmt_count; i++) {
                free_ast(node->data.block.statements[i]);
            }
            free(node->data.block.statements);
            break;
        case AST_BINARY_OP:
            free_ast(node->data.binary_op.left);
            free_ast(node->data.binary_op.right);
            break;
        case AST_LITERAL:
            break;
        case AST_IDENTIFIER:
            free(node->data.identifier);
            break;
        default:
            break;
    }
    free(node);
}
