#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "parser.h"
#include "queue.h"

extern int yylex();
extern int yylineno;
extern char* yytext;

Token lookahead;
Symbol* symbol_table[MAX_TABLE_SIZE];
int symbol_table_size = 0;
Node* ast;

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
        ast = expr();
        printf("[*] AST: 0x%X, %d\n", ast, ast->token.type);
        printf("%d %d :%d %d\n",ast->left->token.type, ast->left->token.value.integer, ast->right->token.type, ast->right->token.value.integer);
        //printAST(ast, 0);
        if(ast == NULL){
            syntaxError();
        }
        //printf("%s\n", eval());
    } 
    finalize();
    return 0;
}

Node* expr(){
    printf("%s: %s\n", "E", yytext);
    Node* t = term();
    if(t == NULL){
        return NULL;
    }
    //printf("[*] TERM: 0x%X, %d\n", t, t->token.type);
    Node* re = restExpr();
    if(re == NULL){
        return t;
    }
    re->left = t;
    //printf("[*] RE: 0x%X, %d\n", re, re->token.type);
    return re;
}

Node* restExpr(){
    if(lookahead.type == TOKEN_ADD || lookahead.type == TOKEN_SUB){
        printf("%s: %s\n", "E\'", yytext);
        Node* op = createNode(lookahead);
        scanToken();
        Node* t = term();
        if(t == NULL){
            return NULL;
        }
        Node* re = restExpr();
        if(re == NULL){
            op->right = t;
            return op;
        }
        re->left = t;
        op->right = re;
        return op;
    }
    else return NULL;
}

Node* term(){
    printf("%s: %s\n", "T", yytext);
    Node* f = factor();
    if(f == NULL){
        return NULL;
    }
    //printf("[*] FACTOR: 0x%X, %d\n", f, f->token.type);
    Node* rt = restTerm();
    if(rt == NULL){
        return f;
    }
    rt->left = f;
    //printf("[*] RT: 0x%X, %d\n", rt, rt->token.type);
    return rt;
}

Node* restTerm(){
    if(lookahead.type == TOKEN_MUL || lookahead.type == TOKEN_DIV){
        printf("%s: %s\n", "T\'", yytext);
        Node* op = createNode(lookahead);
        scanToken();
        Node* f = factor();
        if(f == NULL){
            return NULL;
        }
        Node* rt = restTerm();
        if(rt == NULL){
            op->right = f;
            return op;
        }
        rt->left = f;
        op->right = rt;
        return op;
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

/* ast 노드를 받고 eval 후 결과를 문자열 형식으로 반환 */
char* eval(){

}

void scanToken(){
    lookahead.type = yylex();    
    //printf("[*]lexem: %s\n", yytext);
    //printf("[*]yylex\n");
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
    if(ast == NULL){
        return;
    }
    Queue* head = initQueue();
    Node* cur = ast;
	enqueue(head, cur);
	while (isEmpty(head) == FALSE) {
        printf("[*] head->next 0x%X \n", head->next);
        cur = dequeue(head);
        printf("[*] cur->token.type 0x%X \n", cur->token.type);
        printf("[*] cur 0x%X \n", cur);
		if (cur->left != NULL)	
            enqueue(head, cur->left);
		if (cur->right != NULL)	
            enqueue(head, cur->right);
	}
    /*
    switch(token.type){
        case TOKEN_ADD: case TOKEN_SUB: case TOKEN_MUL: case TOKEN_DIV:
            printf("%c\t", token.value.operator);
            break;
        case TOKEN_ID:
            printf("%s\t", token.value.id);
            break;
        case TOKEN_STRING:
            printf("%s\t", token.value.string);
            break;
        case TOKEN_INTEGER:
            printf("%d\t", token.value.integer);
            break;
        case TOKEN_REAL:
            printf("%lf\t", token.value.real);
            break;
        default:
            break;
    }
    */
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
