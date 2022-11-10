%{
  #include "parser.h"
  extern Node* ast;
%}
%option yylineno
%%

"+" { /* ADD */ return TOKEN_ADD; }
"-" { /* SUB */ return TOKEN_SUB; }
"*" { /* MUL */ return TOKEN_MUL; }
"/" { /* DIV */ return TOKEN_DIV; }
[a-zA-Z]+[0-9]* { /* IDENTIFIER */ return TOKEN_ID; }
[0-9]*[.][0-9]+   { /* REAL */ return TOKEN_REAL; }
0|[1-9][0-9]*   { /* INTEGER */ return TOKEN_INTEGER; }
\n { return BLANK; }
[ \n\t] ;
<<EOF>> { return END_OF_FILE; }
\$ast { printAST(); }
\$symbol { printSymbol(); }
. { printf("Lexical Error in line %d\n", yylineno); return ERROR; }

%%
int yywrap(void){
  return 1;
}