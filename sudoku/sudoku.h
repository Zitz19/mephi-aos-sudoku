#ifndef SEABATTLE_H
#define SEABATTLE_H

#include "../app.h"

#define COMMAND_LENGTH 256

typedef struct {
    int x;
    int y;
} Coords;

void clearField(char field[FIELD_SIZE][FIELD_SIZE]);
Coords generateNewField();

typedef struct {
    char body[COMMAND_LENGTH];
} Command;

#endif