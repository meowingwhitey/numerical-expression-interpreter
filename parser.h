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

typedef union Value{
    char operator;
    int integer;
    double real;
    char* id;
    char* string;
}Value;

typedef struct Token{
    TokenType type;
    Value value;
}Token;

typedef struct TreeNode {
    Token token;
    struct TreeNode* left;
    struct TreeNode* middle;
    struct TreeNode* right;
}Node;

typedef struct Symbol {
    char* name;
    Token token;
}Symbol;

Node* createNode();

void printEval();

void scanToken();

void syntaxError();
void lexicalError();
void runtimeError();

void printAST();
void printSymbol();

/* Symbol 처리 관련 */
int installID(char* name, Token token);
int checkIdx(char* name);
const char* TOKEN_TYPE_STRING(TokenType type);

/* malloc free 모음 */
void finalize();