#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "parser.h"

extern int yylex();
extern int yylineno;
extern char* yytext;

Token lookahead;
Symbol* symbol_table[MAX_TABLE_SIZE];
int symbol_table_size = 0;

int main(void){
    Node* ast;
    while(TRUE){
        printf(">");
        scanToken();
        if(lookahead.type == END_OF_FILE){
            break;
        }
        ast = expr();
        if(ast == NULL){
            syntaxError();
        }
        Value result = eval(ast);
    } 
    finalize();
    return 0;
}

Node* expr(){
    printf("%s: %s\n", "E", yytext);
    Node* t = term();
    Node* re = restExpr();
    if(t == NULL){
        return NULL;
    }
    if(re == NULL){
        return t;
    }
    re->left = t;
    return re;
}

Node* restExpr(){
    if(lookahead.type == TOKEN_ADD || lookahead.type == TOKEN_SUB){
        printf("%s: %s\n", "E\'", yytext);
        Node* temp = createNode(lookahead);
        scanToken();
        Node* t = factor();
        if(t == NULL){
            return NULL;
        }
        temp->right = t;
        Node* re = restExpr();
        if(re == NULL){
            return temp;
        }
        re->left = temp;
        return re;
    }
    else return NULL;
}

Node* term(){
    printf("%s: %s\n", "T", yytext);
    Node* f = factor();
    if(f == NULL){
        return NULL;
    }
    Node* rt = restTerm();
    if(rt == NULL){
        return NULL;
    }
    rt->left = f;
    return rt;
}

Node* restTerm(){
    if(lookahead.type == TOKEN_MUL || lookahead.type == TOKEN_DIV){
        printf("%s: %s\n", "T\'", yytext);
        Node* temp = createNode(lookahead);
        scanToken();
        Node* f = factor();
        if(f == NULL){
            return NULL;
        }
        temp->right = f;
        Node* rt = restTerm();
        if(rt == NULL){
            return temp;
        }
        rt->left = temp;
        return rt;
    }
    else return NULL;
}

Node* factor(){
    if(lookahead.type == TOKEN_INTEGER || lookahead.type == TOKEN_REAL){
        printf("%s: %s\n", "F", yytext);
        Node* temp = createNode(lookahead);
        scanToken();
        return temp;
    }
    else return NULL;
}

Node* createNode(Token token){
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->token = token;
    temp->left = NULL; temp->right = NULL;
    return temp;
}

/* root 노드를 받고 eval 후 결과를 Value에 담아 반환 */
Value eval(Node* root){

}

void scanToken(){
    lookahead.type = yylex();
    switch(lookahead.type){
        case TOKEN_ADD: case TOKEN_SUB: case TOKEN_MUL: case TOKEN_DIV:
            lookahead.value.operator = yytext[0];
            break;
        case TOKEN_ID:
            lookahead.value.id = (char*)malloc(strlen(yytext) + 1);
            strcpy(lookahead.value.id, yytext);
            break;
        case TOKEN_STRING:
            lookahead.value.string = (char*)malloc(strlen(yytext) + 1);
            strcpy(lookahead.value.string, yytext);           
            break;
        case TOKEN_INTEGER:
            lookahead.value.integer = atoi(yytext);
            break;
        case TOKEN_REAL:
            lookahead.value.real = atof(yytext);
            break;
        default:
            break;
    }
}

void syntaxError(){
    printf("Syntax error in line %d, Unexpected token %s\n", yylineno, yytext);
}

void printAST(){
    printf("[*] printAST\n");
}

void printSymbol(){
    printf("[*] printSymbol\n");
}

int installID(char* name, Token token){
    int size = symbol_table_size;
    symbol_table[size] = (Symbol*)malloc(sizeof(Symbol));
    symbol_table[size]->name = name;
    symbol_table[size]->token = token;
    return ++symbol_table_size;
}

void finalize(){
    return;
}
