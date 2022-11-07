#define MAX_TABLE_SIZE 100
#define MAX_TOKEN_SIZE 1000

#define TRUE 1
#define FALSE 0

#define ERROR -1
#define TABLE -2
#define END_OF_FILE 0
#define BLANK 1
#define IDENTIFIER 2

typedef enum NodeType{
    ADD, SUB, MUL, DIV, NEG, ID, INTEGER
};
void printAST();
void printSymbol();