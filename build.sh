flex lexer.l
bison -d parser.y
gcc lex.yy.c parser.tab.c symbol_table.c ast.c -o parser