#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <string.h>
#include <errno.h>

#define INITIAL_BUFFER_SIZE 200

// Commands execution --------------------
void exec_command(char *const arguments[]);
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
        exit(EXIT_SUCCESS);
    }
    char* file_path = argv[1];
    FILE* commands_file = open_file(file_path);
    execute_line(commands_file);


    fclose(commands_file);
    return 0;
}

/** ---------------------------------------------------------------------------------------
 * Commands execution
 * ---------------------------------------------------------------------------------------
 */

int execute_line(FILE* file) {
    char** args_buffer = malloc(sizeof(char) * INITIAL_BUFFER_SIZE);
    if(args_buffer == NULL) return 1;
    char* line = malloc(sizeof(char) * INITIAL_BUFFER_SIZE);
    if(read_line(file, &line)) return 1;
    if(tokenize_args(line, &args_buffer, INITIAL_BUFFER_SIZE)) return 1;
    exec_command(args_buffer);
    free(args_buffer);
    free(line);
    return 0;
}



void exec_command(char *const arguments[]) {
    pid_t pid = fork();
    if(pid != 0) return; //parent
    execvp(arguments[0], arguments);

    // Something went wrong - execvp returned
    printf("Executing command %s", arguments[0]);
    int i = 0;
    if(arguments[0] != NULL) printf(", with arguments ");
    while(arguments[i] != NULL) {
        printf("%s ", arguments[i]);
        i++;
    }
    printf(" failed. \n");
    printf("%s. \n", strerror(errno));
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
        while(++read_args >= args_size) {
            args_size *= 2;
            (*args_buffer) = realloc((*args_buffer), sizeof(char) * args_size);
            if(*args_buffer == NULL) {
                printf("Realloc failed.");
                return 1;
            }
        }
        (*args_buffer)[read_args] = buffer;
        read_args++;
    }
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
    fgets(*result, INITIAL_BUFFER_SIZE, file);
    *result[strlen(*result) - 2] = '\0';
    return 0;
}

/** ---------------------------------------------------------------------------------------
 * File handling
 * ---------------------------------------------------------------------------------------
 */
void print_help() {
    return;
}