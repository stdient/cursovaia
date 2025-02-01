#pragma once
#include <iostream>
#include <math.h>

typedef unsigned char byte;

typedef struct
{
    char citizen_full_name[32];
    char street_name[18];
    short int house_number;
    short int apartment_number;
    char date_of_move_in[10];
} Tdata;

typedef struct Record
{
    char citizen_full_name[32 * 2];
    char street_name[18 * 2];
    short int house_number;
    short int apartment_number;
    char date_of_move_in[10];

    friend std::ostream &operator<<(std::ostream &os, const Record &record)
    {
        os << record.citizen_full_name << '|';
        os << record.street_name << '|';
        os << record.house_number << '|';
        os << record.apartment_number;

        for (int i = 0; i < 2; ++i)
        {
            if (record.apartment_number < pow(10, i + 1))
                os << ' ';
        }

        os << '|';
        os << record.date_of_move_in;

        return os;
    }

    bool operator<(const Record &other)
    {
        return this->house_number < other.house_number;
    }

    bool operator>(const Record &other)
    {
        return this->house_number > other.house_number;
    }
} Record;

typedef struct Node
{
    union {
        Record data;
        byte digit[sizeof(Record)];
    };
    struct Node *next;
} Node;

typedef struct
{
    Node *head;
    Node *tail;
} Queue;

void init(Queue *database);
int isEmpty(Queue *q);
void enqueue(Queue *q, Record data);
Record dequeue(Queue *q);
void destroy(Queue *q);