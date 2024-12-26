#include <iconv.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "../include/Queue.h"
#include "../include/String.h"

typedef struct {
  int database_size;
  int database_records_to_show;
  int count_of_frames;
  FILE *file_descriptor;
} Database_settings;

typedef struct Vertex {
  Record data;
  struct Vertex *left;
  struct Vertex *right;
  int weight;

  Node *next;
} vertex;

void digitalSort(Queue *S, byte *KDI);

void printData(Tdata data);
void printDData(Record data);
void showFrame(Node **frame, int size, int idx);
void showAsFrame(Node **frame, Database_settings param);

void showMenu();
int menuLoop(Queue database, Node **frames, Database_settings param);

int readKey(char *key);
void readFile(Queue *database, Node **frame_begin, Database_settings param);
void initParam(Database_settings *param, int database_size,
               int database_records_to_show, int count_of_frames,
               FILE *file_descriptor);

void setattr(struct termios *attr, int show_cursor);

void detourLTR(vertex *p) {
  if (p != NULL) {
    detourLTR(p->left);
    printDData(p->data);
    detourLTR(p->right);
  }
}

void freemem(vertex *root) {
  if (root != NULL) {
    freemem(root->left);
    freemem(root->right);
    free(root);
  }
}

int main() {
  char *filename = "testBase4.dat";
  FILE *file = fopen(filename, "rb");

  if (!file) {
    perror(filename);
    return EXIT_FAILURE;
  }

  Queue database;
  init(&database);
  Database_settings param;
  int database_size = 4000;
  int records_to_show = 20;
  initParam(&param, database_size, records_to_show,
            database_size / records_to_show - 1, file);

  Node *frame_begin[param.count_of_frames];
  readFile(&database, frame_begin, param);

  menuLoop(database, frame_begin, param);

  destroy(&database);
  fclose(file);
  return EXIT_SUCCESS;
}

char *findYear(char *date, char *sep) {
  char *token = mstrtok(date, sep);
  for (int i = 0; i < 2; ++i) {
    token = mstrtok(NULL, sep);
  }

  return token;
}

int binarySearch(Node **arr, int *idx_arr, int key, int n) {
  int L = 0,  //
      R = n - 1;

  int year;
  char bufs[10];

  int m;
  while (L < R) {
    m = floor((L + R) / 2);

    mstrcpy(bufs, arr[idx_arr[m]]->data.date_of_move_in);
    year = convertStrToInt(findYear(bufs, "- "));

    if (year < key)
      L = m + 1;
    else
      R = m;
  }

  mstrcpy(bufs, arr[idx_arr[R]]->data.date_of_move_in);
  year = convertStrToInt(findYear(bufs, "- "));

  if (year == key) return idx_arr[R];
  return -1;
}

vertex *findVertex(vertex *p, int to_found) {
  while (p != NULL) {
    if (p->data.house_number > to_found) {
      p = p->left;
    } else if (p->data.house_number < to_found) {
      p = p->right;
    } else {
      p->weight++;
      break;
    }
  }

  return p;
}

vertex *newNode(Record data) {
  vertex *p = (vertex *)malloc(sizeof(vertex));
  if (p == NULL) {
    fprintf(stderr, "Требуемая память не была выделена\n");
  }
  p->data = data;
  p->weight = 0;
  p->left = NULL;
  p->right = NULL;
  p->next = NULL;

  return p;
}

vertex *addNode(vertex *root, Node *node_p) {
  vertex **p = &root;

  while (*p != NULL) {
    if (node_p->data.house_number < (*p)->data.house_number)
      p = &((*p)->left);
    else if (node_p->data.house_number > (*p)->data.house_number)
      p = &((*p)->right);
    else {
      break;
    }
  }
  if (*p == NULL) {
    *p = newNode(node_p->data);
  }

  return root;
}

vertex *A2(vertex *root, int L, int R) {
  vertex **p = &root;

  int wes = 0, sum = 0;
  int i = 0;

  if (L <= R) {
    for (int i = L; i < R; ++i) {
      wes += (*p)->weight;
    }
    for (i = L; i < R; ++i) {
      if (sum < wes / 2 && sum + (*p)->weight > wes / 2) break;
      sum = sum + (*p)->weight;
    }
  }

  root = addNode(root, (*p)->next);
  root = A2(root, L, i + 1);
  root = A2(root, i + 1, R);

  return root;
}

void clearBuf() {
  char ch;
  while ((ch = getchar()) != '\n' && ch != EOF);
}

