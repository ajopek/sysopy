#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <memory.h>

#define BUFFER_SIZE 256

int main(int argc, char* argv[]) {
    if(argc < 2) {
        exit(EXIT_FAILURE);
    }

    if(mkfifo(argv[1], S_IWUSR | S_IRUSR) == -1) {
        exit(EXIT_FAILURE);
    };

    char output_buffer[BUFFER_SIZE];
    FILE* pipe = fopen(argv[1], "r");
    sleep(1);
    while(fgets(output_buffer, BUFFER_SIZE, pipe) != NULL) {
        write(1, output_buffer, strlen(output_buffer));
    }

    fclose(pipe);
    return 1;
}