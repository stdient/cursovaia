#include "../include/other_functions.h"

std::string convert_alphabet(std::string alphabet_utf8)
{
    iconv_t cd = iconv_open("WINDOWS-1251", "UTF8");
    if (cd == (iconv_t)-1)
    {
        throw std::runtime_error("iconv_open failed");
    }

    size_t inbytesleft = alphabet_utf8.size();
    size_t outbytesleft = inbytesleft - 33;

    std::vector<char> output(outbytesleft);
    char *inbuf = const_cast<char *>(alphabet_utf8.data());
    char *outbuf = output.data();

    if (iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft) == (size_t)-1)
    {
        throw std::runtime_error("iconv conversion failed");
    }
    iconv_close(cd);

    return std::string(output.data(), outbuf - output.data());
}

std::string convert(std::string text, std::string from_code, std::string to_code)
{
    iconv_t cd = iconv_open(to_code.c_str(), from_code.c_str());
    if (cd == (iconv_t)-1)
    {
        throw std::runtime_error("iconv open failed");
    }

    size_t inbytesleft = text.size();
    size_t outbytesleft = inbytesleft;
    std::vector<char> output(outbytesleft);

    char *inbuf = &text[0];
    char *outbuf = output.data();

    if (iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft) == (size_t)-1)
    {
        throw std::runtime_error("convert failed");
    }

    iconv_close(cd);

    return std::string(output.data(), outbuf - output.data());
}

std::string convert(Record record, std::string from_code, std::string to_code)
{
    std::string result;
    result = convert(record.citizen_full_name, from_code, to_code);
    result += ' ';
    result += convert(record.street_name, from_code, to_code);
    result += ' ';
    result += convert(std::to_string(record.house_number), from_code, to_code);
    result += ' ';
    result += convert(std::to_string(record.apartment_number), from_code, to_code);
    result += ' ';
    result += convert(record.date_of_move_in, from_code, to_code);
    result += ' ';

    return result;
}

void record_converted_values(std::vector<symbol> &symbols)
{
    iconv_t cd = iconv_open("UTF8", "WINDOWS-1251");
    if (cd == (iconv_t)-1)
    {
        throw std::runtime_error("iconv_open failed");
    }

    for (size_t i = 0; i < symbols.size(); ++i)
    {
        size_t inbytesleft = 1;
        size_t outbytesleft = 4;
        std::vector<char> output(outbytesleft);
        std::vector<char> input(inbytesleft);
        char *inbuf = input.data();
        char *outbuf = output.data();

        inbuf[inbytesleft - 1] = symbols[i].value;
        if (iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft) == (size_t)-1)
        {
            throw std::runtime_error("iconv conversion failed");
        }
        symbols[i].converted_value = std::string(output.data(), outbuf - output.data());
    }

    iconv_close(cd);
}

std::vector<symbol> find_probabilities(std::string filename)
{
    std::ifstream file(filename);
    std::vector<symbol> symbols;
    bool symbol_found;
    char ch;
    symbol temp;
    int filesize = 0;

    if (file.is_open())
    {
        while (file.get(ch))
        {
            symbol_found = 0;
            for (size_t i = 0; i < symbols.size(); ++i)
            {
                if (symbols[i].value == ch)
                {
                    symbol_found = 1;
                    symbols[i].amount_reps++;
                    break;
                }
            }
            if (!symbol_found)
            {
                temp.value = ch;
                temp.amount_reps = 1;
                symbols.push_back(temp);
            }
            filesize++;
        }
        file.close();
    }

    double accurate = 100000.0;
    for (size_t i = 0; i < symbols.size(); ++i)
    {
        symbols[i].probability = std::round(symbols[i].amount_reps / (double)filesize * accurate) / accurate;
    }

    return symbols;
}

bool isRusLetter(symbol symbol)
{
    std::string unacceptable_values = "?!,.-; 1234567890\n";

    for (auto sym : unacceptable_values)
    {
        if (symbol.converted_value[0] == sym)
            return false;
    }
    return true;
}

void printLine(int size)
{
    for (int i = 0; i < size; ++i)
        std::cout << '-';
    std::cout << std::endl;
}

