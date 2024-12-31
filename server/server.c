#include "server.h"
#include <signal.h>

char logFilename[256];
char configFilename[256];

int port;

int serverSocket;
int clientSocket;

int sharedMemoryId;

int semaphoreId;

int messageQueueId;

int main(int argc, char** argv) {
    if (argc != 2) {
        write(STDERR_FILENO, "Incorrect arguments input! It should be ./server configurationsFilename\n", 73);
        return 1;
    }

    MainData* sharedMemoryPointer;


    //====================================================

    configureSignalProcessing();

    strcpy(configFilename, argv[1]);
    parseConfig(configFilename, logFilename, &port);

    createAndConfigureSocket(&serverSocket, port);

    becomeDaemon(logFilename);

    createSharedMemory(&sharedMemoryId);
    connectSharedMemory(&sharedMemoryPointer, sharedMemoryId);
    initializeSharedMemory(sharedMemoryPointer);

    createSemaphore(&semaphoreId);
    initializeSemaphore(semaphoreId);

    createMessageQueue(&messageQueueId);

    //core
    {
        write(STDIN_FILENO, "Waiting for connection\n", 24);
        clientSocket = awaitForClientSocket(serverSocket);
        
        int gameStartedFlag = 0;
        MainData data;
        while (1) {
            char buffer[256];
            int x = 0;
            int y = 0;
            Command command;

            clearMessage(data.message);
            read(clientSocket, &command, sizeof(command));

            if (strcmp(command.body, "generate") == 0) {
                write(clientSocket, &data, sizeof(MainData));
            } else {
                strcpy(data.message, "Unknown command");
                write(STDERR_FILENO, "Unknown command", 16);
            }
        }
    }

    const char *gameStarted = "GAME STARTED!";
    write(clientSocket, gameStarted, sizeof(gameStarted));

    turnOffSharedMemory(sharedMemoryPointer);

    deleteSemaphore(semaphoreId);

    deleteMessageQueue(messageQueueId);
}

void configureSignalProcessing() {
    signal(SIGTERM, customSignalHandler);
    signal(SIGHUP, customSignalHandler);
}

void customSignalHandler(int signum) {
    switch (signum) {
    case SIGTERM:
        processSIGTERM();
    case SIGHUP:
        processSIGHUP();
    }
}

//Terminate and send result to client
void processSIGTERM() {
    MainData* data = malloc(sizeof(MainData));
    connectSharedMemory(&data, sharedMemoryId);

    write(clientSocket, data, sizeof(MainData));

    if (shmctl(sharedMemoryId, IPC_RMID, NULL) == -1) {
        write(STDERR_FILENO, "Error on deleting shared memory\n", 33);
        exit(1);
    }

    close(clientSocket);
    close(serverSocket);
    
    write(STDOUT_FILENO, "\nSIGTERM has been called\n", 26);
    exit(0);
}

//Re-read config file
void processSIGHUP() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGHUP);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        write(STDERR_FILENO, "Error on blocking SIGHUP signal", 32);
    }

    parseConfig(configFilename, logFilename, &port);

    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
        write(STDERR_FILENO, "Error on unblocking SIGHUP signal", 34);
    }
    
    write(STDOUT_FILENO, "\nSIGHUP has been called\n", 25);
}

void createAndConfigureSocket(int *serverSocket, int port) {
    if ((*serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        write(STDERR_FILENO, "Socket establishment", 21);
        exit(1);
    }

    InternetSocketAddress serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);       //convert to internet port correctly
    serverAddress.sin_addr.s_addr = INADDR_ANY; //bind to all local interfaces
    if (bind(*serverSocket, (AbstractSocketAddress*) &serverAddress, (socklen_t) sizeof(serverAddress)) == -1) {
        write(STDERR_FILENO, "Error on binding socket with address", 37);
        exit(1);
    }

    if (listen(*serverSocket, MAX_PENDING_CONNECTIONS) == -1) {
        write(STDERR_FILENO, "Error on listening to upcoming connections", 43);
        exit(1);
    }
}

