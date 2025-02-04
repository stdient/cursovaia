#include "../include/other_functions.h"

int main() {
    std::string filename = "testBase4.dat";
    FILE *file = fopen(filename.c_str(), "rb");

    if (!file) {
        perror(filename.c_str());
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
