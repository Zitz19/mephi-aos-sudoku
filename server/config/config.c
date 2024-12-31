#include "config.h"

int parseConfig(char* filename, char* loggingFilename, int* port) {
    FILE* fd;
    if ((fd = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Cannot open the file %s\n", filename);
        return 1;
    }

    if (fscanf(fd, "log.filename=%s\nserver.port=%d", loggingFilename, port) != 2) {
        fprintf(stderr, "Cannot read properties from the file %s\n", filename);
        return 1;
    }

    fclose(fd);

    char timestamp[32];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    if (tm == NULL) {
        perror("Error on getting local time\n");
        exit(1);
    }
    if (strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S.txt", tm) == 0) {
        fprintf(stderr, "Error on formatting timestamp\n");
        exit(1);
    }
    strcat(loggingFilename, timestamp);
}