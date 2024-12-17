%{
#include "symbol_table.h"
#include "ast.h"
#include "parser.tab.h"
#include <stdio.h>
#include <string.h>

extern SymbolTable* symbol_table;

extern FILE *error_file;

%}

%option noyywrap

%%
#.*(\r\n|\n)?        { /* Skip comments until end of line */ }
"+"          { return PLUS; }
"-"          { return MINUS; }
"*"          { return MULT; }
"/"          { return DIV; }
"="          { return ASSIGN; }
";"          { return SEMICOLON; }
"("          { return LPAREN; }
")"          { return RPAREN; }
"{"          { return LBRACE; }
"}"          { return RBRACE; }
">="         { return GE; }
"<="         { return LE; }
"=="         { return EQ; }
"!="         { return NEQ; }
">"          { return GT; }
"<"          { return LT; }
[ \t\r\n]+   { /* Skip whitespace */ }

"print"      { return PRINT; }
"if"         { return IF; }
"else"       { return ELSE; }
"while"      { return WHILE; }
"int"        { return TYPE_INT; }
"float"      { return TYPE_FLOAT; }
"char"       { return TYPE_CHAR; }

\'([^\\'\n]|\\.)\' { 
    if (yyleng == 3) { // Simple character like 'A'
        yylval.char_val = yytext[1];
    } else if (yyleng == 4 && yytext[1] == '\\') { 
        switch (yytext[2]) {
            case 'n': yylval.char_val = '\n'; break;
            case 't': yylval.char_val = '\t'; break;
            case '\\': yylval.char_val = '\\'; break;
            case '\'': yylval.char_val = '\''; break;
            case '\"': yylval.char_val = '\"'; break;
            default: 
                fprintf(error_file, "Unknown escape sequence: \\%c\n", yytext[2]);
                yylval.char_val = '\0'; 
                break;
        }
    } else {
        fprintf(error_file, "Invalid character literal: %s\n", yytext);
        yylval.char_val = '\0';
    }
    return CHAR_LITERAL;
}

[0-9]+\.[0-9]+([eE][-+]?[0-9]+)? { 
    yylval.float_val = atof(yytext); 
    return FLOAT_NUMBER; 
}

[0-9]+ { 
    yylval.int_val = atoi(yytext); 
    return NUMBER; 
}

[a-zA-Z_][a-zA-Z0-9_]* { 
    yylval.id = strdup(yytext); 
    return IDENTIFIER; 
}

. { 
    fprintf(error_file, "Unknown character: %s\n", yytext);
    return YYUNDEF; 
}

%%