#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <memory.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>


#define RED_COLOR "\e[1;31m"
#define RESET_COLOR "\e[0m"
#define PRINT_EXIT(format, ...) {                                            \
    printf(RED_COLOR format "\n" RESET_COLOR, ##__VA_ARGS__);                       \
    exit(EXIT_FAILURE);                                                                  }

int **picture = NULL;
int **filtered = NULL;
double **filter;
int w, h, c;
int thread_num = 1;

typedef struct interval{
    int begin;
    int end;
} interval_t;

typedef struct timestamp{
    long real;
    long user;
    long system;
} timestamp;


int max(int a, int b) {
    return a > b ? a : b;
}

int calculate_pixel(int x, int y) {
    int i,j;
    double value = 0;
    for(i = 0; i < c; ++i) {
        for(j = 0; j < c; ++j) {
            int a = max(1, x - (int)ceil(c/2) + i);
            int b = max(1, y - (int)ceil(c/2) + j);
            if(a < w && b < h) value += picture[a][b] * filter[i][j];
        }
    }
    return (int) round(value);
}

void apply_filter(interval_t *interval){
    int i,j;
    for (i = interval->begin; i < interval->end; ++i) {
        for (j = 0; j < h; ++j) {
            filtered[i][j] = calculate_pixel(i, j);
        }
    }
};

FILE* open_file(char* pathname, char* flags){
    FILE *f = fopen(pathname, flags);
    if(f == NULL) PRINT_EXIT("File could not be opened.");
    return f;
};

void close_file(FILE* file) {
    if(fclose(file)) PRINT_EXIT("Closing file failed.")
}

void load_picture(char* pathname){
    FILE* picture_file = open_file(pathname, "r");
    int i,j;
    char *header = malloc(sizeof(char) * 10);
    fscanf(picture_file, "%s", header);
    if(strcmp(header, "P2") != 0) {
        free(header);
        PRINT_EXIT("Wrong picture file format.");
    }
    fscanf(picture_file, "%i", &w);
    fscanf(picture_file, "%i", &h);
    picture = malloc(sizeof(int*) * w );
    for(i = 0; i < w; ++i) {
        picture[i] = malloc(sizeof(int) * h);
    }
    filtered = malloc(sizeof(int*) * w );
    for(i = 0; i < w; ++i) {
        filtered[i] = malloc(sizeof(int) * h);
    }
    for(i = 0; i < h; i++) {
        for(j = 0; j < w; j++){
            fscanf(picture_file, "%i", &picture[i][j]);
        }
    }
    free(header);
    close_file(picture_file);
};

void load_filter(char* pathname){
    int i,j;
    FILE* filter_file = open_file(pathname, "r");
    fscanf(filter_file, "%i", &c);
    filter = malloc(sizeof(double*) * c);
    for(i = 0; i < c; ++i) {
        filter[i] = malloc(sizeof(double) * c);
    }
    for(i = 0; i < c; i++) {
        for(j = 0; j < c; j++){
            fscanf(filter_file, "%lf", &filter[i][j]);
        }
    }
    close_file(filter_file);
};

void clean_up(void) {
    int i;
    if(picture != NULL) {
        for(i = 0; i < w; ++i) {
            free(picture[i]);
        }
    }
    if(filter != NULL) free(filter);

}

void write_picture(char* pathname) {
    int i, j;
    FILE* output = open_file(pathname, "w");
    fprintf(output, "P2\n%i %i\n 256 \n", w, h);
    for(i = 0; i < h; ++i) {
        for (j = 0; j < w; ++j) {
            fprintf(output, "%i ", filtered[i][j]);
        }
        fprintf(output, "\n");
    }
    fclose(output);
}

long get_time(struct timeval time) {
    return (long)time.tv_sec * 1000000 + (long)time.tv_usec;
}

timestamp get_timestamp() {
    struct timeval time;
    struct rusage usage;
    gettimeofday(&time, 0);
    getrusage(RUSAGE_SELF, &usage);
    timestamp result = {get_time(time), get_time(usage.ru_utime), get_time(usage.ru_stime)};
    return result;
}

timestamp stamp_diff(timestamp start, timestamp end) {
    timestamp diff = {end.real - start.real,
                      end.user - start.user,
                      end.system - start.system};
    return diff;
}

void print_timestamp(timestamp to_print) {
    printf("Real time: %li \n", to_print.real);
    printf("User time: %li \n", to_print.user);
    printf("System time: %li \n", to_print.system);
    printf("\n");
}

void print_time(timestamp start, timestamp end, char* description) {
    timestamp diff = stamp_diff(start, end);
    printf("Time of %s \n", description);
    print_timestamp(diff);
    printf("\n");
}

int main(int argc, char* argv[]) {
    if(argc < 5) PRINT_EXIT("Provide: num of threads, picture pathname, filter pathname, out pathname");
    int i,j;
    thread_num = atoi(argv[1]);
    atexit(&clean_up);
    load_picture(argv[2]);
    load_filter(argv[3]);
    int rows_per_thread = w/thread_num;
    int last_end = 0;
    timestamp timestamp1 = get_timestamp();

    pthread_t tid[thread_num];
    pthread_attr_t* attr = calloc(1, sizeof(pthread_attr_t));
    interval_t intervals[thread_num];
    for (i = 0; i < thread_num; ++i) {
        if(i == thread_num - 1 && rows_per_thread * thread_num < h) {
            rows_per_thread += h - (rows_per_thread * thread_num);
        }
        intervals[i].begin = last_end;
        intervals[i].end = last_end+rows_per_thread;
        last_end += rows_per_thread;
        pthread_attr_init(attr);
        pthread_create(tid + i, attr, &apply_filter, intervals + i);
        pthread_attr_destroy(attr);
    }

    int k;
    for (k = 0; k < thread_num ; ++k) {
        pthread_join(tid[k], NULL);
    }


    timestamp timestamp2 = get_timestamp();
    char* desc = malloc(sizeof(char) * 100);
    sprintf(desc, "\n thread num: %i\n picture: w: %i, h: %i \n filter: c: %i", thread_num, w, h, c);
    print_time(timestamp1, timestamp2, desc);
    write_picture(argv[4]);
    return 0;
}

