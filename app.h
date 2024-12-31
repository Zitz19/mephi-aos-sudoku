#ifndef SUDOKU_H
#define SUDOKU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>

#define MAX_CLIENTS 10

#define MAX_PENDING_CONNECTIONS 10

#define MESSAGE_LENGTH 256

#define FIELD_SIZE 9

typedef struct sockaddr AbstractSocketAddress;
typedef struct sockaddr_in InternetSocketAddress;

typedef struct {
    char orig[FIELD_SIZE][FIELD_SIZE];
    char mask[FIELD_SIZE][FIELD_SIZE];
} Field;

typedef struct {
    char message[MESSAGE_LENGTH];
    Field field;
} MainData;

typedef struct {
    long type;
    Field info;
} Message;

#endif /* SUDOKU_H */