int menuLoop(Queue database, Node **database_frames, Database_settings param) {
  struct termios cur, old;
  tcgetattr(STDIN_FILENO, &cur);
  old = cur;
  cur.c_lflag &= ~(ISIG | ICANON | ECHO);
  cur.c_cc[VMIN] = 1;
  cur.c_cc[VTIME] = 0;
  setattr(&cur, 0);

  Queue sortedBase;
  init(&sortedBase);
  byte KDI[sizeof(Record)] = {
      113, 112, 111, 110, 109, 108, 107, 106, 105, 104, 64, 65, 66, 67, 68,
      69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79, 80, 81, 82, 83,
  };

  for (Node *p = database.head; p != NULL; p = p->next) {
    enqueue(&sortedBase, p->data);
  }
  digitalSort(&sortedBase, KDI);

  Node *sortedBase_frame[param.count_of_frames];
  Node *p = sortedBase.head;
  int frame_idx = 0;

  Node *sortedBase_array[param.database_size];
  int sortedBase_idx_array[param.database_size];

  for (int i = 0; i < param.database_size; ++i) {
    sortedBase_idx_array[i] = i;
    sortedBase_array[i] = p;

    if (i % param.database_records_to_show == 0)
      sortedBase_frame[frame_idx++] = p;

    p = p->next;
  }

  char action;

  int key_size = 3;
  char key_str[key_size];
  for (int i = 0; i < key_size; ++i) {
    key_str[i] = '\0';
  }
  int first_key_entry = 0;
  char year_str[10];

  Queue keys;
  int amount_keys = 0;

  vertex *root = NULL;
  vertex *found = NULL;
  int num_to_found = 0;

  while (1) {
    showMenu();
    read(STDIN_FILENO, &action, 1);

    switch (action) {
      case '1':
        system("clear");
        showFrame(&database.head, param.database_size, 0);
        getchar();
        break;

      case '2':
        showAsFrame(database_frames, param);
        break;

      case '3':
        showAsFrame(sortedBase_frame, param);
        break;

      case '4':
        system("clear");

        setattr(&old, 1);
        while (readKey(key_str));
        setattr(&cur, 0);

        first_key_entry =
            binarySearch(sortedBase_array, sortedBase_idx_array,
                         convertStrToInt(key_str), param.database_size);

        if (first_key_entry == -1) {
          fprintf(stderr, "Элемент не найден\n");
          getchar();
          break;
        }

        if (amount_keys) destroy(&keys);
        init(&keys);
        amount_keys = 0;

        mstrcpy(year_str,
                sortedBase_array[sortedBase_idx_array[first_key_entry]]
                    ->data.date_of_move_in);

        while (convertStrToInt(findYear(year_str, "- ")) ==
               convertStrToInt(key_str)) {
          enqueue(&keys, sortedBase_array[sortedBase_idx_array[first_key_entry +
                                                               amount_keys]]
                             ->data);
          amount_keys++;

          if (first_key_entry + amount_keys == param.database_size) break;

          mstrcpy(year_str,
                  sortedBase_array[sortedBase_idx_array[first_key_entry +
                                                        amount_keys]]
                      ->data.date_of_move_in);
        }

        showFrame(&keys.head, amount_keys, 0);

        getchar();
        break;

      case '5':
        system("clear");

        if (!amount_keys) {
          printf("Сначала нужно выполнить поиск по ключу\n");
          getchar();
          break;
        }

        for (int i = 0; i < (int)sizeof(Record); ++i) {
          KDI[i] = 0;
        }
        KDI[0] = (byte)(sizeof(keys.head->data.citizen_full_name) +
                        sizeof(keys.head->data.street_name));

        digitalSort(&keys, KDI);

        for (p = keys.head; p != NULL; p = p->next) {
          root = addNode(root, p);
        }
        detourLTR(root);

        setattr(&old, 1);
        scanf("%d", &num_to_found);
        setattr(&cur, 0);
        clearBuf();

        found = findVertex(root, num_to_found);
        if (found == NULL) {
          printf("Такой вершины нет в дереве\n");
          getchar();
          break;
        }
        printDData(found->data);

        root = A2(root, 0, amount_keys - 1);

        getchar();
        break;

      case '0':
        setattr(&old, 1);
        system("clear");
        if (amount_keys) destroy(&keys);
        destroy(&sortedBase);
        return EXIT_SUCCESS;
        break;
    }
  }
}

int readKey(char *key) {
  system("clear");

  for (int i = 0; i < 2; ++i) {
    key[i] = getchar();
    if (key[i] == 'q') {
      clearBuf();
      return EXIT_SUCCESS;
    }
  }
  clearBuf();

  return EXIT_SUCCESS;
}

