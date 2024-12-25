#pragma once

typedef unsigned char byte;

typedef struct {
    char citizen_full_name[32];
    char street_name[18];
    short int house_number;
    short int apartment_number;
    char date_of_move_in[10];
} Tdata;

typedef struct {
    char citizen_full_name[32 * 2];
    char street_name[18 * 2];
    short int house_number;
    short int apartment_number;
    char date_of_move_in[10];
} Record;

typedef struct Node {
    union {
        Record data;
        byte digit[sizeof(Record)];
    };
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
} Queue;

void init(Queue *database);
int isEmpty(Queue *q);
void enqueue(Queue *q, Record data);
Record dequeue(Queue *q);
void destroy(Queue *q);