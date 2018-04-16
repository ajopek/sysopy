#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <string.h>
#include <errno.h>
#include <wait.h>

#define INITIAL_BUFFER_SIZE 100
#define MAX_ARGS 10
#define MAX_CMDS 10

// Commands execution --------------------
int exec_command(char *const arguments[]);
int execute_line(FILE* file);
// Commands parsing ----------------------
int tokenize_args(char* line, char*** args_buffer, int args_size);
// Files handling ------------------------
FILE* open_file(char* path);
int read_line(FILE* file, char** result);
// Helpers -------------------------------
void print_help();

int main(int argc, char* argv[]) {
    if(argc < 2) {
        print_help();
        exit(EXIT_FAILURE);
    }
    char* file_path = argv[1];
    FILE* commands_file = open_file(file_path);
    while(!execute_line(commands_file));
    fclose(commands_file);
    return 0;
}

/** ---------------------------------------------------------------------------------------
 * Commands execution
 * ---------------------------------------------------------------------------------------
 */

int execute_line(FILE* file) {
    int eof = 0;
    int commands_cnt = 0;
    int pipes[2][2];
    char* commands[MAX_CMDS];

    char* line = malloc(sizeof(char*) * INITIAL_BUFFER_SIZE);
    if(read_line(file, &line)) eof = 1;

    while((commands[commands_cnt] = strtok(commands_cnt == 0 ? line : NULL, "|")) != NULL) {
        ++commands_cnt;
    }

    char** args_buffer = malloc(sizeof(char*) * MAX_ARGS);
    if(args_buffer == NULL) return 1;

    // Execute commands
    int i;
    for (int i = 0; i < commands_cnt; ++i) {

        if (i > 0) {
            close(pipes[i % 2][0]);
            close(pipes[i % 2][1]);
        }

        if(pipe(pipes[i % 2]) == -1) {
            printf("Error on pipe.\n");
            exit(EXIT_FAILURE);
        }

        pid_t fork_res = fork();

        if (fork_res == 0) {

            if(tokenize_args(commands[i], &args_buffer, INITIAL_BUFFER_SIZE)) return 1;

            if ( i  !=  commands_cnt - 1) {
                close(pipes[i % 2][0]);
                if (dup2(pipes[i % 2][1], STDOUT_FILENO) < 0) {
                    exit(EXIT_FAILURE);
                };
            }
            if (i != 0) {
                close(pipes[(i + 1) % 2][1]);
                if (dup2(pipes[(i + 1) % 2][0], STDIN_FILENO) < 0) {
                    close(EXIT_FAILURE);
                }
            }
            execvp(args_buffer[0], args_buffer);

            exit(EXIT_SUCCESS);
        }
    }

    free(args_buffer);
    free(line);

    if(eof) return 1;
    return 0;
}



int exec_command(char *const arguments[]) {
    pid_t pid = fork();
    int status;
    if(pid != 0) {
        do{
            pid_t wpid = waitpid(pid, &status, WUNTRACED);
        } while(!WIFEXITED(status) && !WIFSIGNALED(status));
        if(status != 0) {
            printf("Executing command %s", arguments[0]);
            int i = 0;
            if(arguments[0] != NULL) printf(", with arguments ");
            while(arguments[i] != NULL) {
                printf("%s ", arguments[i]);
                i++;
            }
            printf(" failed with status %i. \n", status);
            return 1;
        }
        return 0;
    }
    execvp(arguments[0], arguments);
    return 1;
}

/** ---------------------------------------------------------------------------------------
 * Commands parsing
 * ---------------------------------------------------------------------------------------
 */

int tokenize_args(char* line, char*** args_buffer, int args_size) {
    int read_args = 0;
    if(strlen(line) == 0) return 1;
    (*args_buffer)[0] = strtok(line, " ");
    char* buffer;
    read_args++;
    while((buffer = strtok(NULL, " ")) != NULL) {
        // too small args buffer

        (*args_buffer)[read_args] = buffer;
        strcpy((*args_buffer)[read_args], buffer);

        read_args++;
    }
    (*args_buffer)[read_args] = NULL;
    return 0;
}

/** ---------------------------------------------------------------------------------------
 * File handling
 * ---------------------------------------------------------------------------------------
 */

FILE* open_file(char* path){
    FILE *f = fopen(path, "r");
    if(f == NULL) {
        printf("File could not be opened");
        exit(EXIT_FAILURE);
    }
    return f;
}

int read_line(FILE* file, char** result) {
    int i = 0;
    while(((*result)[i] = fgetc(file)) != '\n'){
        if((*result)[i] == EOF) {
            return 1;
        }
        i++;
    }
    (*result)[i] = '\0';
    return 0;
}

/** ---------------------------------------------------------------------------------------
 * File handling
 * ---------------------------------------------------------------------------------------
 */
void print_help() {
    return;
}