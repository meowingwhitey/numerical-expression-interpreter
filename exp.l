%{
  #include "parser.h"
%}
%option yylineno
%%

0|[1-9][0-9]*   { 
  /* Integer */
  printf("Integer1 : %s\n", yytext); 
  return INTEGER; 
  }

[a-zA-Z]+[0-9]* { 
  //printf("Identifier : %s\n", yytext);
  return IDENTIFIER; 
  }

"$"ast          { 
  printAST(); 
  }

"$"symbol       { 
  printSymbol(); 
  }

[ \t\n]+        { 
  return BLANK; 
  }

<<EOF>>         { 
  return END_OF_FILE; 
  }

.               {
  printf("Syntax error in line %d, Expected an identifier but found %s\n", yylineno, yytext);
  return ERROR; 
  }

%%

int yywrap(void){
        return 1;
}