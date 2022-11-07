#include<stdio.h>
#include<string.h>
#include "parser.h"

extern int yylex();
extern int yylineno;
extern char* yytext;
typedef union Value{
    int integer;
    float real;
    char* string;
    char type;
}Value;
typedef struct TreeNode {
    char type;
    Value value;
}TreeNode;

char symbol_table[MAX_TABLE_SIZE][3][MAX_TOKEN_SIZE] = { NULL, };
char token[MAX_TOKEN_SIZE] = { NULL, };

int main(void){
    int token_type = 0;
    printf(">");
    while(TRUE){
        if((token_type = yylex()) == END_OF_FILE){
            break;
        }
        printf(">");
        if((token_type = yylex()) == END_OF_FILE){
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