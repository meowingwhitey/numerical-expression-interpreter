%{
  #include "parser.h"
%}
%option yylineno
%%

"+" { /* ADD */ return TOKEN_ADD; }
"-" { /* SUB */ return TOKEN_SUB; }
"*" { /* MUL */ return TOKEN_MUL; }
"/" { /* DIV */ return TOKEN_DIV; }
[a-zA-Z]+[0-9]* { /* IDENTIFIER */ return TOKEN_ID; }
[0-9][0-9]*"."[0-9][0-9]+   { /* INTEGER */ return TOKEN_INTEGER; }
0|[1-9][0-9]*   { /* INTEGER */ return TOKEN_REAL; }
"$"ast { printAST(); }
"$"symbol { printSymbol(); }
[" "\t\n]+ ;
<<EOF>> { return END_OF_FILE; }
. { printf("Lexical Error\n"); return ERROR; }

%%
int yywrap(void){
  return 1;
}