void becomeDaemon(char* logFilename) {
    chdir("/");
    if (fork()) {
        // wait(NULL);
        exit(0);
    }

    setsid();

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    int loggingDescriptor = open(logFilename, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (loggingDescriptor == -1) {
        write(STDERR_FILENO, "Error on open logging file", 27);
        exit(1);
    }
    
    dup2(loggingDescriptor, STDIN_FILENO);
    dup2(loggingDescriptor, STDOUT_FILENO);
    dup2(loggingDescriptor, STDERR_FILENO);

    close(loggingDescriptor);
}

void createSharedMemory(int *sharedMemoryId) {
    *sharedMemoryId = shmget(IPC_PRIVATE, sizeof(MainData), IPC_CREAT | 0666);
    if (*sharedMemoryId == -1) {
        write(STDERR_FILENO, "Error on creating shared memory id", 35);
        exit(1);
    }
}

void connectSharedMemory(MainData **sharedMemoryPointer, int sharedMemoryId) {
    //Gets random free segment
    *sharedMemoryPointer = (MainData*)shmat(sharedMemoryId, NULL, 0);
    if (*sharedMemoryPointer == (MainData*) - 1) {
        write(STDERR_FILENO, "Error on connecting shared memory", 34);
        exit(1);
    }
}

void turnOffSharedMemory(MainData* sharedMemoryPointer) {
    if (shmdt(sharedMemoryPointer) == -1) {
        write(STDERR_FILENO, "Error on turning off shared memory", 35);
        exit(1);
    }
}

void deleteSharedMemory(int sharedMemoryId) {
    if (shmctl(sharedMemoryId, IPC_RMID, NULL) == -1) {
        write(STDERR_FILENO, "Error on deleting shared memory", 32);
        exit(1);
    }
}

void initializeSharedMemory(MainData* sharedMemoryPointer) {
    clearData(sharedMemoryPointer);
}

void clearData(MainData* data) {
    clearMessage(data->message);
    clearField(data->field.orig);
    clearField(data->field.mask);
}

void createSemaphore(int *semaphoreId) {
    *semaphoreId = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (*semaphoreId == -1) {
        write(STDERR_FILENO, "Error on creating semaphore id", 31);
        exit(1);
    }
}

void initializeSemaphore(int semaphoreId) {
    /* according to X/OPEN we have to define it ourselves */
    union semun {
        int val;                  /* value for SETVAL */
        struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
        unsigned short *array;    /* array for GETALL, SETALL */
                                    /* Linux specific part: */
        struct seminfo *__buf;    /* buffer for IPC_INFO */
    } arg;

    //0 - free, 1 - locked
    arg.val = 1;
    if (semctl(semaphoreId, 0, SETVAL, arg) == -1) {
        write(STDERR_FILENO, "Error on initializing semaphore", 32);
        exit(1);
    }
}

void captureSemaphore(int semaphoreId) {
    struct sembuf op = {0, -1, 0};
    if (semop(semaphoreId, &op, 1) == -1) {
        write(STDERR_FILENO, "Error on capture semaphore", 27);
        exit(1);
    }
}

void freeSemaphore(int semaphore) {
    struct sembuf op = {0, 1, 0};
    if (semop(semaphoreId, &op, 1) == -1) {
        write(STDERR_FILENO, "Error on free semaphore", 24);
        exit(1);
    }
}

void deleteSemaphore(int semaphoreId) {
    if (semctl(semaphoreId, 0, IPC_RMID) == -1) {
        write(STDERR_FILENO, "Error on deleting semaphore", 28);
        exit(1);
    }
}

void createMessageQueue(int *messageQueueId) {
    *messageQueueId = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    if (*messageQueueId == -1) {
        write(STDERR_FILENO, "Error on creating message queue", 32);
        exit(1);
    }
}

void sendMessage(char orig[MESSAGE_LENGTH], char mask[MESSAGE_LENGTH], int type) {
    Message msg;
    msg.type = type;
    strcpy(orig, msg.info.orig);
    strcpy(mask, msg.info.mask);
    if (msgsnd(messageQueueId, &msg, sizeof(Field), 0) == -1) {
        write(STDERR_FILENO, "Error on sending a message", 27);
        exit(1);
    }
}

void recieveMessage(Message *msg, int type) {
    if (msgrcv(messageQueueId, msg, sizeof(Field), type, 0) == -1) {
        write(STDERR_FILENO, "Error on recieving a message", 29);
        exit(1);
    }
}

void deleteMessageQueue(int messageQueueId) {
    if (msgctl(messageQueueId, IPC_RMID, NULL) == -1) {
        write(STDERR_FILENO, "Error on deleting message queue", 32);
        exit(1);
    }
}

int awaitForClientSocket(int serverSocket) {
    int clientSocket;
    InternetSocketAddress clientAddress;
    clientAddress.sin_family = AF_INET;
    socklen_t clientAddressLength = sizeof(clientAddress);
    if ((clientSocket = accept(serverSocket, (AbstractSocketAddress*) &clientAddress, &clientAddressLength)) == -1) {
        write(STDERR_FILENO, "Error occured on accepting upcoming connections", 48);
        exit(1);
    }

    return clientSocket;
}

void startGame(int *gameStartedFlag, MainData *data, int clientSocket) {
    clearData(data);

    strcpy(data->message, "The game was started!\n");

    for (int i = 0; i < FIELD_SIZE; i++) {
        for (int j = 0; j < FIELD_SIZE; j++) {
            data->field.orig[i][j] = '1' + j;
        }
    }

    createWorkerToGenerateField(data, clientSocket);
    
    *gameStartedFlag = 1;
}

void createWorkerToGenerateField(MainData *data, int clientSocket) {
    pid_t childPid = fork();
    
    if (childPid) {
    //master process
        waitpid(childPid, NULL, 0);
        captureSemaphore(semaphoreId);
        
        MainData* ptr;
        connectSharedMemory(&ptr, sharedMemoryId);
        for (int i = 0; i < FIELD_SIZE; i++) {
            for (int j = 0; j < FIELD_SIZE; j++) {
                data->field.orig[i][j] = ptr->field.orig[i][j];
            }
        }

        freeSemaphore(semaphoreId);
    } else {
    //worker process

        captureSemaphore(semaphoreId);
        
        MainData* ptr;
        connectSharedMemory(&ptr, sharedMemoryId);
        Coords removed;
        do {
            removed = generateNewField();
        } while (ptr->field.mask[removed.x][removed.y] == ' ');
        ptr->field.mask[removed.x][removed.y] = ' ';

        freeSemaphore(semaphoreId);

        close(clientSocket);
        close(serverSocket);
        exit(0);
    }
}

void clearMessage(char message[MESSAGE_LENGTH]) {
    for (int i = 0; i < MESSAGE_LENGTH; i++) {
        message[i] = '\0';
    }
}