void showTable1(std::vector<symbol> symbols)
{
    int shift;
    printLine(63);
    std::cout << "|Символ|Вероятность символа|Кодовое слово|Длина кодового слова|" << std::endl;
    printLine(63);
    for (size_t i = 0; i < symbols.size(); ++i)
    {
        if (!isRusLetter(symbols[i]))
            shift = 6;
        else
            shift = 7;

        if (symbols[i].converted_value[0] == '\n')
            std::cout << '|' << std::setw(shift) << "nls";
        else
            std::cout << '|' << std::setw(shift) << symbols[i].converted_value;

        std::cout << '|' << std::setw(19) << symbols[i].probability;
        std::cout << '|' << std::setw(14 - symbols[i].code_word.size());
        for (auto digit : symbols[i].code_word)
        {
            std::cout << digit;
        }
        std::cout << '|' << std::setw(20) << symbols[i].code_word_len;
        std::cout << '|' << std::endl;
    }
    printLine(63);
}

void showTable2(double craft, double entropy, double average_len)
{
    std::string buffer;
    std::string titles = "|Неравентство Крафта                |Энтропия исходного "
                         "текста|Средняя длина кодового "
                         "слова|Избыточность кода|";

    int amount_one_byte_symbols_in_title = 14 + 9 + 5;
    int devided_line_size = (titles.size() - amount_one_byte_symbols_in_title) / 2 + amount_one_byte_symbols_in_title;
    printLine(devided_line_size);
    std::cout << titles << std::endl;
    printLine(devided_line_size);

    int shift = 48;
    buffer += std::to_string(craft);
    buffer += " <= 1, ";
    if (craft <= 1)
    {
        buffer += "выполняется";
        shift -= 2;
    }
    else
        buffer += "не выполняется";
    std::cout << '|' << std::setw(shift) << buffer;
    buffer.clear();

    std::cout << '|' << std::setw(25) << entropy;
    std::cout << '|' << std::setw(28) << average_len;
    std::cout << '|' << std::setw(17) << average_len - entropy;
    std::cout << '|' << std::endl;

    printLine(devided_line_size);
}

void gilbert_mur(std::vector<symbol> &symbols)
{
    for (size_t i = 0; i < symbols.size(); ++i)
    {
        for (size_t j = 0; j < i; ++j)
            symbols[i].cumulative_probability += symbols[j].probability;
        symbols[i].cumulative_probability += symbols[i].probability / 2.0;

        symbols[i].code_word_len = ceil(-1 * log2(symbols[i].probability)) + 1;
    }

    double q;
    for (size_t i = 0; i < symbols.size(); ++i)
    {
        q = symbols[i].cumulative_probability;
        for (size_t j = 0; j < symbols[i].code_word_len; ++j)
        {
            q *= 2;
            symbols[i].code_word.push_back(floor(q));
            if (q > 1)
                q -= 1;
        }
    }
}

double craft_formula(std::vector<symbol> symbols, int i)
{
    return 1 / (pow(2, symbols[i].code_word_len));
}
double entropy_formula(std::vector<symbol> symbols, int i)
{
    return -1 * symbols[i].probability * log2(symbols[i].probability);
}
double average_len_formula(std::vector<symbol> symbols, int i)
{
    return symbols[i].probability * symbols[i].code_word_len;
}

double find_symbols_sum(std::vector<symbol> symbols, double (*function)(std::vector<symbol> symbols, int i))
{
    double sum = 0;
    for (size_t i = 0; i < symbols.size(); ++i)
    {
        sum += function(symbols, i);
    }
    return sum;
}

void swap(symbol &a, symbol &b)
{
    symbol t = a;
    a = b;
    b = t;
}

void sort(std::vector<symbol> &arr, int L, int R)
{
    int mid = floor((double)(L + R) / 2);
    symbol support_element = arr[mid];
    int idx_current_left = L, idx_current_right = R;

    while (idx_current_left <= idx_current_right)
    {
        while (arr[idx_current_left].value < support_element.value)
        {
            idx_current_left++;
        }
        while (arr[idx_current_right].value > support_element.value)
        {
            idx_current_right--;
        }

        if (idx_current_left <= idx_current_right)
        {
            swap(arr[idx_current_left], arr[idx_current_right]);
            idx_current_left++;
            idx_current_right--;
        }
    }

    if (idx_current_left < R)
        sort(arr, idx_current_left, R);
    if (L < idx_current_right)
        sort(arr, L, idx_current_right);
}

