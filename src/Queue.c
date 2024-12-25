#include "../include/Queue.h"

#include <stdio.h>
#include <stdlib.h>

void init(Queue *database) {
    (*database).head = NULL;
    (*database).tail = (Node *)&(*database).head;
}

int isEmpty(Queue *q) {
    if ((*q).tail == (Node *)&(*q).head) return 1;
    return 0;
}

void enqueue(Queue *q, Record data) {
    Node *new_node = NULL;

    new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Требуемая память не выделена");
    }

    new_node->data = data;
    new_node->next = NULL;

    if (isEmpty((q))) {
        (*q).head = new_node;
        (*q).tail = new_node;
    } else {
        (*q).tail->next = new_node;
        (*q).tail = new_node;
    }
}

Record dequeue(Queue *q) {
    Record data = (*q).head->data;

    if (isEmpty(q)) {
        printf("Очередь пуста\n");
    } else {
        Node *p = (*q).head;
        (*q).head = p->next;
        free(p);
    }

    return data;
}

void destroy(Queue *q) {
    while ((*q).head != NULL) {
        dequeue(q);
    }
}
