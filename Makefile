all: rascalc

rascalc: rascal_ast.o rascal_parser.tab.o lex.yy.o
	gcc $^ -o $@ -lfl

rascal_parser.tab.c rascal_parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: rascal_lexer.l rascal_parser.tab.h
	flex rascal_lexer.l

rascal_ast.o: rascal_ast.c rascal_ast.h
	gcc -c rascal_ast.c

rascal_parser.tab.o: rascal_parser.tab.c rascal_ast.h
	gcc -c rascal_parser.tab.c

lex.yy.o: lex.yy.c
	gcc -c lex.yy.c

main.o: main.c rascal_ast.h
	gcc -c main.c

clean:
	rm -f rascalc *.o rascal_parser.tab.* lex.yy.c parser.output

runOK:
	./rascalc exemplo1.ras

runErro:
	./rascalc exemplo_erro.ras

.PHONY: all clean runErro runOK