void task(std::vector<symbol> &symbols)
{
    for (size_t i = 0; i < symbols.size(); ++i)
        symbols[i].reset();

    sort(symbols, 0, symbols.size() - 1);

    gilbert_mur(symbols);
    showTable1(symbols);

    double craft = find_symbols_sum(symbols, craft_formula);
    double entropy = find_symbols_sum(symbols, entropy_formula);
    double average_len = find_symbols_sum(symbols, average_len_formula);
    showTable2(craft, entropy, average_len);
}

int buildTree(Queue &keys, int amount_keys, byte *KDI, Tree<Record> &tree)
{
    system("clear");

    if (!amount_keys)
    {
        printf("Сначала нужно выполнить поиск по ключу\n");
        getchar();
        return 1;
    }

    for (size_t i = 0; i < sizeof(Record); ++i)
    {
        KDI[i] = 0;
    }
    KDI[0] = (byte)(sizeof(keys.head->data.citizen_full_name) + sizeof(keys.head->data.street_name));
    digitalSort(&keys, KDI);

    std::vector<Record> keys_array;
    for (Node *p = keys.head; p != NULL; p = p->next)
    {
        keys_array.push_back(p->data);
    }

    tree.buildA2Tree(keys_array);

    return 0;
}

int findInTree(Tree<Record> &tree, int amount_keys, struct termios old, struct termios cur)
{
    system("clear");

    if (!amount_keys)
    {
        printf("Сначала нужно выполнить поиск по ключу\n");
        getchar();
        return 1;
    }

    std::cout << "Введите ключ (номер дома)" << std::endl;
    int key;

    setattr(&old, 1);
    std::cin >> key;
    setattr(&cur, 0);
    clearBuf();

    tree.findVertex(key);

    return 0;
}

void encodeTheFile(Queue database)
{
    system("clear");

    std::string filename = "temp.txt";
    std::ofstream file(filename);
    if (file.is_open())
    {
        for (Node *p = database.head; p != NULL; p = p->next)
        {
            file << convert(p->data, "UTF8", "WINDOWS-1251") << std::endl;
        }

        file.close();
    }

    std::vector<symbol> symbols = find_probabilities(filename);
    record_converted_values(symbols);
    task(symbols);

    getchar();
    remove(filename.c_str());
}

int menuLoop(Queue database, Node **database_frames, Database_settings param)
{
    struct termios cur, old;
    cur = setNewTerminalSettings(&old);

    byte KDI[sizeof(Record)] = {
        113, 112, 111, 110, 109, 108, 107, 106, 105, 104, 64, 65, 66, 67, 68,
        69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79, 80, 81, 82, 83,
    };
    Queue sortedBase = copyQueueAndSort(database, KDI);

    Node *sortedBase_frame[param.count_of_frames];
    Node *sortedBase_array[param.database_size];
    int sortedBase_idx_array[param.database_size];
    splitIntoFramesAndBuildIdxArray(&sortedBase, sortedBase_frame, sortedBase_array, sortedBase_idx_array, param);

    char action;

    Queue keys;
    int amount_keys = 0;

    Tree<Record> tree;

    while (1)
    {
        showMenu();
        read(STDIN_FILENO, &action, 1);

        switch (action)
        {
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
            if (searchData(old, cur, sortedBase_array, sortedBase_idx_array, param, &amount_keys, &keys))
                break;
            break;

        case '5':
            if (buildTree(keys, amount_keys, KDI, tree))
                break;
            tree.display();
            getchar();
            break;

        case '6':
            findInTree(tree, amount_keys, old, cur);
            break;

        case '7':
            encodeTheFile(database);
            break;

        case '0':
            EndingOfProgram(old, &keys, &sortedBase, amount_keys);
            return EXIT_SUCCESS;
            break;
        }
    }
}

int readKey(char *key)
{
    system("clear");
    std::cout << "Введите ключ поиска (год поселения)" << std::endl;

    for (int i = 0; i < 2; ++i)
    {
        key[i] = getchar();
        if (key[i] == 'q')
        {
            clearBuf();
            return EXIT_SUCCESS;
        }
    }

    clearBuf();
    return EXIT_SUCCESS;
}

