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
int error_detect;
char* error_token;

int main(void){
    while(TRUE){
        printf(">");
        scanToken();
        //printf("yytext: %s\n", yytext);
        if(yytext[0] == '\n'){
            continue;
        }
        if(lookahead.type == END_OF_FILE){
            break;
        }
        ast = all();
        if(error_detect == TRUE){
            error_detect = FALSE;
            syntaxError();
            continue;
        }
        printEval();
        
    } 
    finalize();
    return 0;
}
Node* all(){
    //printf("%s: %s\n", "A", yytext);
    /* id A' */
    if(lookahead.type == TOKEN_ID){
        Node* id = createNode(lookahead);
        Node* ra = restAll();
        ra->left = id;
        return ra;
    }
    /* F' T' E'  */
    else{
        Node* rf = restFactor();
        if(rf == NULL){
            error_detect = TRUE;
            return NULL;
        }
        Node* rt = restTerm();
        Node* re = restExpr();
        if(rt == NULL && re == NULL){
            return rf;
        }
        if(rt != NULL && re == NULL){    
            Node* temp = rt;
            while(temp->left != NULL){
                temp = temp->left;
            }
            temp->left = rf;
            return rt;
        }
        if(re != NULL && rt == NULL){    
            Node* temp = re;
            while(temp->left != NULL){
                temp = temp->left;
            }
            temp->left = rf;
            return re;
        }
        Node* temp1 = rt;
        while(temp1->left != NULL){
            temp1 = temp1->left;
        }
        temp1->left = rf;

        Node* temp2 = re;
        while(temp2->left != NULL){
            temp2 = temp2->left;
        }
        temp2->left = rt;
        return re;
    }
}
Node* restAll(){
    /* = A */
    if(lookahead.type == TOKEN_ASSIGN){
        Node* a = all();
        if(a == NULL){
            error_detect = TRUE;
            return NULL;
        }
        return a;
    }
    /* T' E’ */
    else{
        Node* tr = restTerm();
        if(tr == NULL){
            error_detect = TRUE;
            return NULL;
        }
        Node* re = restExpr();
        if(re == NULL){
            return tr;
        }
        Node* temp = re;
        while(temp->left != NULL){
            temp = temp->left;
        }
        temp->left = tr;
        return re;       
    }
}
Node* expr(){
    /* T E’ */
    //printf("%s: %s\n", "E", yytext);
    Node* t = term();
    if(t == NULL){
        error_detect = TRUE;
        return NULL;
    }
    Node* re = restExpr();
    if(re == NULL){
        return t;
    }
    Node* temp = re;
    while(temp->left != NULL){
        temp = temp->left;
    }
    temp->left = t;
    return re;
}

Node* restExpr(){
    /* + T E‘ | - F T' */
    if(lookahead.type == TOKEN_ADD || lookahead.type == TOKEN_SUB){
        //printf("%s: %s\n", "E\'", yytext);
        Node* op = createNode(lookahead);
        scanToken();
        Node* t = term();
        if(t == NULL){
            error_detect = TRUE;
            //detectError();
            return NULL;
        }
        Node* re = restExpr();
        op->right = t;
        if(re == NULL){
            return op;
        }
        Node* temp = re;
        while(temp->left != NULL){
            temp = temp->left;
        }
        temp->left = op;
        return re;
    }
    /* ε */
    else return NULL;
}

Node* term(){
    /* F T' */
    //printf("%s: %s\n", "T", yytext);
    Node* f = factor();
    if(f == NULL){
        error_detect = TRUE;
        return NULL;
    }
    //printf("[*] FACTOR: 0x%X, %d\n", f, f->token.type);
    Node* rt = restTerm(f);
    if(rt == NULL){
        return f;
    }
    Node* temp = rt;
    while(temp->left != NULL){
        temp = temp->left;
    }
    temp->left = f;    
    //printf("[*] RT: 0x%X, %d\n", rt, rt->token.type);
    return rt;
}

Node* restTerm(){
    /* * F T' | / F T' */
    if(lookahead.type == TOKEN_MUL || lookahead.type == TOKEN_DIV){
        //printf("%s: %s\n", "T\'", yytext);
        Node* op = createNode(lookahead);
        scanToken();
        Node* f = factor();
        if(f == NULL){
            error_detect = TRUE;
            return NULL;
        }
        Node* rt = restTerm();
        op->right = f;
        if(rt == NULL){
            return op;
        }
        Node* temp = rt;
        while(temp->left != NULL){
            temp = temp->left;
        }
        temp->left = op;
        return rt;
    }
    /* ε */
    else return NULL;
}

Node* factor(){
    /* id */
    if(lookahead.type == TOKEN_ID){
        //printf("%s: %s\n", "F", yytext);
        Node* num = createNode(lookahead);
        scanToken();
        return num;
    }
    /* F' */
    else{
        Node* rf = restFactor();
        if(rf == NULL){
            error_detect = TRUE; return NULL;
        }
        return rf;
    }
}

Node* restFactor(){
    /* ( A ) */
    if(lookahead.type == TOKEN_LP){
        Node* a = all();
        if(a == NULL){
            error_detect = TRUE; return NULL;
        }
        if(lookahead.type == TOKEN_RP){
            error_detect = TRUE; return NULL;
        }
        else return a;
    }
    /* inum | fnum */
    else if(lookahead.type == TOKEN_INTEGER || lookahead.type == TOKEN_REAL){
        //printf("%s: %s\n", "F", yytext);
        Node* num = createNode(lookahead);
        scanToken();
        return num;
    }
    /* S */
    else if(lookahead.type == TOKEN_STRING){
        return string();
    }
    /* - F */
    else if(lookahead.type == TOKEN_SUB){
        Node* sub = createNode(lookahead);
        scanToken();
        Node* f = factor();
        if(f == NULL){
            error_detect = TRUE;
            return NULL;
        }
        sub->right = f;
        return sub;
    }
    /* sub(S, E, E) */
    else if(lookahead.type == TOKEN_SUB_STRING){
    }
    else{ error_detect = TRUE; return NULL; }
}

Node* string(){
    if(lookahead.type == TOKEN_STRING){
        //printf("%s: %s\n", "S", yytext);
        Node* str = createNode(lookahead);
        scanToken();
        return str;
    }
    else{ error_detect = TRUE; return NULL; }
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
    //printf("printEval: %d\n", result.type);
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
    //printf("[*] result: %d\n", result.value.integer);
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
    switch(lookahead.type){
        case TOKEN_ADD: case TOKEN_SUB: case TOKEN_MUL: case TOKEN_DIV: 
        case TOKEN_ASSIGN: case TOKEN_LP: case TOKEN_RP: case TOKEN_SUB_STRING:
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
    printf("Syntax error in line %d, Unexpected token %s\n", yylineno, error_token);
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
                case TOKEN_ADD: case TOKEN_SUB: case TOKEN_MUL: case TOKEN_DIV:
                    if(cur->left != NULL){ child_num ++; }
                    if(cur->right != NULL){ child_num ++; }
                    printf("%c%d  ", token.value.operator, child_num); child_num = 0; break;
                case TOKEN_ID:
                    printf("%s  ", token.value.id); break;
                case TOKEN_STRING:
                    printf("%s  ", token.value.string); break;
                case TOKEN_INTEGER:
                    printf("%d  ", token.value.integer); break;
                case TOKEN_REAL:
                    printf("%lf  ", token.value.real); break;
                default: break;
            }
            if (cur->left != NULL)	{
                enqueue(head, cur->left); queueSize ++;
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
