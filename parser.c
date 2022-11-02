#include<stdio.h>
#include "parser.h"

extern int yylex();
extern int yylineno;
extern char* yytext;

char symbol_table[MAX_TABLE_SIZE][3][MAX_TOKEN_SIZE] = { NULL, };
char token[MAX_TOKEN_SIZE] = { NULL, };

int main(void){
    int token_type = yylex();
    while(token_type){
        printf("token : %s %d", yytext, token_type);
        token_type = yylex();
    }
    return 0;
}

void printAST(){
    printf("[*] printAST\n");
}
void printSymbol(){
    printf("[*] printSymbol\n");
}