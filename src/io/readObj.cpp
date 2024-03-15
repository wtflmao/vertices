//
// Created by root on 2024/3/15.
//

#include "readers.h"
#include "../structures/item.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <cctype>

// a dummy function
void processLine(const char* line) {
    if (line == nullptr) {
        return;
    }

    // to check if it is a comment or nothing
    if (line[0] == '#' || line[0] == '\0' || line[0] == '\n')
        return;

    // ignore mtllib and usemtl
    if (strstr(line, "mtllib") || strstr(line, "usemtl")) {
        return;
    }

    // ignore smooth rays line
    if (line[0] == 's' && line[1] == ' ') {
        return;
    }

    // ignore object notation
    if (line[0] == 'o' && line[1] == ' ') {
        return;
    }

    // to check if it is a vertex
    if (line[0] == 'v' && line[1] == ' ') {
        processVertex(line);
        return;
    }

    // to check if it is a face
    if (line[0] == 'f' && line[1] == ' ') {
        processFace(line);
        return;
    }

    // to check if it is a normal
    if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
        // processNormal(line);
        // normal from the file is not used now
        return;
    }

    // to check if it is a texture
    if (line[0] == 'v' && line[1] == 't' && line[2] == ' ') {
        processTexture(line);
        return;
    }

}

// We use C style I/O cuz it may faster than the C++ one
// even if we use std::ios::sync_with_stdio(false);
std::unique_ptr<Item> readAndPass(const char *filename) {
    auto nilPtr = std::make_unique<Item>(true);
    FILE *fp = fopen(filename, "rt");
    if (fp == nullptr) {
        fprintf(stderr, "Cannot open %s\a\n", filename);
        return nilPtr;
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
        return nilPtr;
    } else {
        fprintf(stdout, "File size: %zu bytes\n", size);
    }
    fseek(fp, 0, SEEK_SET);

    char line[MAX_LINE_LENGTH] = {0};
    int count = 0;
    auto itemPtr = std::make_unique<Item>(false);

    while (fgets(line, sizeof(line), fp)) {
        // replace all \r to \0 so we can no longer care about if it is CRLF or LF
        char *p = strchr(line, '\r');
        if (p) *p = '\0';
        processLine(line);
        count++;
    }

    fclose(fp);
    fprintf(stdout, "Processed %d lines from %s\n", count, filename);
    return itemPtr;
}

