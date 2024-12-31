#ifndef CLIENT_H
#define CLIENT_H

#include "../app.h"
#include "../sudoku/sudoku.h"

int main(int argc, char** argv);

void createAndConfigureSocket(int *clientSocket, char *hostname, int port);

#endif