void setattr(struct termios *attr, int show_cursor)
{
    tcsetattr(STDIN_FILENO, TCSANOW, attr);

    if (show_cursor)
        printf("\e[?25h");
    else
        printf("\e[?25l");
}

void initParam(Database_settings *param, int database_size, int database_records_to_show, int count_of_frames,
               FILE *file_descriptor)
{
    (*param).database_size = database_size;
    (*param).database_records_to_show = database_records_to_show;
    (*param).count_of_frames = count_of_frames;
    (*param).file_descriptor = file_descriptor;
}

void copyStringInBuffer(Tdata buffer1, Record *buffer2)
{
    char *citizen_full_name = convertStr(buffer1.citizen_full_name);
    mstrcpy((*buffer2).citizen_full_name, citizen_full_name);
    free(citizen_full_name);

    char *street_name = convertStr(buffer1.street_name);
    mstrcpy((*buffer2).street_name, street_name);
    free(street_name);
}

void readFile(Queue *database, Node **frame_begin, Database_settings param)
{
    Tdata buffer1;
    Record buffer2;
    int frame_idx = 0;

    for (int i = 0; i < param.database_size; ++i)
    {
        fread(&buffer1, sizeof(buffer1), 1, param.file_descriptor);

        copyStringInBuffer(buffer1, &buffer2);

        buffer2.house_number = buffer1.house_number;
        buffer2.apartment_number = buffer1.apartment_number;
        mstrcpy(buffer2.date_of_move_in, buffer1.date_of_move_in);

        enqueue(database, buffer2);

        if (i % param.database_records_to_show == 0)
            frame_begin[frame_idx++] = (*database).tail;
    }
}

#define COUNT_OF_SECTIONS 7
void showMenu()
{
    system("clear");
    std::string menuSection[COUNT_OF_SECTIONS] = {
        "Показать все записи", "Вывод по 20 записей", "Отсортировать и вывести", "Поиск по ключу",
        "Построить дерево",    "Поиск в дереве",      "Закодировать файл"};

    for (int i = 0; i < COUNT_OF_SECTIONS; ++i)
    {
        printf("%d) %s\n", i + 1, menuSection[i].c_str());
    }

    printf("\n0) Exit\n");
}
#undef COUNT_OF_SECTIONS

void digitalSort(Queue *S, byte *KDI)
{
    Queue q[256];
    int L = sizeof(Record);
    Node *p = NULL;
    byte d = 0;
    byte k;

    for (int j = L - 1; j >= 0; --j)
    {
        for (int i = 0; i < 256; ++i)
        {
            q[i].head = NULL;
            q[i].tail = (Node *)&q[i].head;
        }

        p = (*S).head;
        k = KDI[j];
        while (p != NULL)
        {
            d = p->digit[k];

            if (isEmpty(&q[d]))
            {
                q[d].head = p;
                q[d].tail = p;
            }
            else
            {
                q[d].tail->next = p;
                q[d].tail = p;
            }

            p = p->next;
        }

        (*S).head = NULL;
        (*S).tail = (Node *)&(*S).head;

        p = (*S).tail;
        for (int i = 0; i < 256; ++i)
        {
            if (!isEmpty(&q[i]))
            {
                if (isEmpty(S))
                {
                    (*S).head = q[i].head;
                    (*S).tail = q[i].tail;
                }
                else
                {
                    p->next = q[i].head;
                }
                p = q[i].tail;
            }
        }
        p->next = NULL;
    }
}

void printData(Tdata data)
{
    puts(data.citizen_full_name);
    puts(data.street_name);
    printf("%d\n", data.house_number);
    printf("%d\n", data.apartment_number);
    puts(data.date_of_move_in);
}

void printDData(Record data)
{
    printf("%s | %s | %4d | %4d | %s\n", data.citizen_full_name, data.street_name, data.house_number,
           data.apartment_number, data.date_of_move_in);
}

void showFrame(Node **frame, int size, int idx)
{
    Node *p = frame[idx];
    Record data;
    for (int i = 0; i < size; ++i)
    {
        data = p->data;
        printf("%4d | %s | %s | %4d | %4d | %s\n", i + (idx * size) + 1, data.citizen_full_name, data.street_name,
               data.house_number, data.apartment_number, data.date_of_move_in);
        p = p->next;
    }
}

