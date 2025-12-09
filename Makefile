# Definitions
CC = gcc
CFLAGS = -g -Wall
LIBS = -lfl

# Main Target
all: rascalc

# Linking
rascalc: rascal_parser.tab.o lex.yy.o rascal_ast.o symbol_table.o semantics.o rascal_mepa.o main.o
	$(CC) $(CFLAGS) -o rascalc \
		rascal_parser.tab.o lex.yy.o rascal_ast.o \
		symbol_table.o semantics.o rascal_mepa.o main.o $(LIBS)

# Bison Compilation
rascal_parser.tab.c rascal_parser.tab.h: rascal_parser.y
	bison -d rascal_parser.y

# Lexer Compilation
lex.yy.c: rascal_lexer.l rascal_parser.tab.h
	flex rascal_lexer.l

# Objects Compilation
# Lexer
lex.yy.o: lex.yy.c rascal_parser.tab.h rascal_ast.h
	$(CC) $(CFLAGS) -c lex.yy.c

# Parser
rascal_parser.tab.o: rascal_parser.tab.c rascal_ast.h
	$(CC) $(CFLAGS) -c rascal_parser.tab.c

# Abstract Syntax Tree
rascal_ast.o: rascal_ast.c rascal_ast.h
	$(CC) $(CFLAGS) -c rascal_ast.c

# Symbol Table
symbol_table.o: symbol_table.c symbol_table.h
	$(CC) $(CFLAGS) -c symbol_table.c

# Semantics
semantics.o: semantics.c semantics.h rascal_ast.h symbol_table.h
	$(CC) $(CFLAGS) -c semantics.c

# MEPA Code Generator
rascal_mepa.o: rascal_mepa.c rascal_mepa.h rascal_ast.h symbol_table.h
	$(CC) $(CFLAGS) -c rascal_mepa.c

# Main
main.o: main.c rascal_ast.h rascal_parser.tab.h semantics.h rascal_mepa.h
	$(CC) $(CFLAGS) -c main.c

# Utils
clean:
	rm -f rascalc *.o rascal_parser.tab.* lex.yy.c *.mep

# Quick tests
run: rascalc
	./rascalc teste.ras saida.mep

runOK: rascalc
	./rascalc exemplo1.ras saida_ok.mep

runErro: rascalc
	./rascalc exemplo_erro.ras saida_erro.mep

.PHONY: all clean run runOK runErro
