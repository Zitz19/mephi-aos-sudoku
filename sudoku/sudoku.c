#include <time.h>
#include <stdlib.h>

#include "sudoku.h"

void clearField(char field[FIELD_SIZE][FIELD_SIZE]) {
    for (int i = 0; i < FIELD_SIZE; i++) {
        for (int j = 0; j < FIELD_SIZE; j++) {
            field[i][j] = ' ';
        }
    }
}

Coords generateNewField() {
    srand(time(NULL));
    int x = rand() % FIELD_SIZE;
    int y = rand() % FIELD_SIZE;
    Coords res;
    res.x = x;
    res.y = y;
    return res;
}