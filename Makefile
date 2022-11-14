parser_201720857 : exp.l
	flex exp.l
	gcc -g -c -o parser.o parser.c
	gcc -g -c -o lex.yy.o lex.yy.c
	gcc -g -o $@.out parser.o lex.yy.o -lfl

.PHONY : clean 
clean : 
	rm -rf *.tab.c *.tab.h *.yy.c *.out *.o
