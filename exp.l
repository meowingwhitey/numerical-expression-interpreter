%{
  #include "parser.h"
%}

%%

[1-9][0-9]*  { /* Integer */ return INTEGER; }
"$"ast       { printAST(); }
"$"symbol    { printSymbol(); }
.            { return ERROR; }
<<EOF>>      { return END_OF_FILE; }
%%

int yywrap(void)
{
        return 1;
}