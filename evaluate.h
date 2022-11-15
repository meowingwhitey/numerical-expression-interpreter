#pragma once
#include "parser.h"

Token evalRecursive(Node* cur);
Token evalAdd(Token lval, Token rval);
Token evalSub(Token lval, Token rval);
Token evalMul(Token lval, Token rval);
Token evalDiv(Token lval, Token rval);
Token evalAssign(Token lval, Token rval);
Token subString(Token string, Token sp, Token ep);

extern int yylex();
extern int yylineno;
extern char* yytext;

extern Token lookahead;
extern Symbol symbol_table[MAX_TABLE_SIZE];
extern int symbol_table_size;
extern Node* ast;
extern int error_detect;
extern char* error_token;

/* 연산을 위한 Recursive 함수 */
Token evalRecursive(Node* cur){
    Token result;
    result.type = ERROR;
    if(cur == NULL){
        return result;
    }
    Token val = cur->token;
    if(val.type == TOKEN_INTEGER || val.type == TOKEN_REAL || val.type == TOKEN_STRING){
        printf("========evalRecursive========\n");
        printf("[*] value: %d\n", val.value.integer);
        return val;
    }
    Token lval = evalRecursive(cur->left);
    Token mval = evalRecursive(cur->middle);
    Token rval = evalRecursive(cur->right);
    printf("=========evalRecursive=======\n");
    printf("[*] token: %d\n", val.type);
    printf("[*] lval: %d, rval: %d\n", lval.type, rval.type);
    
    /* 연산 케이스 별로 나눠서 진행 */
    switch(val.type){
        case TOKEN_ADD: result = evalAdd(lval, rval); break;
        case TOKEN_SUB: result = evalSub(lval, rval); break;
        case TOKEN_MUL: result = evalMul(lval, rval); break;
        case TOKEN_DIV: result = evalDiv(lval, rval); break;
        case TOKEN_ASSIGN: result = evalAssign(lval, rval); break;
        case TOKEN_ID: result = val; break;
        case TOKEN_SUB_STRING: 
            result = subString(lval, mval, rval); break;
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
    else if(lval.type == TOKEN_STRING && rval.type == TOKEN_STRING){
        result.type = TOKEN_STRING;
        char* concat = (char*)malloc(strlen(lval.value.string) + strlen(rval.value.string) + 1);
        strncpy(concat, lval.value.string, strlen(lval.value.string));
        strncpy(concat + strlen(lval.value.string), rval.value.string, strlen(rval.value.string));
        concat[strlen(concat) + 1] = NULL;
        result.value.string = concat;
    }
    else if(lval.type == TOKEN_STRING){
        result.type = TOKEN_STRING;
        char concat_buf[MAX_STRING_SIZE];
        if(rval.type == TOKEN_INTEGER){
            sprintf(concat_buf, "%s%d%c", lval.value.string, rval.value.integer, NULL);
        }
        else if(rval.type == TOKEN_REAL){
            sprintf(concat_buf, "%s%lf%c", lval.value.string, rval.value.real, NULL);
        }
        else if(rval.type == TOKEN_ID){
            int idx = checkIdx(rval.value.id);
            if(idx == ERROR){
                result.type = ERROR;
                printf("Runtime Error: variable %s is not defined.\n", rval.value.id);
                return result;
            }
            return evalAdd(lval, symbol_table[idx].token);
        }
        char* concat = (char*)malloc(strlen(concat_buf) + 1);
        strcpy(concat, concat_buf);
        result.value.string = concat;
    }
    else if(rval.type == TOKEN_STRING){
        result.type = TOKEN_STRING;
        char concat_buf[MAX_STRING_SIZE];
        if(lval.type == TOKEN_INTEGER){
            sprintf(concat_buf, "%d%s%c", lval.value.integer, rval.value.string, NULL);
        }
        else if(lval.type == TOKEN_REAL){
            sprintf(concat_buf, "%lf%s%c", lval.value.real, rval.value.string, NULL);
        }
        else if(lval.type == TOKEN_ID){
            int idx = checkIdx(lval.value.id);
            if(idx == ERROR){
                result.type = ERROR;
                printf("Runtime Error: variable %s is not defined.\n", lval.value.id);
                return result;
            }
            return evalAdd(symbol_table[idx].token, rval);
        }
        char* concat = (char*)malloc(strlen(concat_buf) + 1);
        strcpy(concat, concat_buf);
        result.value.string = concat;
    }
    else if(lval.type == TOKEN_ID && rval.type == TOKEN_ID){
        int lval_idx = checkIdx(lval.value.id);
        if(lval_idx == ERROR){
            result.type = ERROR;
            printf("Runtime Error: variable %s is not defined.\n", lval.value.id);
            return result;
        }
        int rval_idx = checkIdx(rval.value.id);
         if(rval_idx == ERROR){
            result.type = ERROR;
            printf("Runtime Error: variable %s is not defined.\n", rval.value.id);
            return result;
        }
        return evalAdd(symbol_table[lval_idx].token, symbol_table[rval_idx].token);
    }
    else if(lval.type == TOKEN_ID){
        int idx = checkIdx(lval.value.id);
        if(idx == ERROR){
            result.type = ERROR;
            printf("Runtime Error: variable %s is not defined.\n", lval.value.id);
            return result;
        }
        return evalAdd(symbol_table[idx].token, rval);
    }
    else if(rval.type == TOKEN_ID){
        int idx = checkIdx(rval.value.id);
        if(idx == ERROR){
            result.type = ERROR;
            printf("Runtime Error: variable %s is not defined.\n", rval.value.id);
            return result;
        }
        return evalAdd(lval, symbol_table[idx].token);
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
    else if(lval.type == TOKEN_ID){
        int idx = checkIdx(lval.value.id);
        if(idx == ERROR){
            result.type = ERROR;
            printf("Runtime Error: variable %s is not defined.\n", lval.value.id);
            return result;
        }
        return evalSub(symbol_table[idx].token, rval);
    }
    else if(rval.type == TOKEN_ID){
        int idx = checkIdx(rval.value.id);
        if(idx == ERROR){
            result.type = ERROR;
            printf("Runtime Error: variable %s is not defined.\n", rval.value.id);
            return result;
        }
        return evalSub(lval, symbol_table[idx].token);
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
    else if(lval.type == TOKEN_STRING && rval.type == TOKEN_INTEGER){
        int src_length = strlen(lval.value.string);
        int repeat = rval.value.integer;
        result.type = TOKEN_STRING;
        result.value.string = (char*)malloc(sizeof(lval.value.string) * repeat + 1);
        for(int i = 0; i < repeat; i ++){
            strncpy(result.value.string + i * src_length, lval.value.string, src_length);
        }
        result.value.string[src_length * repeat + 1] = NULL;
        return result;
    }
    else if(lval.type == TOKEN_ID){
        int idx = checkIdx(lval.value.id);
        if(idx == ERROR){
            result.type = ERROR;
            printf("Runtime Error: variable %s is not defined.\n", lval.value.id);
            return result;
        }
        return evalMul(symbol_table[idx].token, rval);
    }
    else if(rval.type == TOKEN_ID){
        int idx = checkIdx(rval.value.id);
        if(idx == ERROR){
            result.type = ERROR;
            printf("Runtime Error: variable %s is not defined.\n", rval.value.id);
            return result;
        }
        return evalMul(lval, symbol_table[idx].token);
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
    Token result;
    printf("======evalAssign======\n");
    printf("[*] lval: %s, rval: %d\n", lval.value.id, rval.value.integer);
    printf("[*] ltype: %d, rtype: %d\n", lval.type, rval.type);
    //좌변이 id가 아니면 Assign 자체가 불가능
    if(lval.type != TOKEN_ID){
        result.type = ERROR;
        return result;   
    }
    // 우변이 할당 가능한 값이어야됨
    if(rval.type == TOKEN_INTEGER || rval.type == TOKEN_REAL || rval.type == TOKEN_STRING){
        int idx = ERROR;
        idx = checkIdx(lval.value.id);
        /*
        switch(rval.type){
            case TOKEN_INTEGER: result.varType = INT; break;
            case TOKEN_REAL: result.varType = REAL; break;
            case TOKEN_STRING: result.varType = STRING; break;
            default: break;
        }
        */
        // 이미 이전에 선언한 변수
        if(idx != ERROR){
            symbol_table[idx].token = rval;
            return symbol_table[idx].token;
        }   
        // 새로 선언되는 변수
        else{
            idx = installID(lval.value.id, rval);
            return symbol_table[idx].token;
        }
    }
    else if(rval.type == TOKEN_ID){
        int r_idx = ERROR;
        r_idx = checkIdx(rval.value.id);
        if(r_idx == ERROR){
            result.type = ERROR; return result;
        }
        Token rval_token = symbol_table[r_idx].token;
        int l_idx = ERROR;
        l_idx = checkIdx(lval.value.id);

        // 이미 이전에 선언한 변수
        if(l_idx != ERROR){
            symbol_table[l_idx].token = rval_token;
            return symbol_table[l_idx].token;
        }   

        // 새로 선언되는 변수
        else{
            l_idx = installID(lval.value.id, rval_token);
            return symbol_table[l_idx].token;
        }        
    }
    else { result.type = ERROR; return result; }
}
// (sub(((("abc123"))),(3),(3)))
// sub("abc123",(3),(5))
// sub("abc123",3,5)
// (sub((((abc123))),(3),(5)))
Token subString(Token src, Token lval, Token rval){
    Token result;
    if(lval.type != TOKEN_INTEGER && rval.type != TOKEN_INTEGER){
        printf("aaa\n");
        result.type = ERROR; return result;
    }

    //String 대신 Token이 들어가는 경우
    if(src.type == TOKEN_ID && src.varType == STRING){
        int idx = checkIdx(src.value.id);
        if(idx == ERROR){
            printf("bbbb\n");
            result.type = ERROR; return result;
        }
        src.value.string = symbol_table[idx].token.value.string;
        src.type = TOKEN_STRING;
    }
    else if(src.type != TOKEN_STRING){ printf("[*] src.type = %d\n", src.type);result.type = ERROR; return result; }
    int sp = lval.value.integer; int size = rval.value.integer;
    if(sp > strlen(src.value.string) || strlen(src.value.string) - sp < size){
        printf("dddd\n");
        result.type = ERROR; return result;
    }
    printf("======subString======\n");
    printf("[*] string: %s\n", src.value.string);
    printf("[*] lval: %d, rval: %d\n", lval.value.integer, rval.value.integer);
    printf("[*] ltype: %d, rtype: %d\n", lval.type, rval.type);
    char* sub_str = (char*)malloc(size + 1);
    memcpy(sub_str, src.value.string + sp, size);
    sub_str[size + 1] = NULL;
    result.type = TOKEN_STRING;
    result.value.string = sub_str;
    result.varType = STRING;
    return result;
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