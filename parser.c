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
        fflush(stdin);
        if(yytext[0] == '\n'){
            continue;
        }
        if(lookahead.type == END_OF_FILE){
            break;
        }
        ast = expr();
        printAST();
        if(ast == NULL){
            syntaxError();
            continue;
        }
        printEval();
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
    Node* re = restExpr(t);
    if(re == NULL){
        return t;
    }
    //re->left = t;
    //printf("[*] RE: 0x%X, %d\n", re, re->token.type);
    return re;
}

Node* restExpr(Node* _t){
    if(lookahead.type == TOKEN_ADD || lookahead.type == TOKEN_SUB){
        printf("%s: %s\n", "E\'", yytext);
        Node* op = createNode(lookahead);
        scanToken();
        Node* t = term();
        if(t == NULL){
            return NULL;
        }
        Node* re = restExpr(t);
        op->left = _t;
        op->right = t;
        if(re == NULL){
            return op;
        }
        re->left = op;
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
    //printf("[*] FACTOR: 0x%X, %d\n", f, f->token.type);
    Node* rt = restTerm(f);
    if(rt == NULL){
        return f;
    }
    //rt->left = f;
    //printf("[*] RT: 0x%X, %d\n", rt, rt->token.type);
    return rt;
}

Node* restTerm(Node* _f){
    if(lookahead.type == TOKEN_MUL || lookahead.type == TOKEN_DIV){
        printf("%s: %s\n", "T\'", yytext);
        Node* op = createNode(lookahead);
        scanToken();
        Node* f = factor();
        if(f == NULL){
            return NULL;
        }
        Node* rt = restTerm(f);
        op->left = _f;
        op->right = f;
        if(rt == NULL){
            return op;
        }
        rt->left = op;
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

/* 연산 후 결과를 문자열 형식으로 반환 */
void printEval(){
    Token result = evalRecursive(ast);
    printf("printEval: %d\n", result.type);
    switch(result.type){
        case TOKEN_INTEGER: printf("%d\n", result.value.integer); break;
        case TOKEN_REAL: printf("%lf\n", result.value.real); break;
        case TOKEN_STRING: printf("%s\n", result.value.string); break;
        default: runtimeError(); break;
    }
}
/* 연산을 위한 Recursive 함수 */
Token evalRecursive(Node* cur){
    Token result;
    result.type = ERROR;
    if(cur == NULL){
        return result;
    }
    Token val = cur->token;
    if(val.type == TOKEN_INTEGER || val.type == TOKEN_REAL || val.type == TOKEN_STRING){
        //printf("========evalRecursive========\n");
        //printf("[*] value: %d\n", val.value.integer);
        return val;
    }
    Token lval = evalRecursive(cur->left);
    Token rval = evalRecursive(cur->right);
    /*
    printf("=========evalRecursive=======\n");
    printf("[*] token: %d\n", val.type);
    printf("[*] lval: %d, rval: %d\n", lval.type, rval.type);
    */
    /* 연산 케이스 별로 나눠서 진행 */
    switch(val.type){
        case TOKEN_ADD: result = evalAdd(lval, rval); break;
        case TOKEN_SUB: result = evalSub(lval, rval); break;
        case TOKEN_MUL: result = evalMul(lval, rval); break;
        case TOKEN_DIV: result = evalDiv(lval, rval); break;
        case TOKEN_ASSIGN: result = evalAssign(lval, rval); break;
        case TOKEN_SUB_STRING: result = subString(val, lval, rval); break;
        default: break;
    }
    printf("[*] result: %d\n", result.value.integer);
    return result;
}
Token evalAdd(Token lval, Token rval){
    Token result;
    /*
    printf("======evalAdd======\n");
    printf("[*] lval: %d, rval: %d\n", lval.value.integer, rval.value.integer);
    printf("[*] ltype: %d, rtype: %d\n", lval.type, rval.type);
    */
    if(lval.type == TOKEN_INTEGER && rval.type == TOKEN_INTEGER){
        result.type = TOKEN_INTEGER;
        result.value.integer = lval.value.integer + rval.value.integer;
    }
    else if(lval.type == TOKEN_REAL && rval.type == TOKEN_REAL){
        result.type = TOKEN_REAL;
        result.value.real = lval.value.real + rval.value.real;
    }
    else if(lval.type == TOKEN_INTEGER && rval.type == TOKEN_REAL){
        result.type = TOKEN_REAL;
        result.value.real = lval.value.integer + rval.value.real;
    }
    else if(lval.type == TOKEN_REAL && rval.type == TOKEN_INTEGER){
        result.type = TOKEN_REAL;
        result.value.real = lval.value.real + rval.value.integer;
    }
    else{
        result.type = ERROR;
        //runtimeError();
        return result;
    }
    //printf("[*] result: %d\n", result.value.integer);
    return result;
}

Token evalSub(Token lval, Token rval){
    Token result;
    /*
    printf("======evalAdd======\n");
    printf("[*] lval: %d, rval: %d\n", lval.value.integer, rval.value.integer);
    printf("[*] ltype: %d, rtype: %d\n", lval.type, rval.type);
    */
    if(lval.type == TOKEN_INTEGER && rval.type == TOKEN_INTEGER){
        result.type = TOKEN_INTEGER;
        result.value.integer = lval.value.integer - rval.value.integer;
    }
    else if(lval.type == TOKEN_REAL && rval.type == TOKEN_REAL){
        result.type = TOKEN_REAL;
        result.value.real = lval.value.real - rval.value.real;
    }
    else if(lval.type == TOKEN_INTEGER && rval.type == TOKEN_REAL){
        result.type = TOKEN_REAL;
        result.value.real = lval.value.integer - rval.value.real;
    }
    else if(lval.type == TOKEN_REAL && rval.type == TOKEN_INTEGER){
        result.type = TOKEN_REAL;
        result.value.real = lval.value.real - rval.value.integer;
    }
    else{
        result.type = ERROR;
        //runtimeError();
        return result;
    }
    //printf("[*] result: %d\n", result.value.integer);
    return result;
}

Token evalMul(Token lval, Token rval){
    Token result;
    /*
    printf("======evalAdd======\n");
    printf("[*] lval: %d, rval: %d\n", lval.value.integer, rval.value.integer);
    printf("[*] ltype: %d, rtype: %d\n", lval.type, rval.type);
    */
    if(lval.type == TOKEN_INTEGER && rval.type == TOKEN_INTEGER){
        result.type = TOKEN_INTEGER;
        result.value.integer = lval.value.integer * rval.value.integer;
    }
    else if(lval.type == TOKEN_REAL && rval.type == TOKEN_REAL){
        result.type = TOKEN_REAL;
        result.value.real = lval.value.real * rval.value.real;
    }
    else if(lval.type == TOKEN_INTEGER && rval.type == TOKEN_REAL){
        result.type = TOKEN_REAL;
        result.value.real = lval.value.integer * rval.value.real;
    }
    else if(lval.type == TOKEN_REAL && rval.type == TOKEN_INTEGER){
        result.type = TOKEN_REAL;
        result.value.real = lval.value.real * rval.value.integer;
    }
    else{
        result.type = ERROR;
        //runtimeError();
        return result;
    }
    //printf("[*] result: %d\n", result.value.integer);
    return result;
}

Token evalDiv(Token lval, Token rval){
    Token result;
    /*
    printf("======evalAdd======\n");
    printf("[*] lval: %d, rval: %d\n", lval.value.integer, rval.value.integer);
    printf("[*] ltype: %d, rtype: %d\n", lval.type, rval.type);
    */
    if(lval.type == TOKEN_INTEGER && rval.type == TOKEN_INTEGER){
        result.type = TOKEN_INTEGER;
        result.value.integer = lval.value.integer / rval.value.integer;
    }
    else if(lval.type == TOKEN_REAL && rval.type == TOKEN_REAL){
        result.type = TOKEN_REAL;
        result.value.real = lval.value.real / rval.value.real;
    }
    else if(lval.type == TOKEN_INTEGER && rval.type == TOKEN_REAL){
        result.type = TOKEN_REAL;
        result.value.real = lval.value.integer / rval.value.real;
    }
    else if(lval.type == TOKEN_REAL && rval.type == TOKEN_INTEGER){
        result.type = TOKEN_REAL;
        result.value.real = lval.value.real / rval.value.integer;
    }
    else{
        result.type = ERROR;
        //runtimeError();
        return result;
    }
    //printf("[*] result: %d\n", result.value.integer);
    return result;
}

Token evalAssign(Token lval, Token rval){

}

Token subString(Token string, Token sp, Token ep){

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
void runtimeError(){
    printf("Runtime error in line %d \n", yylineno - 1);
}
void printAST(){
    if(ast == NULL){
        return;
    }
    Queue* head = initQueue();
    Node* cur = ast;
	enqueue(head, cur);
	while (isEmpty(head) == FALSE) {
        //printf("[*] head->next 0x%X \n", head->next);
        cur = dequeue(head);
        printf("[*] cur->token.type 0x%X \n", cur->token.type);
        //printf("[*] cur 0x%X \n", cur);
        Token token = cur->token;
        switch(token.type){
            case TOKEN_ADD: case TOKEN_SUB: case TOKEN_MUL: case TOKEN_DIV:
                printf("%c\t", token.value.operator); break;
            case TOKEN_ID:
                printf("%s\t", token.value.id); break;
            case TOKEN_STRING:
                printf("%s\t", token.value.string); break;
            case TOKEN_INTEGER:
                printf("%d\t", token.value.integer); break;
            case TOKEN_REAL:
                printf("%lf\t", token.value.real); break;
            default: break;
        }
		if (cur->left != NULL)	{
            enqueue(head, cur->left);
        }
		if (cur->right != NULL){
            enqueue(head, cur->right);
        }
	}
    printf("\n");
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
