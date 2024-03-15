//
// Created by root on 2024/3/15.
//

#include "readNewItem.h"

// We use C style I/O cuz it may faster than the C++ one
// even if we use std::ios::sync_with_stdio(false);
bool readNewItem(const char *filename, Item& item) {
    FILE *fp = fopen(filename, "rt");
    if (fp == nullptr) {
        fprintf(stderr, "Cannot open %s\a\n", filename);
        return false;
    }
    fseek(fp, 0, SEEK_END);
    size_t size = 0;

    // get the size of the file
    #ifdef _WIN32
        _fseeki64(fp, 0, SEEK_END);
        size = _ftelli64(fp);
    #elif __unix__ || __unix || __APPLE__ || __MACH__ || __linux__
        fseeko(fp, 0, SEEK_END);
            size = ftello(fp);
        #else
            fseeko64(fp, 0, SEEK_END);
            size = ftello64(fp);
    #endif

    if(size > MAX_FILE_SIZE) {
        fprintf(stderr, "File is too large\a\n");
        return false;
    } else {
        fprintf(stdout, "File size: %zu bytes\n", size);
    }
    fseek(fp, 0, SEEK_SET);

    char line[MAX_LINE_LENGTH] = {0};
    int count = 0;

    while (fgets(line, sizeof(line), fp)) {
        // replace all \r to \0 so we can no longer care about if it is CRLF or LF
        char *p = strchr(line, '\r');
        if (p) *p = '\0';
        // pass the reference to the call, without losing the ownership to that unique_ptr
        // sounds like something in Rust? OWNERSHIP!
        processLine(line, item);
        count++;
    }

    fclose(fp);
    fprintf(stdout, "Processed %d lines from %s\n", count, filename);
    return true;
}
