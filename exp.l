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
"=" { /* ASSIGN */ return TOKEN_ASSIGN; }
"(" { /* LP */ return TOKEN_LP; }
")" { /* RP */ return TOKEN_RP; }
"," { /* COMMA */ return TOKEN_COMMA; }
"sub" { /*SUB STRING*/ return TOKEN_SUB_STRING; }
[0-9]*[.][0-9]+   { /* REAL */ return TOKEN_REAL; }
0|[1-9][0-9]*   { /* INTEGER */ return TOKEN_INTEGER; }
[_A-Za-z]+[_A-Za-z0-9]* { /* IDENTIFIER */ return TOKEN_ID; }
\$ast { printAST(ast); }
\$symbol { printSymbol(); }
\n { return BLANK; }
[ \n\t] ;
<<EOF>> { return END_OF_FILE; }
\"([^\\\"]|\\.)*\" { 
  /* STRING */ 
  int size = strlen(yytext);
  strncpy(yytext, yytext + 1, size - 2);
  yytext[size - 2] = NULL;
  return TOKEN_STRING; 
  }
. { printf("Lexical Error in line %d. %x\n", yylineno, yytext[0]); return ERROR; }

%%
int yywrap(void){
  return 1;
}