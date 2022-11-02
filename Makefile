parser_201720857 : exp.l
	flex exp.l
	gcc -c -o parser.o parser.c
	gcc -c -o lex.yy.o lex.yy.c
	gcc -o $@.out parser.o lex.yy.o -lfl

.PHONY : clean 
clean : 
	rm -rf *.tab.c *.tab.h *.yy.c parser_201720857.* parser.o lex.yy.o