void showAsFrame(Node **frame, Database_settings param)
{
    int idx = 0;
    int isMod = 1;
    char action;

    while (1)
    {
        if (isMod)
        {
            system("clear");
            showFrame(frame, param.database_records_to_show, idx);
            isMod = 0;
        }

        read(STDIN_FILENO, &action, 1);

        switch (action)
        {
        case 'j':
            if (idx != 0)
            {
                idx--;
                isMod = 1;
            }
            break;
        case 'k':
            if (idx != param.count_of_frames)
            {
                idx++;
                isMod = 1;
            }
            break;
        }

        if (action == '0')
            break;
    }
}

char *findYear(char *date, std::string sep)
{
    char *token = mstrtok(date, sep.c_str());
    for (int i = 0; i < 2; ++i)
    {
        token = mstrtok(NULL, sep.c_str());
    }

    return token;
}

int binarySearch(Node **arr, int *idx_arr, int key, int n)
{
    int L = 0, //
        R = n - 1;

    int year;
    char bufs[10];

    int m;
    while (L < R)
    {
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

    if (year == key)
        return idx_arr[R];
    return -1;
}

void clearBuf()
{
    char ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
}

struct termios setNewTerminalSettings(struct termios *old)
{
    struct termios cur;
    tcgetattr(STDIN_FILENO, &cur);
    (*old) = cur;
    cur.c_lflag &= ~(ISIG | ICANON | ECHO);
    cur.c_cc[VMIN] = 1;
    cur.c_cc[VTIME] = 0;
    setattr(&cur, 0);

    return cur;
}

Queue copyQueueAndSort(Queue database, byte *KDI)
{
    Queue sortedBase;

    init(&sortedBase);
    for (Node *p = database.head; p != NULL; p = p->next)
    {
        enqueue(&sortedBase, p->data);
    }
    digitalSort(&sortedBase, KDI);

    return sortedBase;
}

void splitIntoFramesAndBuildIdxArray(Queue *sortedBase, Node **sortedBase_frame, Node **sortedBase_array,
                                     int *sortedBase_idx_array, Database_settings param)
{
    Node *p = (*sortedBase).head;
    int frame_idx = 0;

    for (int i = 0; i < param.database_size; ++i)
    {
        sortedBase_idx_array[i] = i;
        sortedBase_array[i] = p;

        if (i % param.database_records_to_show == 0)
            sortedBase_frame[frame_idx++] = p;

        p = p->next;
    }
}

void EndingOfProgram(struct termios old, Queue *keys, Queue *sortedBase, int amount_keys)
{
    setattr(&old, 1);
    system("clear");
    if (amount_keys)
        destroy(keys);
    destroy(sortedBase);
}

int searchData(struct termios old, struct termios cur, Node **sortedBase_array, int *sortedBase_idx_array,
               Database_settings param, int *amount_keys, Queue *keys)
{
    int key_size = 3;
    char key_str[key_size];
    for (int i = 0; i < key_size; ++i)
    {
        key_str[i] = '\0';
    }
    int first_key_entry = 0;
    char year_str[10];

    system("clear");

    setattr(&old, 1);
    while (readKey(key_str))
        ;
    setattr(&cur, 0);

    first_key_entry =
        binarySearch(sortedBase_array, sortedBase_idx_array, convertStrToInt(key_str), param.database_size);

    if (first_key_entry == -1)
    {
        fprintf(stderr, "Элемент не найден\n");
        getchar();
        return 1;
    }

    if ((*amount_keys))
        destroy(keys);
    init(keys);
    (*amount_keys) = 0;

    mstrcpy(year_str, sortedBase_array[sortedBase_idx_array[first_key_entry]]->data.date_of_move_in);

    while (convertStrToInt(findYear(year_str, "- ")) == convertStrToInt(key_str))
    {
        enqueue(keys, sortedBase_array[sortedBase_idx_array[first_key_entry + (*amount_keys)]]->data);
        (*amount_keys)++;

        if (first_key_entry + (*amount_keys) == param.database_size)
            break;

        mstrcpy(year_str,
                sortedBase_array[sortedBase_idx_array[first_key_entry + (*amount_keys)]]->data.date_of_move_in);
    }

    showFrame(&(*keys).head, (*amount_keys), 0);

    getchar();
    return 0;
}