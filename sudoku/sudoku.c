#include <time.h>
#include <stdlib.h>

#include "sudoku.h"

void clearField(char field[FIELD_SIZE][FIELD_SIZE]) {
    for (int i = 0; i < FIELD_SIZE; i++) {
        for (int j = 0; j < FIELD_SIZE; j++) {
            field[i][j] = 'x';
        }
    }
}

void generateBasicField(char field[FIELD_SIZE][FIELD_SIZE]) {
    int n = FIELD_SIZE / 3;
    for (int i = 0; i < FIELD_SIZE; ++i) {
        for (int j = 0; j < FIELD_SIZE; ++j) {
            field[i][j] = '0' + (i*n + i/n + j) % FIELD_SIZE + 1;
        }
    }
}

void transposeField(char field[FIELD_SIZE][FIELD_SIZE]) {
    int n = FIELD_SIZE / 3;
    for (int i = 0; i < FIELD_SIZE; i++) {
        for (int j = i + 1; j < FIELD_SIZE; j++) {
            int temp = field[i][j];
            field[i][j] = field[j][i];
            field[j][i] = temp;
        }
    }
}

void swapRowsSmall(char field[FIELD_SIZE][FIELD_SIZE]) {
    int n = FIELD_SIZE / 3;
    int area = getRandomInt() % n;
    int line1 = getRandomInt() % n;
    int line2 = getRandomInt() % n;

    while (line1 == line2) {
        line2 = getRandomInt() % n;
    }

    int N1 = area * n + line1;
    int N2 = area * n + line2;

    char temp[9] = {};
    memcpy(temp, field[N1], FIELD_SIZE);
    // field[N1] = field[N2];
    memcpy(field[N1], field[N2], FIELD_SIZE);
    // field[N2] = temp;
    memcpy(field[N2], temp, FIELD_SIZE);
}

void swapColumnsSmall(char field[FIELD_SIZE][FIELD_SIZE]) {
    transposeField(field);
    swapRowsSmall(field);
    transposeField(field);
}

void swapRowsArea(char field[FIELD_SIZE][FIELD_SIZE]) {
    int n = FIELD_SIZE / 3;
    int area1 = getRandomInt() % n;
    int area2 = getRandomInt() % n;

    while (area1 == area2) {
        area2 = getRandomInt() % n;
    }

    for (int i = 0; i < n; i++) {
        int N1 = area1 * n + i;
        int N2 = area2 * n + i;

        char temp[9] = {};
        memcpy(temp, field[N1], FIELD_SIZE);
        // field[N1] = field[N2];
        memcpy(field[N1], field[N2], FIELD_SIZE);
        // field[N2] = temp;
        memcpy(field[N2], temp, FIELD_SIZE);
    }
}

void swapColumnsArea(char field[FIELD_SIZE][FIELD_SIZE]) {
    transposeField(field);
    swapRowsArea(field);
    transposeField(field);
}

void mixField(char field[FIELD_SIZE][FIELD_SIZE], int amt) {
    void (*mix_funcs[])(char [FIELD_SIZE][FIELD_SIZE]) = {
        transposeField,
        swapRowsSmall,
        swapColumnsSmall,
        swapRowsArea,
        swapColumnsArea
    };

    int num_funcs = sizeof(mix_funcs) / sizeof(mix_funcs[0]);

    for (int i = 0; i < amt; i++) {
        int func_id = getRandomInt() % num_funcs;
        mix_funcs[func_id](field);
    }
}


Coords removeCell() {
    int x = getRandomInt() % FIELD_SIZE;
    int y = getRandomInt() % FIELD_SIZE;
    // printf("gen %d %d\n", x, y);
    Coords res;
    res.x = x;
    res.y = y;
    return res;
}