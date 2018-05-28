#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <memory.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <zconf.h>
#include <bits/signum.h>
#include <signal.h>


#define RED_COLOR "\e[1;31m"
#define RESET_COLOR "\e[0m"
#define PRINT_EXIT(format, ...) {                                            \
    printf(RED_COLOR format "\n" RESET_COLOR, ##__VA_ARGS__);                \
    exit(EXIT_FAILURE);}
#define FULL 1

char** buffer = NULL;
int N = 0;
int P = 0;
int K = 0;
int L = 0;
int last_write = 0;
int last_read = 0;
int search_mode = 1;
int lines_in_buff = 0;

int raport_mode;

FILE* product_src;

pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty_buffer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t full_buffer_cond = PTHREAD_COND_INITIALIZER;

void* consumer(void) {
    char* read_line;
    while(1) {
        pthread_mutex_lock(&buffer_mutex);
        while(lines_in_buff == 0){
            pthread_cond_wait(&empty_buffer_cond, &buffer_mutex);
        }

        last_read = (last_read + 1) % N;
        read_line = buffer[last_read];


        lines_in_buff--;
        if(lines_in_buff == N -1) pthread_cond_broadcast(&full_buffer_cond);

        if(raport_mode == FULL) {
            printf("Consumer gets line from %i, is the %i line in buff. \n", last_read, lines_in_buff);
        }

        if (read_line[strlen(read_line)-1] == '\n') read_line[strlen(read_line)-1] = '\0';
        int flag;
        switch (search_mode)
        {
            case 0:    flag = (strlen(read_line) == L); break;
            case 1:  flag = (strlen(read_line) > L);  break;
            case -1:     flag = (strlen(read_line) < L);  break;
        }
        if(flag) printf("Index: %i, line: %s \n", last_read, read_line);
        free(read_line);
        pthread_mutex_unlock(&buffer_mutex);

    }
}

void* producer(void){

    char* read_line = NULL;
    size_t line_size = 0;
    while(1) {
        pthread_mutex_lock(&buffer_mutex);
        while(lines_in_buff == N) {
            pthread_cond_wait(&full_buffer_cond, &buffer_mutex);
        }

        if(getline(&read_line, &line_size, product_src) <= 0) {
            pthread_mutex_unlock(&buffer_mutex);
            break;
        }
        last_write = (last_write+1)%N;
        buffer[last_write] = malloc(line_size * sizeof(char));
        strcpy(buffer[last_write], read_line);
        lines_in_buff++;
        if(raport_mode == FULL) {
            printf("Producer puts line at %i, of length %i, is the %i line in buff. \n", last_write, (int)line_size, lines_in_buff);
        }
        if(lines_in_buff == 1) pthread_cond_broadcast(&empty_buffer_cond);
        pthread_mutex_unlock(&buffer_mutex);
    }
}

void clean_up() {
    fclose(product_src);
    free(buffer);
    pthread_mutex_destroy(&buffer_mutex);
    pthread_cond_destroy(&empty_buffer_cond);
    pthread_cond_destroy(&full_buffer_cond);
}

void sigexit(int sig) {PRINT_EXIT("Signal exit. \n")};

int main(int argc, char* argv[]) {
    int i;
    atexit(&clean_up);
    signal(SIGINT, sigexit);
    if(argc < 9) PRINT_EXIT("Provide buffer size, num of producers, num of consumers, length, search_mode, file name, nk, report mode.\n");
    N = atoi(argv[1]);
    buffer = malloc(sizeof(char*) * N);

    for(i = 0; i < N; i++) {
        buffer[i] = NULL;
    }
    P = atoi(argv[2]);
    K = atoi(argv[3]);
    L = atoi(argv[4]);
    search_mode = atoi(argv[5]);
    char* filename = argv[6];
    if ((product_src = fopen(filename, "r")) < 0) PRINT_EXIT("Conf file open \n");

    int nk = atoi(argv[7]);
    if(nk) alarm(nk);

    raport_mode = atoi(argv[8]);

    pthread_t prods[P];
    pthread_t cons[K];

    for(i = 0; i < P; ++i) {
        pthread_create(&prods[i], NULL , &producer, NULL);
    }

    for(i = 0; i < K; ++i) {
        pthread_create(&cons[i], NULL , &consumer, NULL);
    }

    for(i = 0; i < P; i++) pthread_join(prods[i], NULL);

    while (1)
    {
        pthread_mutex_lock(&buffer_mutex);
        if (lines_in_buff == 0) break;
        pthread_mutex_unlock(&buffer_mutex);
    }

    return 0;
}
