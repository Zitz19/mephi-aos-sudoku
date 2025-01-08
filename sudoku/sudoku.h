#ifndef SEABATTLE_H
#define SEABATTLE_H

#include "../app.h"
#include "random/myrandom.h"

#define COMMAND_LENGTH 256

typedef struct {
    int x;
    int y;
} Coords;

void clearField(char field[FIELD_SIZE][FIELD_SIZE]);
void generateBasicField(char field[FIELD_SIZE][FIELD_SIZE]);
void transposeField(char field[FIELD_SIZE][FIELD_SIZE]);
void swapRowsSmall(char field[FIELD_SIZE][FIELD_SIZE]);
void swapColumnsSmall(char field[FIELD_SIZE][FIELD_SIZE]);
void swapRowsArea(char field[FIELD_SIZE][FIELD_SIZE]);
void swapColumnsArea(char field[FIELD_SIZE][FIELD_SIZE]);
void mixField(char field[FIELD_SIZE][FIELD_SIZE], int amt);
Coords removeCell();

typedef struct {
    char body[COMMAND_LENGTH];
} Command;

#endif