void setattr(struct termios *attr, int show_cursor) {
  tcsetattr(STDIN_FILENO, TCSANOW, attr);

  if (show_cursor)
    printf("\e[?25h");
  else
    printf("\e[?25l");
}

void initParam(Database_settings *param, int database_size,
               int database_records_to_show, int count_of_frames,
               FILE *file_descriptor) {
  (*param).database_size = database_size;
  (*param).database_records_to_show = database_records_to_show;
  (*param).count_of_frames = count_of_frames;
  (*param).file_descriptor = file_descriptor;
}

void readFile(Queue *database, Node **frame_begin, Database_settings param) {
  Tdata buffer1;
  Record buffer2;
  int frame_idx = 0;

  for (int i = 0; i < param.database_size; ++i) {
    fread(&buffer1, sizeof(buffer1), 1, param.file_descriptor);

    char *citizen_full_name = convertStr(buffer1.citizen_full_name);
    mstrcpy(buffer2.citizen_full_name, citizen_full_name);
    free(citizen_full_name);

    char *street_name = convertStr(buffer1.street_name);
    mstrcpy(buffer2.street_name, street_name);
    free(street_name);

    buffer2.house_number = buffer1.house_number;
    buffer2.apartment_number = buffer1.apartment_number;
    mstrcpy(buffer2.date_of_move_in, buffer1.date_of_move_in);

    enqueue(database, buffer2);

    if (i % param.database_records_to_show == 0)
      frame_begin[frame_idx++] = (*database).tail;
  }
}

#define COUNT_OF_SECTIONS 5
void showMenu() {
  system("clear");
  char *menuSection[COUNT_OF_SECTIONS] = {
      "Показать все записи", "Вывод по 20 записей", "Отсортировать и вывести",
      "Поиск по ключу", "Поиск в дереве"};

  for (int i = 0; i < COUNT_OF_SECTIONS; ++i) {
    printf("%d) %s\n", i + 1, menuSection[i]);
  }

  printf("\n0) Exit\n");
}
#undef COUNT_OF_SECTIONS

void digitalSort(Queue *S, byte *KDI) {
  Queue q[256];
  int L = sizeof(Record);
  Node *p = NULL;
  byte d = 0;
  byte k;

  for (int j = L - 1; j >= 0; --j) {
    for (int i = 0; i < 256; ++i) {
      q[i].head = NULL;
      q[i].tail = (Node *)&q[i].head;
    }

    p = (*S).head;
    k = KDI[j];
    while (p != NULL) {
      d = p->digit[k];

      if (isEmpty(&q[d])) {
        q[d].head = p;
        q[d].tail = p;
      } else {
        q[d].tail->next = p;
        q[d].tail = p;
      }

      p = p->next;
    }

    (*S).head = NULL;
    (*S).tail = (Node *)&(*S).head;

    p = (*S).tail;
    for (int i = 0; i < 256; ++i) {
      if (!isEmpty(&q[i])) {
        if (isEmpty(S)) {
          (*S).head = q[i].head;
          (*S).tail = q[i].tail;
        } else {
          p->next = q[i].head;
        }
        p = q[i].tail;
      }
    }
    p->next = NULL;
  }
}

void printData(Tdata data) {
  puts(data.citizen_full_name);
  puts(data.street_name);
  printf("%d\n", data.house_number);
  printf("%d\n", data.apartment_number);
  puts(data.date_of_move_in);
}

void printDData(Record data) {
  printf("%s | %s | %4d | %4d | %s\n", data.citizen_full_name, data.street_name,
         data.house_number, data.apartment_number, data.date_of_move_in);
}

void showFrame(Node **frame, int size, int idx) {
  Node *p = frame[idx];
  Record data;
  for (int i = 0; i < size; ++i) {
    data = p->data;
    printf("%4d | %s | %s | %4d | %4d | %s\n", i + (idx * size) + 1,
           data.citizen_full_name, data.street_name, data.house_number,
           data.apartment_number, data.date_of_move_in);
    p = p->next;
  }
}

void showAsFrame(Node **frame, Database_settings param) {
  int idx = 0;
  int isMod = 1;
  char action;

  while (1) {
    if (isMod) {
      system("clear");
      showFrame(frame, param.database_records_to_show, idx);
      isMod = 0;
    }

    read(STDIN_FILENO, &action, 1);

    switch (action) {
      case 'j':
        if (idx != 0) {
          idx--;
          isMod = 1;
        }
        break;
      case 'k':
        if (idx != param.count_of_frames) {
          idx++;
          isMod = 1;
        }
        break;
    }

    if (action == '0') break;
  }
}