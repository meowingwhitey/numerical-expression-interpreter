#pragma once
#define MAX_TABLE_SIZE 100
#define MAX_TOKEN_SIZE 1000
#define MAX_LINE_LENGTH 1000
#define MAX_STRING_SIZE 100

#define TRUE 1
#define FALSE 0
#define ERROR -1
#define NEW_LINE 1
#define END_OF_FILE 0

typedef enum TokenType{
    TOKEN_ADD = 10, TOKEN_SUB, TOKEN_MUL, TOKEN_DIV,
    TOKEN_ASSIGN, TOKEN_LP, TOKEN_RP, TOKEN_COMMA, TOKEN_SUB_STRING, 
    TOKEN_ID, TOKEN_INTEGER, TOKEN_REAL, TOKEN_STRING, 
}TokenType;

typedef enum VariableType{
    INT = TOKEN_INTEGER, REAL = TOKEN_REAL, STRING = TOKEN_STRING
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
    //evaluate에 사용하는 변수 타입
    //다른 종류의 토큰에선 사용하지 않음
    VariableType varType;
    Value value;
}Token;

typedef struct TreeNode {
    Token token;
    struct TreeNode* left;
    // sub 연산용
    struct TreeNode* middle;
    struct TreeNode* right;
}Node;

typedef struct Symbol {
    char* name;
    Token token;
}Symbol;

Node* createNode();

void printEval();
void printToken(Token token);

void scanToken();

void syntaxError();
void lexicalError();
void runtimeError();
void typeError();

void printAST();
void printSymbol();

/* Symbol 처리 관련 */
int installID(char* name, Token token);
int checkIdx(char* name);
const char* TOKEN_TYPE_STRING(TokenType type);

/* malloc free 모음 */
void finalize();
void setLookahead(TokenType type, Value value);