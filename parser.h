/* 
* E -> T E' => T + TE' | T - TE'
* E' -> + T E' | ε 
* T -> F T' => T -> F * F T'
* T' -> * F T' | ε => T' -> * F * F T'
* F -> Int | Real
*/

#define MAX_TABLE_SIZE 100
#define MAX_TOKEN_SIZE 1000

#define TRUE 1
#define FALSE 0
#define ERROR -1
#define END_OF_FILE 0

typedef enum TokenType{
    TOKEN_ADD = 10, TOKEN_SUB, TOKEN_MUL, TOKEN_DIV, TOKEN_NEG, TOKEN_ID, TOKEN_INTEGER, TOKEN_REAL, TOKEN_STRING
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
    struct TreeNode* right;
}Node;

typedef struct Symbol {
    char* name;
    Token token;
}Symbol;

Node* expr();
Node* restExpr();
Node* term();
Node* restTerm();
Node* factor();

Node* createNode();
Value eval(Node* root);

void scanToken();
void syntaxError();
void lexicalError();
void runtimeError();
void printAST();
void printSymbol();
int installID(char* name, Token token);
/* malloc free 모음 */
void finalize();
void setLookahead(TokenType type, Value value);