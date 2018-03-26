#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <string.h>
#include <errno.h>
#include <wait.h>
#include <sys/resource.h>
#include <sys/time.h>

#define INITIAL_BUFFER_SIZE 100
#define MAX_ARGS 10

// Commands execution --------------------
int exec_command(char *const arguments[], int hard_cpu_time, int hard_memory_size);
int execute_line(FILE* file, int hard_cpu_time, int hard_memory_size);
// Process monitoring --------------------
void monitor_child(pid_t pid, int* status);
void print_usage_info(struct rusage* usage);
// Commands parsing ----------------------
int tokenize_args(char* line, char*** args_buffer);
// Files handling ------------------------
FILE* open_file(char* path);
int read_line(FILE* file, char** result);
// Helpers -------------------------------
void print_help();

int main(int argc, char* argv[]) {
    if(argc < 4) {
        print_help();
        exit(EXIT_FAILURE);
    }
    int hard_cpu_time, hard_memory_size;
    char* file_path = argv[1];
    hard_cpu_time = atoi(argv[2]);
    hard_memory_size = atoi(argv[3]);
    FILE* commands_file = open_file(file_path);
    while(!execute_line(commands_file, hard_cpu_time, hard_memory_size));
    fclose(commands_file);
    return 0;
}

/** ---------------------------------------------------------------------------------------
 * Commands execution
 * ---------------------------------------------------------------------------------------
 */

int execute_line(FILE* file, int hard_cpu_time, int hard_memory_size) {
    int eof = 0;
    char** args_buffer = malloc(sizeof(char*) * MAX_ARGS);
    if(args_buffer == NULL) return 1;
    char* line = malloc(sizeof(char*) * INITIAL_BUFFER_SIZE);
    if(read_line(file, &line)) eof = 1;
    if(tokenize_args(line, &args_buffer)) return 1;
    if(exec_command(args_buffer, hard_cpu_time, hard_memory_size)) return 1;
    free(args_buffer);
    free(line);
    if(eof) return 1;
    return 0;
}



int exec_command(char *const arguments[], int hard_cpu_time, int hard_memory_size) {
    pid_t pid = fork();
    int status;
    if(pid > 0) {
        monitor_child(pid, &status);
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
    } else if (pid < 0){
        printf("Fork failed.");
        return 1;
    } else {
        struct rlimit limit;
        limit.rlim_max = (rlim_t) hard_memory_size;
        limit.rlim_cur = (rlim_t) hard_memory_size;
        if(setrlimit(RLIMIT_AS, &limit)) {
            printf("Setting memory limit failed: \n%s \n", strerror(errno));
            return 1;
        }
        limit.rlim_max = (rlim_t)hard_cpu_time;
        limit.rlim_cur = (rlim_t)hard_cpu_time;
        if(setrlimit(RLIMIT_CPU, &limit)) {
            printf("Setting cpu time limit failed: \n%s \n", strerror(errno));
            return 1;
        }
        execvp(arguments[0], arguments);
    }

    return 1;
}
/** ---------------------------------------------------------------------------------------
 * Process monitoring
 * ---------------------------------------------------------------------------------------
 */

void monitor_child(pid_t pid, int* status) {
    struct rusage child_usage;
    do{
        pid_t wpid = waitpid(pid, status, WUNTRACED);
        if(getrusage(RUSAGE_CHILDREN, &child_usage)) {
            printf("Monitoring child process failed: \n %s", strerror(errno));
        } else {
            print_usage_info(&child_usage);
        }

    } while(!WIFEXITED(status) && !WIFSIGNALED(status));
}

void print_usage_info(struct rusage* usage) {
    printf("System time: %i s %li us \n User time: %i s %li us \n",
           (int)usage->ru_stime.tv_sec, (long)usage->ru_stime.tv_usec,
           (int)usage->ru_utime.tv_sec, (long)usage->ru_utime.tv_usec);
}


/** ---------------------------------------------------------------------------------------
 * Commands parsing
 * ---------------------------------------------------------------------------------------
 */

int tokenize_args(char* line, char*** args_buffer) {
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