#pragma once
#include "parser.h"
/*
    A -> id A' | F' T' E' 
    A' -> = A | T' E’
    E -> TE’// 아래 sub에서 사용해서 다시 추가 
    E' -> + T E‘ | - F T' | ε 
    T -> F T' 
    T' -> * F T' | / F T' | ε 
    F -> id | F'
    F' -> ( A ) | inum | fnum | S | - F | + F | sub(A, E, E)
    S -> str
*/
/* Grammers */
Node* all();
Node* restAll();
Node* expr();
Node* restExpr();
Node* term();
Node* restTerm();
Node* factor();
Node* restFactor();
Node* string();

extern int yylex();
extern int yylineno;
extern char* yytext;

extern Token lookahead;
extern Symbol symbol_table[MAX_TABLE_SIZE];
extern int symbol_table_size;
extern Node* ast;
extern int syntax_error;

Node* all(){
    if(lookahead.type == NEW_LINE || syntax_error == TRUE){ return NULL; }
    /* id A' */
    if(lookahead.type == TOKEN_ID){
        Node* id = createNode(lookahead);
        scanToken();
        Node* ra = restAll();
        if(ra == NULL){
            return id;
        }
        ra->left = id;
        return ra;
    }
    /* F' T' E'  */
    else{
        Node* rf = restFactor();
        if(rf == NULL){
            /* if(syntax_error == FALSE) { syntax_error = TRUE; syntaxError("factor"); } */
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
    if(lookahead.type == NEW_LINE || syntax_error == TRUE){ return NULL; }
    //printf("%s: %s\n", "A\'", yytext);
    /* = A */
    if(lookahead.type == TOKEN_ASSIGN){
        Node* op = createNode(lookahead);
        scanToken();
        Node* a = all();
        if(a == NULL){
            if(syntax_error == FALSE) { syntax_error = TRUE; syntaxError("="); }
            return NULL;
        }
        op->right = a;
        return op;
    }
    /* T' E’ */
    else{
        Node* tr = restTerm();
        Node* re = restExpr();
        if(tr == NULL && re == NULL){
            return NULL;
        }
        if(tr == NULL && re != NULL){
            return re;
        }
        if(tr != NULL && re == NULL){
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
    if(lookahead.type == NEW_LINE || syntax_error == TRUE){ return NULL; }
    //printf("%s: %s\n", "E", yytext);
    /* T E’ */
    Node* t = term();
    if(t == NULL){
        /* if(syntax_error == FALSE) { syntax_error = TRUE; syntaxError("term"); } */
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
    if(lookahead.type == NEW_LINE || syntax_error == TRUE){ return NULL; }
    //printf("%s: %s\n", "E\'", yytext);
    /* + T E' | -  T E' */
    if(lookahead.type == TOKEN_ADD || lookahead.type == TOKEN_SUB){
        TokenType op_type = lookahead.type;
        Node* op = createNode(lookahead);
        scanToken();
        Node* t = term();
        if(t == NULL){
            if(syntax_error == FALSE) { 
                syntax_error = TRUE; 
                if(op_type == TOKEN_ADD){ syntaxError("+"); }
                else{ syntaxError("-"); }
            }
            return NULL;
        }
        op->right = t;
        Node* re = restExpr();
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
    if(lookahead.type == NEW_LINE || syntax_error == TRUE){ return NULL; }
    /* F T' */
    //printf("%s: %s\n", "T", yytext);
    Node* f = factor();
    if(f == NULL){
        /* if(syntax_error == FALSE) { syntax_error = TRUE; syntaxError("factor"); } */
        return NULL;
    }
    //printf("[*] FACTOR: 0x%X, %d\n", f, f->token.type);
    Node* rt = restTerm();
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
    if(lookahead.type == NEW_LINE || syntax_error == TRUE){ return NULL; }
    //printf("%s: %s\n", "T\'", yytext);
    /* * F T' | / F T' */
    if(lookahead.type == TOKEN_MUL || lookahead.type == TOKEN_DIV){
        TokenType op_type = lookahead.type;
        Node* op = createNode(lookahead);
        scanToken();
        Node* f = factor();
        if(f == NULL){
            if(syntax_error == FALSE) { 
                syntax_error = TRUE; 
                if(op_type == TOKEN_MUL){ syntaxError("*"); }
                else{ syntaxError("/"); }
            }
            return NULL;
        }
        op->right = f;
        Node* rt = restTerm();
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
    if(lookahead.type == NEW_LINE || syntax_error == TRUE){ return NULL; }
    //printf("%s: %s\n", "F", yytext);
    /* id */
    if(lookahead.type == TOKEN_ID){
        Node* num = createNode(lookahead);
        scanToken();
        return num;
    }
    /* F' */
    else{
        Node* rf = restFactor();
        if(rf == NULL){
            /*if(syntax_error == FALSE) { syntax_error = TRUE; syntaxError("factor"); }*/
            return NULL;
        }
        return rf;
    }
}

Node* restFactor(){
    if(lookahead.type == NEW_LINE || syntax_error == TRUE){ return NULL; }
    //printf("%s: %s\n", "F\'", yytext);
    /* ( A ) */
    if(lookahead.type == TOKEN_LP){
        scanToken();
        Node* a = all();
        if(a == NULL || lookahead.type != TOKEN_RP){
            if(syntax_error == FALSE) { syntax_error = TRUE; syntaxError("()"); }
            return NULL;
        }
        else{
            scanToken(); return a;
        }
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
            if(syntax_error == FALSE) { syntax_error = TRUE; syntaxError("-"); }
            return NULL;
        }
        sub->right = f;
        return sub;
    }
    /* + F */
    else if(lookahead.type == TOKEN_ADD){
        Node* add = createNode(lookahead);
        scanToken();
        Node* f = factor();
        if(f == NULL){
            if(syntax_error == FALSE) { syntax_error = TRUE; syntaxError("+"); }
            return NULL;
        }
        add->right = f;
        return add;
    }
    /* sub(A, E, E) */
    else if(lookahead.type == TOKEN_SUB_STRING){
        //printf("[*]sub(A, E, E): %s\n", yytext);
        Node* sub_str = createNode(lookahead);
        scanToken();
        //lookahead가 "("가 아닌 경우 
        if(strcmp(yytext, "(") != 0 ){
            if(syntax_error == FALSE) { syntax_error = TRUE; syntaxError("sub(STRING, INT, INT)"); }
            return NULL;
        }
        scanToken();
        Node* a = all();
        //scanToken();
        if(a == NULL){
            if(syntax_error == FALSE) { syntax_error = TRUE; syntaxError("sub(STRING, INT, INT)"); }
            return NULL;
        }
        //printf("[*]sub(A, %d\n", a->token.type);
        sub_str->left = a;
        //lookahead가 ","가 아닌 경우 
        if(strcmp(yytext, ",") != 0 ){
            if(syntax_error == FALSE) { syntax_error = TRUE; syntaxError("sub(STRING, INT, INT)"); }
            return NULL;
        }
        scanToken();
        Node* expr1 = expr();
        if(expr1 == NULL){
            if(syntax_error == FALSE) { syntax_error = TRUE; syntaxError("sub(STRING, INT, INT)"); }
            return NULL;
        }
        //lookahead가 ","가 아닌 경우 
        if(strcmp(yytext, ",") != 0 ){
            if(syntax_error == FALSE) { syntax_error = TRUE; syntaxError("sub(STRING, INT, INT)"); }
            return NULL;
        }    
        scanToken(); 
        Node* expr2 = expr();
        if(expr2 == NULL){
            if(syntax_error == FALSE) { syntax_error = TRUE; syntaxError("sub(STRING, INT, INT)"); }
            return NULL;
        }
        //lookahead가 ")"가 아닌 경우 
        if(strcmp(yytext, ")") != 0 ){
            if(syntax_error == FALSE) { syntax_error = TRUE; syntaxError("sub(STRING, INT, INT)"); }
            return NULL;
        }
        scanToken();
        sub_str->middle = expr1; sub_str->right = expr2;
        return sub_str;
    }
    else{ /* syntax_error = TRUE; syntaxError("factor"); */ return NULL; }
}

Node* string(){
    if(lookahead.type == NEW_LINE || syntax_error == TRUE){ return NULL; }
    //printf("%s: %s\n", "S", yytext);
    if(lookahead.type == TOKEN_STRING){
        //printf("%s: %s\n", "S", yytext);
        Node* str = createNode(lookahead);
        scanToken();
        return str;
    }
    else{ 
        /* if(syntax_error == FALSE) { syntax_error = TRUE; syntaxError("STRING"); }*/
        return NULL;
    }
}