#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <wait.h>

int main(int argc, char* argv[]) {
    if(argc < 3) {
        exit(EXIT_FAILURE);
    }

    char* args[4];
    //spawn master
    pid_t pid = fork();
    if(pid == 0) {
        args[0] = "master";
        args[1] = argv[1];
        args[2] = NULL;
        execvp(args[0], args);
    } else if(pid < 0) {
        exit(EXIT_FAILURE);
    } else {
        int i = atoi(argv[2]);
        while(i) {
            pid = fork();
            if(pid == 0) {
                args[0] = "slave";
                args[2] = "10";
                args[3] = NULL;
                execvp(args[0], args);
            } else if(pid < 0) {
                exit(EXIT_FAILURE);
            }
            --i;
        }

    }

    wait(NULL);
    return 0;
}