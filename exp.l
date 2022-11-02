%{
  #include "parser.h"
%}

%%

[1-9][0-9]*  { /* Integer */ return INTEGER; }
"$"ast       { printAST(); }
"$"symbol    { printSymbol(); }
%%

int yywrap(void)
{
        return 1;
}