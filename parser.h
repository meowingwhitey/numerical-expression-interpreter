/* 
* E -> T E' => T + TE' | T - TE'
* E' -> + T E' | ε => E' -> + T + T E'
* T -> F T' => T -> F * F T'
* T' -> * F T' | ε => T' -> * F * F T'
* F -> Int | Real
*/
/*
    A -> id A' | F' T' E' 
    A' -> = A | T' E’
    E -> TE’// 아래 sub에서 사용해서 다시 추가 
    E' -> + T E‘ | - F T' | ε 
    T -> F T' 
    T' -> * F T' | / F T' | ε 
    F -> id | F'
    F' -> ( A ) | inum | fnum | S | - F | sub(S , E, E)
    S -> str
*/
#pragma once
#define MAX_TABLE_SIZE 100
#define MAX_TOKEN_SIZE 1000
#define MAX_LINE_LENGTH 1000

#define TRUE 1
#define FALSE 0
#define ERROR -1
#define BLANK 1
#define END_OF_FILE 0

typedef enum TokenType{
    TOKEN_ADD = 10, TOKEN_SUB, TOKEN_MUL, TOKEN_DIV,
    TOKEN_NEG, TOKEN_PLUS, TOKEN_ASSIGN, 
    TOKEN_LP, TOKEN_RP, TOKEN_COMMA, TOKEN_SUB_STRING, 
    TOKEN_ID, TOKEN_INTEGER, TOKEN_REAL, TOKEN_STRING, 
}TokenType;

typedef enum VariableType{
    INT = 0, REAL = TOKEN_REAL, STRING = TOKEN_STRING
}VariableType;

typedef union Value{
    char operator;
    int integer;
    double real;
    char* id;
    char* string;
}Value;

typedef struct Token{
    TokenType type;
    //변수 타입 다른 종류의 토큰에선 사용하지 않음
    VariableType varType;
    Value value;
}Token;

typedef struct TreeNode {
    Token token;
    struct TreeNode* left;
    struct TreeNode* right;
}Node;

typedef struct Symbol {
    char* name;
    Token token;
}Symbol;

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

Node* createNode();

void printEval();
Token evalRecursive(Node* cur);
Token evalAdd(Token lval, Token rval);
Token evalSub(Token lval, Token rval);
Token evalMul(Token lval, Token rval);
Token evalDiv(Token lval, Token rval);
Token evalAssign(Token lval, Token rval);
Token subString(Token string, Token sp, Token ep);

void scanToken();

void syntaxError();
void lexicalError();
void runtimeError();

void printAST();
void printSymbol();

/* Symbol 처리 관련 */
int installID(char* name, Token token);
int checkIdx(char* name);


/* malloc free 모음 */
void finalize();
void setLookahead(TokenType type, Value value);