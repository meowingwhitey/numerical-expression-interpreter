#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include "parser.h"
#include "grammer.h"
#include "evaluate.h"
#include "queue.h"

extern int yylex();
extern int yylineno;
extern char* yytext;

Token lookahead;
Symbol symbol_table[MAX_TABLE_SIZE];
int symbol_table_size = 0;
Node* ast;
int error_detect;
char* error_token;

int main(void){
    while(TRUE){
        printf(">");
        scanToken();
        if(yytext[0] == '\n'){
            continue;
        }
        if(lookahead.type == END_OF_FILE){
            break;
        }
        ast = all();
        if(error_detect == TRUE){
            error_detect = FALSE;
            free(error_token); error_token = NULL;
            syntaxError();
            continue;
        }
        printAST(ast);
        printEval();
    } 
    finalize();
    return 0;
}

Node* createNode(Token token){
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->token = token;
    temp->left = NULL; temp->right = NULL;
    temp->middle = NULL;
    return temp;
}
void printToken(Token token){
    int idx = -1;
    switch(token.type){
        case TOKEN_INTEGER: printf("%d\n", token.value.integer); break;
        case TOKEN_REAL: printf("%lf\n", token.value.real); break;
        case TOKEN_STRING: printf("\"%s\"\n", token.value.string); break;
        case TOKEN_ID: 
            // id가 table에 있음
            idx = checkIdx(token.value.string);
            if(idx != -1){
                printToken(symbol_table[idx].token);
            }
            // id가 table에 없음
            else{ printf("Runtime Error: undefined variable \"%s\"\n", token.value.string); }
        break;
        default: runtimeError(); break;
    }
    return;
}

void printEval(){
    Token result = evalRecursive(ast);
    //printf("printEval: %d\n", result.type);
    printToken(result);
    return;
}

void syntaxError(){
    printf("Syntax error in line %d, Unexpected token %s\n", yylineno, yytext);
}
void runtimeError(){
    printf("Runtime error in line %d \n", yylineno - 1);
}
void printAST(Node* ast){
    int queueSize = 0;
    if(ast == NULL){
        return;
    }
    Queue* head = initQueue();
    Node* cur = ast;
	enqueue(head, cur); queueSize ++;
	while (isEmpty(head) == FALSE) {
        int nodeCount = queueSize;
        while(nodeCount > 0){  
            int child_num = 0;
            cur = dequeue(head); queueSize --;
            Token token = cur->token;
            switch(token.type){
                case TOKEN_ADD: case TOKEN_SUB: case TOKEN_MUL: case TOKEN_DIV: case TOKEN_ASSIGN:
                    if(cur->left != NULL){ child_num ++; }
                    if(cur->middle != NULL){ child_num ++; }
                    if(cur->right != NULL){ child_num ++; }
                    printf("%c%d  ", token.value.operator, child_num); child_num = 0; break;
                case TOKEN_SUB_STRING: printf("%s  ", "sub"); break;
                case TOKEN_ID: printf("%s  ", token.value.id); break;
                case TOKEN_STRING: printf("\"%s\"  ", token.value.string); break;
                case TOKEN_INTEGER: printf("%d  ", token.value.integer); break;
                case TOKEN_REAL: printf("%lf  ", token.value.real); break;
                default: break;
            }
            if (cur->left != NULL)	{
                enqueue(head, cur->left); queueSize ++;
            }
            if (cur->middle != NULL)	{
                enqueue(head, cur->middle); queueSize ++;
            }
            if (cur->right != NULL){
                enqueue(head, cur->right); queueSize ++;
            }
            nodeCount--;
        }
        printf("\n");
	}
}

void printSymbol(){
    printf("%-5s %-5s %-5s\n", "NAME", "VALUE", "TYPE");
    for(int i = 0; i < symbol_table_size; i++){
        Symbol symbol = symbol_table[i];
        Token token = symbol.token;
        printf("%s ", symbol.name);
        switch(token.type){
            case TOKEN_INTEGER: printf("%d ", token.value.integer); break;
            case TOKEN_REAL: printf("%lf ", token.value.real); break;
            case TOKEN_STRING: printf("\"%s\" ", token.value.string); break;
            default: printf("Runtime Error: wrong variable assign.\n"); break;
        }
        printf("%s \n", TOKEN_TYPE_STRING(token.type));
    }
}

int installID(char* name, Token token){
    int size = symbol_table_size;
    printf("[*] Install Id: %s\n", name);
    // variable 길이 제한
    int id_length = strlen(name);
    if(id_length > 10){
        name[10] = NULL;
    }
    symbol_table[size].name = name;
    symbol_table[size].token = token;
    return symbol_table_size++;
}
const char* TOKEN_TYPE_STRING(TokenType type){
    switch(type){
        case TOKEN_INTEGER: return "INT";
        case TOKEN_REAL: return "REAL";
        case TOKEN_STRING: return "STRING";
        default: printf("Runtime Error: wrong variable assign.\n"); break;
    }
    return NULL;
}
int checkIdx(char* name){
    printf("checking...\n");
    int id_length = strlen(name);
    if(id_length > 10){
        name[10] = NULL;
    }
    for(int i = 0; i < symbol_table_size; i++){
        if(strcmp(symbol_table[i].name, name) == 0){
            return i;
        }
    }
    printf("check done!\n");
    return ERROR;
}

void finalize(){
    return;
}
