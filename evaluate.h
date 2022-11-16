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
extern char error_str[MAX_LINE_LENGTH];
extern int runtime_error;

extern Token lookahead;
extern Symbol symbol_table[MAX_TABLE_SIZE];
extern int symbol_table_size;
extern Node* ast;

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
    Token mval = evalRecursive(cur->middle);
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
        case TOKEN_ID: result = val; break;
        case TOKEN_SUB_STRING: 
            result = subString(lval, mval, rval); break;
        default: if(runtime_error == FALSE){ runtimeError(); printf("undefined operator.\n");} break;
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
    if(lval.type == ERROR && (rval.type == TOKEN_INTEGER || rval.type == TOKEN_REAL)){
        result.type = rval.type;
        if(rval.type == TOKEN_INTEGER){ result.value.integer = rval.value.integer; }
        else if(rval.type == TOKEN_REAL){ result.value.real = rval.value.real; }
    }
    else if(lval.type == TOKEN_INTEGER && rval.type == TOKEN_INTEGER){
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
                runtimeError(); if(runtime_error == FALSE){ printf("undefined variable %s.\n", rval.value.id); }
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
                if(runtime_error == FALSE){ runtimeError(); printf("undefined variable %s.\n", lval.value.id); }
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
            if(runtime_error == FALSE){ runtimeError(); printf("undefined variable %s.\n", lval.value.id); }
            return result;
        }
        int rval_idx = checkIdx(rval.value.id);
         if(rval_idx == ERROR){
            result.type = ERROR;
            if(runtime_error == FALSE){ runtimeError(); printf("undefined variable %s.\n", rval.value.id); }
            return result;
        }
        return evalAdd(symbol_table[lval_idx].token, symbol_table[rval_idx].token);
    }
    else if(lval.type == TOKEN_ID){
        int idx = checkIdx(lval.value.id);
        if(idx == ERROR){
            result.type = ERROR;
            if(runtime_error == FALSE){ runtimeError(); printf("undefined variable %s.\n", lval.value.id); }
            return result;
        }
        return evalAdd(symbol_table[idx].token, rval);
    }
    else if(rval.type == TOKEN_ID){
        int idx = checkIdx(rval.value.id);
        if(idx == ERROR){
            result.type = ERROR;
            if(runtime_error == FALSE){ runtimeError(); printf("undefined variable %s.\n", rval.value.id); }
            return result;
        }
        return evalAdd(lval, symbol_table[idx].token);
    }
    else{
        result.type = ERROR;
        if(runtime_error == FALSE){ runtimeError(); printf("%s + %s is not defined.\n", TOKEN_TYPE_STRING(lval.type), TOKEN_TYPE_STRING(rval.type)); }
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
    if(lval.type == ERROR && (rval.type == TOKEN_INTEGER || rval.type == TOKEN_REAL)){
        result.type = rval.type;
        if(rval.type == TOKEN_INTEGER){ result.value.integer = -1 * rval.value.integer; }
        else if(rval.type == TOKEN_REAL){ result.value.real = -1 * rval.value.real; }
    }
    else if(lval.type == TOKEN_INTEGER && rval.type == TOKEN_INTEGER){
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
            if(runtime_error == FALSE){ runtimeError(); printf("undefined variable %s.\n", lval.value.id); }
            return result;
        }
        return evalSub(symbol_table[idx].token, rval);
    }
    else if(rval.type == TOKEN_ID){
        int idx = checkIdx(rval.value.id);
        if(idx == ERROR){
            result.type = ERROR;
            if(runtime_error == FALSE){ runtimeError(); printf("undefined variable %s.\n", rval.value.id); }
            return result;
        }
        return evalSub(lval, symbol_table[idx].token);
    }
    else{
        result.type = ERROR;
        if(runtime_error == FALSE){ runtimeError(); printf("%s - %s is not defined.\n", TOKEN_TYPE_STRING(lval.type), TOKEN_TYPE_STRING(rval.type)); }
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
            if(runtime_error == FALSE){ runtimeError(); printf("undefined variable %s.\n", lval.value.id); }
            return result;
        }
        return evalMul(symbol_table[idx].token, rval);
    }
    else if(rval.type == TOKEN_ID){
        int idx = checkIdx(rval.value.id);
        if(idx == ERROR){
            result.type = ERROR;
            if(runtime_error == FALSE){ runtimeError(); printf("undefined variable %s.\n", rval.value.id); }
            return result;
        }
        return evalMul(lval, symbol_table[idx].token);
    }
    else{
        result.type = ERROR;
        if(runtime_error == FALSE){ runtimeError(); printf("%s * %s is not defined.\n", TOKEN_TYPE_STRING(lval.type), TOKEN_TYPE_STRING(rval.type)); }
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
    else if(lval.type == TOKEN_STRING && rval.type == TOKEN_STRING){
        char* l_str = lval.value.string; char* r_str = rval.value.string;
        int length = strlen(r_str); int repeat = 0;
        while(TRUE){
            if(length * repeat > strlen(l_str) - 1){ break; }
            if(strncmp(l_str + (length * repeat), r_str, length) == 0){
                repeat ++; continue;
            }
            break;
        }
        result.type = TOKEN_INTEGER;
        result.value.integer = repeat;
        return result;
    }
    else if(lval.type == TOKEN_ID){
        int idx = checkIdx(lval.value.id);
        if(idx == ERROR){
            result.type = ERROR;
            if(runtime_error == FALSE){ printf("Runtime Error: undefined variable %s.\n", lval.value.id); }
            return result;
        }
        return evalDiv(symbol_table[idx].token, rval);
    }
    else if(rval.type == TOKEN_ID){
        int idx = checkIdx(rval.value.id);
        if(idx == ERROR){
            result.type = ERROR;
            if(runtime_error == FALSE){ printf("Runtime Error: undefined variable %s.\n", rval.value.id); }
            return result;
        }
        return evalDiv(lval, symbol_table[idx].token);
    }
    else{
        result.type = ERROR;
        if(runtime_error == FALSE){ runtimeError(); printf("%s / %s is not defined.\n", TOKEN_TYPE_STRING(lval.type), TOKEN_TYPE_STRING(rval.type)); }
        return result;
    }
    //printf("[*] result: %d\n", result.value.integer);
    return result;
}

