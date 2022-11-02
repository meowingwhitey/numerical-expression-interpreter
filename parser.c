#include<stdio.h>
#include<string.h>
#include "parser.h"

extern int yylex();
extern int yylineno;
extern char* yytext;

char symbol_table[MAX_TABLE_SIZE][3][MAX_TOKEN_SIZE] = { NULL, };
char token[MAX_TOKEN_SIZE] = { NULL, };

int main(void){
    int token_type = 0;
    while(TRUE){
        printf("> ");
        token_type = yylex();
        switch(token_type){
            case INTEGER:
                printf("token : %s %d", yytext, token_type);
                break;
            case ERROR:
                printf("Syntax error in line %d, Expected an identifier but found %s\n", yylineno, yytext);
                break;
            case END_OF_FILE:
                return 0;
            default:
                break;
        }
    }
    return 0;
}

void printAST(){
    printf("[*] printAST\n");
}
void printSymbol(){
    printf("[*] printSymbol\n");
}