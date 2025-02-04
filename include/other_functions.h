#pragma once

#include <iconv.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../include/Queue.h"
#include "../include/String.h"

typedef struct {
    int database_size;
    int database_records_to_show;
    int count_of_frames;
    FILE *file_descriptor;
} Database_settings;

struct symbol {
    char value;
    std::string converted_value;
    size_t amount_reps;

    double probability;
    double cumulative_probability;

    size_t code_word_len;
    std::vector<bool> code_word;

    symbol() {
        amount_reps = 0;
        probability = 0;
        cumulative_probability = 0;
        code_word_len = 0;
    }

    void reset() {
        code_word.clear();
        code_word_len = 0;
    }
};

std::string convert_alphabet(std::string alphabet_utf8);
std::string convert(std::string text, std::string from_code,
                    std::string to_code);
std::string convert(Record record, std::string from_code, std::string to_code);
void record_converted_values(std::vector<symbol> &symbols);
std::vector<symbol> find_probabilities(std::string filename);
bool isRusLetter(symbol symbol);
void printLine(int size);
void showTable1(std::vector<symbol> symbols);
double craft_formula(std::vector<symbol> symbols, int i);
double entropy_formula(std::vector<symbol> symbols, int i);
double average_len_formula(std::vector<symbol> symbols, int i);
double find_symbols_sum(std::vector<symbol> symbols,
                        double (*function)(std::vector<symbol> symbols, int i));
void showTable2(double craft, double entropy, double average_len);
void gilbert_mur(std::vector<symbol> &symbols);
void task(std::vector<symbol> &symbols);

void initParam(Database_settings *param, int database_size,
               int database_records_to_show, int count_of_frames,
               FILE *file_descriptor);
void readFile(Queue *database, Node **frame_begin, Database_settings param);
int menuLoop(Queue database, Node **frames, Database_settings param);

void swap(symbol &a, symbol &b);
void sort(std::vector<symbol> &arr, int L, int R);
void digitalSort(Queue *S, byte *KDI);
Queue copyQueueAndSort(Queue database, byte *KDI);

void printData(Tdata data);
void printDData(Record data);
void showFrame(Node **frame, int size, int idx);
void showAsFrame(Node **frame, Database_settings param);
void showMenu();

int readKey(char *key);

void setattr(struct termios *attr, int show_cursor);
void clearBuf();
struct termios setNewTerminalSettings(struct termios *old);

void splitIntoFramesAndBuildIdxArray(Queue *sortedBase, Node **sortedBase_frame,
                                     Node **sortedBase_array,
                                     int *sortedBase_idx_array,
                                     Database_settings param);
int searchData(struct termios old, struct termios cur, Node **sortedBase_array,
               int *sortedBase_idx_array, Database_settings param,
               int *amount_keys, Queue *keys);
void EndingOfProgram(struct termios old, Queue *keys, Queue *sortedBase,
                     int amount_keys);

template <typename T>
class Tree {
   private:
    struct Vertex_ {
        Vertex_ *left;
        Vertex_ *right;

        T data;

        Vertex_ *next;

        Vertex_(T data) {
            this->data = data;
            left = nullptr;
            right = nullptr;
            next = nullptr;
        }
    };
    Vertex_ *root_;

    void add_implementation_(T data, Vertex_ *&p) {
        if (p == nullptr) {
            p = new Vertex_(data);
        } else {
            if (data < p->data) {
                add_implementation_(data, p->left);
            } else if (data > p->data) {
                add_implementation_(data, p->right);
            } else {
                add_implementation_(data, p->next);
            }
        }
    }

    void A2(std::vector<Record> arr, size_t L, size_t R) {
        if (L < R) {
            int weight = 0;
            int sum = 0;
            size_t i;

            for (i = L; i < R; ++i) weight += arr[i].house_number;

            for (i = L; i < R; ++i) {
                if (sum < weight / 2 && sum + arr[i].house_number > weight / 2)
                    break;
                sum += arr[i].house_number;
            }

            add_implementation_(arr[i], root_);
            A2(arr, L, i - 1);
            A2(arr, i + 1, R);
        }
    }

    void detourLTR_(Vertex_ *&p) {
        if (p != nullptr) {
            detourLTR_(p->left);
            std::cout << p->data << std::endl;
            detourLTR_(p->next);
            detourLTR_(p->right);
        }
    }

    void destroy_(Vertex_ *p) {
        if (p != nullptr) {
            destroy_(p->left);
            destroy_(p->right);
            destroy_(p->next);
            delete p;
        }
    }

    void del_next_field_(Vertex_ *&p) {
        Vertex_ **q = &p;
        q = &((*q)->next);
        while ((*q)->next != nullptr) q = &((*q)->next);
        delete (*q);
        (*q) = nullptr;
    }

    Vertex_ *find_implementation_(Vertex_ *p, int key) {
        while (p != nullptr) {
            if (key < p->data.house_number)
                p = p->left;
            else if (key > p->data.house_number)
                p = p->right;
            else
                return p;
        }

        return nullptr;
    }

   public:
    Tree() { root_ = nullptr; }
    ~Tree() { destroy_(root_); }

    bool isEmpty() { return root_ == nullptr ? true : false; }

    void addVertex(T data) { add_implementation_(data, root_); }

    void buildA2Tree(std::vector<Record> arr) { A2(arr, 0, arr.size() - 1); }

    void printData(Vertex_ *p) {
        if (p != nullptr) {
            std::cout << p->data << std::endl;
            printData(p->next);
        }
    }

    void findVertex(int key) {
        Vertex_ *p = find_implementation_(root_, key);
        if (p == nullptr) {
            std::cout << "Вершины нет в дереве";
            getchar();
        } else {
            std::cout << "По вашему запросу найдены следующие данные:"
                      << std::endl;
            printData(p);
            getchar();
        }
    }

    void del(T data) {
        Vertex_ **p;
        p = &root_;

        while (*p != nullptr) {
            if (data < (*p)->data)
                p = &((*p)->left);
            else if (data > (*p)->data)
                p = &((*p)->right);
            else
                break;
        }

        if ((*p)->next != nullptr) {
            del_next_field_(*p);
            return;
        }

        if (*p != nullptr) {
            Vertex_ *q;
            q = *p;
            if (q->left == nullptr) {
                *p = q->right;
            } else if (q->right == nullptr) {
                *p = q->left;
            } else {
                Vertex_ *S, *r;
                S = q;
                r = q->left;

                if (r->right == nullptr) {
                    r->right = q->right;
                    *p = r;
                } else {
                    while (r->right != nullptr) {
                        S = r;
                        r = r->right;
                    }

                    S->right = r->left;
                    r->right = q->right;
                    r->left = q->left;
                    *p = r;
                }
            }

            delete q;
        }
    }

    void display() {
        std::cout << "Содержимое дерева" << std::endl;
        detourLTR_(root_);
        std::cout << std::endl;
    }
};
