%{
  #include "parser.h"
  extern Node* ast;
  extern int char_pos;
  extern int lexical_error;
%}
%option yylineno
%%

"+" { /* ADD */ char_pos += strlen(yytext); return TOKEN_ADD; }
"-" { /* SUB */ char_pos += strlen(yytext); return TOKEN_SUB; }
"*" { /* MUL */ char_pos += strlen(yytext); return TOKEN_MUL; }
"/" { /* DIV */ char_pos += strlen(yytext); return TOKEN_DIV; }
"=" { /* ASSIGN */ char_pos += strlen(yytext); return TOKEN_ASSIGN; }
"(" { /* LP */ char_pos += strlen(yytext); return TOKEN_LP; }
")" { /* RP */ char_pos += strlen(yytext); return TOKEN_RP; }
"," { /* COMMA */ char_pos += strlen(yytext); return TOKEN_COMMA; }
"sub" { /*SUB STRING*/ char_pos += strlen(yytext); return TOKEN_SUB_STRING; }
[0-9]*[.][0-9]+   { /* REAL */ char_pos += strlen(yytext); return TOKEN_REAL; }
0|[1-9][0-9]*   { /* INTEGER */ char_pos += strlen(yytext); return TOKEN_INTEGER; }
[A-Za-z]+[A-Za-z0-9]* { /* IDENTIFIER */ char_pos += strlen(yytext); return TOKEN_ID; }
\$ast { printAST(ast); }
\$symbol { printSymbol(); }
\\\n ;
\n { return NEW_LINE; }
[ \t] { char_pos += strlen(yytext); };
<<EOF>> { return END_OF_FILE; }
\"([^"]|\\\")*\" { 
  /* STRING */ 
  int size = strlen(yytext);
  strncpy(yytext, yytext + 1, size - 2);
  yytext[size - 2] = NULL;
  char_pos += strlen(yytext);
  return TOKEN_STRING; 
  }
. { 
  char_pos += strlen(yytext); 
  if(lexical_error == TRUE){ return ERROR; };
  lexical_error = TRUE;
  printf("Lexical Error in line #%d(%d): cannot use %s.\n", yylineno, char_pos, yytext);
  return ERROR; 
 }

%%
int yywrap(void){
  return 1;
}