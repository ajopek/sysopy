#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <zconf.h>
#include <memory.h>

#define BUFFER_LEN 256

int main(int argc, char* argv[]) {
    if(argc < 3) {
        exit(EXIT_FAILURE);
    }

    // open provided named pipe
    int pipe = open(argv[1], O_WRONLY);

    int iter_num = (int) atoi(argv[2]);


    int pid = (int)getpid();

    printf("Slave pid: %i \n", pid);


    char date_buffer[BUFFER_LEN];
    char output_buffer[BUFFER_LEN];

    int i;
    srand(time(NULL) + pid);
    for (i = 0; i < iter_num; ++i) {
        FILE* date_output = popen("date", "r");
        fgets(date_buffer, BUFFER_LEN, date_output);
        pclose(date_output);

        sprintf(output_buffer, "Slave pid: %i, date: %s \n", pid, date_buffer);
        write(pipe, output_buffer, strlen(output_buffer));
        sleep(rand()%4 + 1);
    }

    close(pipe);
    return 0;
}