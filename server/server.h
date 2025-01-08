
#ifndef SERVER_H
#define SERVER_H

#include "../app.h"
#include "config/config.h"
#include "../sudoku/sudoku.h"

#define TO_PROCESSING 1
#define FROM_PROCESSING 2

int main(int argc, char **argv);

void configureSignalProcessing();
void customSignalHandler(int signum);
void processSIGHUP();
void processSIGTERM();

void createAndConfigureSocket(int *serverSocket, int port);

void becomeDaemon(char *logFilename);

void createSharedMemory(int *sharedMemoryId);
void connectSharedMemory(MainData **sharedMemoryPointer, int sharedMemoryId);
void initializeSharedMemory(MainData* sharedMemoryPointer);
void turnOffSharedMemory(MainData *sharedMemoryPointer);
void deleteSharedMemory(int sharedMemoryId);

void createSemaphore(int *semaphoreId);
void initializeSemaphore(int semaphoreId);
void captureSemaphore(int semaphoreId);
void freeSemaphore(int semaphore);
void deleteSemaphore(int semaphoreId);

void createMessageQueue(int *messageQueueId);
void recieveMessage(Message *message, int type);
void deleteMessageQueue(int messageQueueId);

int awaitForClientSocket(int serverSocket);

void createWorkerToGenerateField(MainData *data, int clientSocket);
void createWorkerToRemoveCell(MainData *data, int clientSocket);

void startGame(MainData *data, int clientSocket);

void clearData(MainData* data);
void clearMessage(char message[MESSAGE_LENGTH]);

#endif
