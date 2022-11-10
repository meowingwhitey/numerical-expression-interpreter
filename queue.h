#include "parser.h"

typedef struct Queue{
    Node* node;
    struct Queue* next;
}Queue;

Queue* initQueue(){
    Queue* head = (Queue*)malloc(sizeof(Queue));
    head->next = NULL;
    return head;
}

void enqueue(Queue* head, Node* node){
    Queue* cur = head;
    while(TRUE){
        if(cur->next == NULL){
            cur->next = (Queue*)malloc(sizeof(Queue));
            cur->next->node = node;
            break;
        }
        cur = cur->next;
    }
    return;
}

Node* dequeue(Queue* head){
    Queue* top = head->next;
    Node* node = top->node;
    head->next = top->next;
    //free(top);
    return node;
}

char isEmpty(Queue* head){
    if(head->next == NULL){
        return TRUE;
    }
    return FALSE;
}