Token evalAssign(Token lval, Token rval){
    Token result;
    /*
    printf("======evalAssign======\n");
    printf("[*] lval: %s, rval: %d\n", lval.value.id, rval.value.integer);
    printf("[*] ltype: %d, rtype: %d\n", lval.type, rval.type);
    */
    //좌변이 id가 아니면 Assign 자체가 불가능
    if(lval.type != TOKEN_ID){
        result.type = ERROR;
        if(runtime_error == FALSE){ runtimeError(); printf("%s = %s is not defined.\n", TOKEN_TYPE_STRING(lval.type), TOKEN_TYPE_STRING(rval.type)); }
        return result;   
    }
    // 우변이 할당 가능한 값이어야됨
    if(rval.type == TOKEN_INTEGER || rval.type == TOKEN_REAL || rval.type == TOKEN_STRING){
        int idx = ERROR;
        idx = checkIdx(lval.value.id);
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
            result.type = ERROR; 
            if(runtime_error == FALSE){ runtimeError(); printf("undefined variable %s.\n", rval.value.id); }
            return result;
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
    else { 
        result.type = ERROR; 
        if(runtime_error == FALSE){ runtimeError(); printf("%s = %s is not defined.\n", TOKEN_TYPE_STRING(lval.type), TOKEN_TYPE_STRING(rval.type)); }
        return result; 
    }
}
Token subString(Token src, Token lval, Token rval){
    Token result;
    if(lval.type != TOKEN_INTEGER && rval.type != TOKEN_INTEGER){
        result.type = ERROR; 
        if(runtime_error == FALSE){ runtimeError(); printf("start index and slice length must have integer value.\n"); }
        return result;
    }

    //String 대신 Variable이 들어가는 경우
    if(src.type == TOKEN_ID){
        int idx = checkIdx(src.value.id);
        if(idx == ERROR){
            result.type = ERROR; runtimeError(); printf("undefined variable %s.\n", src.value.id); return result;
        }
        if(symbol_table[idx].token.type != TOKEN_STRING){
            result.type = ERROR; 
            if(runtime_error == FALSE){ 
                runtimeError(); 
                printf("variable %s is %s not a string type.\n", src.value.id, TOKEN_TYPE_STRING(src.type)); 
            }
            return result; 
        }
        src.value.string = symbol_table[idx].token.value.string;
        src.type = TOKEN_STRING;
        subString(src, lval, rval);
    }
    if(src.type != TOKEN_STRING){ 
        result.type = ERROR; 
        if(runtime_error == FALSE){ 
            runtimeError(); 
            printf("variable %s is %s not a string type.\n", src.value.id, TOKEN_TYPE_STRING(src.type)); 
        } 
        return result; 
    }
    int sp = lval.value.integer; int size = rval.value.integer;
    if(sp > strlen(src.value.string)){
        result.type = ERROR; 
        if(runtime_error == FALSE){ 
            runtimeError(); 
            printf("variable %s is %s not a string type.\n", src.value.id, TOKEN_TYPE_STRING(src.type)); 
        } 
        return result;
    }
    if(strlen(src.value.string) - sp < size){
        result.type = ERROR; 
        if(runtime_error == FALSE){ 
            runtimeError(); 
            printf("slice length is bigger than string length.\n"); 
        } 
        
        return result;
    }
    /*
    printf("======subString======\n");
    printf("[*] string: %s\n", src.value.string);
    printf("[*] lval: %d, rval: %d\n", lval.value.integer, rval.value.integer);
    printf("[*] ltype: %d, rtype: %d\n", lval.type, rval.type);
    */
    char* sub_str = (char*)malloc(size + 1);
    memcpy(sub_str, src.value.string + sp, size);
    sub_str[size + 1] = NULL;
    result.type = TOKEN_STRING;
    result.value.string = sub_str;
    return result;
}
