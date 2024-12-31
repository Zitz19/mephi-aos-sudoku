#include "client.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        write(STDERR_FILENO, "Incorrect arguments input! It should be ./client hostname port", 63);
        exit(1);
    }

    char* hostname = argv[1];
    int clientSocket, port = atoi(argv[2]);

    createAndConfigureSocket(&clientSocket, hostname, port);
    
    while (1) {
        printf("\nEnter the command:\n>> ");
        
        char body[COMMAND_LENGTH] = { 0 };

        //TODO normal input
        scanf("%s", body);
        
        Command command;
        strcpy(command.body, body);

        write(clientSocket, &command, sizeof(command));
    
        sleep(0.5);

        MainData currentData;
        read(clientSocket, &currentData, sizeof(currentData));
        if (strcmp(currentData.message, "") == 0) {
            printCombinedField(currentData.field.orig, currentData.field.mask);
        } else {
            printf("%s", currentData.message);
        }
    }

    close(clientSocket);
}

void createAndConfigureSocket(int *clientSocket, char *hostname, int port) {
    if ((*clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("Socket establishment");
        exit(1);
    }

    InternetSocketAddress serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);                                   //convert to internet port correctly
    if (inet_pton(AF_INET, hostname, &(serverAddress.sin_addr)) == -1) {    //convert to internet address and write into sin_addr 
        perror("Error on creating inernet address");
        exit(1);
    }

    if (connect(*clientSocket, (AbstractSocketAddress*)&serverAddress, (socklen_t) sizeof(serverAddress)) == -1) {
        perror("Error on connecting to the server");
        exit(1);
    }
}