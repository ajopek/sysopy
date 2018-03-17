#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "string_array_lib.h"

#define _STATIC
#define CREATE_NUMBER 1000

typedef struct timespec timespec;
typedef struct timeval timeval;
typedef struct rusage rusage;
typedef struct timestamp timestamp;

struct timestamp{
    long real;
    long user;
    long system;
};

// Loading libraries -------------------------
// Testing functions -------------------------
string_array* create(int size, int block_size);
string_array* create_with_content(int size, int block_size);
void add_and_delete(string_array* array_struct, int number_of_blocks);
void search_element(string_array* array_struct, int value);
void add(string_array* array_struct, int number_of_blocks);
void delete(string_array* array_struct, int number_of_blocks);
// Time measurement --------------------------
timestamp get_timestamp();
void print_timestamp(FILE* file,timestamp timestamp);
long get_time(timeval time);
void print_time(FILE* file, timestamp start, timestamp end, char* description, int number);
timestamp stamp_diff(timestamp start, timestamp end);
// File handling -----------------------------
FILE* open_file(char* name);
// Helper functions --------------------------
void random_string(int length, char* string);
void fill_random_content(string_array* array);
void print_help();
// -------------------------------------------

int main(int argc, char* argv[]) {
    srand((unsigned int)time(0));
    string_array* array_with_content = NULL;
    string_array* array = NULL;
    /** ------------------------------------------------------
     *  Program arguments handling
     * -------------------------------------------------------
     */
    int i;
    int argv_i = 1;
    int created = 0;
    int num_arg;
    if(argc <= 3){
        print_help();
        return 0;
    }
    FILE* file = open_file("raport.txt");
    for (i = 0; i < argc && i < 3; ++i) {
        timestamp start, end;
        if(strcmp("create_array", argv[argv_i]) == 0 && !created){
            int size = atoi(argv[++argv_i]);
            int block_size = atoi(argv[++argv_i]);

            start = get_timestamp();
            array_with_content = create_with_content(size, block_size);
            end = get_timestamp();
            print_time(file, start, end, "Creating with random content", CREATE_NUMBER);

            start = get_timestamp();
            array = create(size, block_size);
            end = get_timestamp();
            print_time(file, start, end, "Creating empty", CREATE_NUMBER);

            if(array == NULL || array_with_content == NULL){
                fclose(file);
                exit(1);
            }

            created = 1;
        } else if(strcmp("add", argv[argv_i]) == 0 && created) {
            num_arg = atoi(argv[++argv_i]);

            start = get_timestamp();
            add(array, num_arg);
            end = get_timestamp();
            print_time(file, start, end, "Adding block", num_arg);

        } else if(strcmp("delete", argv[argv_i]) == 0 && created) {
            num_arg = atoi(argv[++argv_i]);

            start = get_timestamp();
            delete(array_with_content, num_arg);
            end = get_timestamp();
            print_time(file, start, end, "Deleting block", num_arg);

        } else if(strcmp("search", argv[argv_i]) == 0 && created) {
            num_arg = atoi(argv[++argv_i]);

            start = get_timestamp();
            search_element(array_with_content, num_arg);
            end = get_timestamp();
            print_time(file, start, end, "Search for element", num_arg);

        } else if(strcmp("add_and_delete", argv[argv_i]) == 0 && created) {
            num_arg = atoi(argv[++argv_i]);

            start = get_timestamp();
            add_and_delete(array, num_arg);
            end = get_timestamp();
            print_time(file, start, end, "Adding and deleting block", num_arg);

        } else {
            print_help();
            break;
        }
        argv_i ++;
    }
    fclose(file);
    if(array != NULL) delete_array(array);
    if(array_with_content != NULL) delete_array(array_with_content);
    return 0;
}

/** --------------------------------------------------------------------------------------
 * Testing functions
 * ---------------------------------------------------------------------------------------
 */

#ifdef _STATIC

string_array* create(int size, int block_size){
    printf("Creating static array \n");
    string_array* result = (string_array *) malloc(sizeof(string_array));
    init(STATIC, 0, block_size, &result);
    return result;
}
#endif
#ifdef _DYNAMIC

string_array* create_table(int size, int block_size) {
    printf("Creating dynamic array \n");
    string_array* result = (string_array *) malloc(sizeof(string_array));
    if(init(DYNAMIC, (size_t) size, &result)){
        printf("Allocating dynamic array failed");
        return NULL;
    }
    return result;
 }

#endif

string_array* create_with_content(int size, int block_size){
    //printf("Creating static array with content\n");
    string_array* result = create(size, block_size);
    fill_random_content(result);
    return result;
}

void search_element(string_array* array_struct, int value) {
    printf("Finding nearest \n");
    find_nearest(array_struct, value);
};

void add_and_delete(string_array* array_struct, int number_of_blocks) {
    int i;
    //printf("Adding and deleting random %i blocks \n", number_of_blocks);
    char *string = malloc((array_struct -> block_size) * sizeof(char));
    for (i = 0; i < number_of_blocks ; ++i) {
        int index = rand() % (int) array_struct->size;
        random_string(array_struct -> block_size, string);
        add_block(array_struct, index, string);
        delete_block(array_struct, index);
    }
    free(string);
};

void add(string_array* array_struct, int number_of_blocks){
    int i;
    //printf("Adding random %i blocks \n", number_of_blocks);
    char *string = malloc((array_struct -> block_size) * sizeof(char));
    for (i = 0; i < number_of_blocks ; ++i) {
        int index = rand() % (int) array_struct->size;
        random_string(array_struct -> block_size, string);
        add_block(array_struct, index, string);
    }
    free(string);
}

void delete(string_array* array_struct, int number_of_blocks){
    int i;
    //printf("Delete random %i blocks \n", number_of_blocks);
    for (i = 0; i < number_of_blocks ; ++i) {
        int index = rand() % (int) array_struct->size;
        delete_block(array_struct, index);
    }
}

/** ---------------------------------------------------------------------------------------
 * Time measurement
 * ---------------------------------------------------------------------------------------
 */

timestamp get_timestamp() {
    timeval time;
    rusage usage;
    clock_gettime(CLOCK_REALTIME, &time);
    getrusage(RUSAGE_SELF, &usage);
    timestamp result = {get_time(time), get_time(usage.ru_utime), get_time(usage.ru_stime)};
}

void print_timestamp(FILE* file, timestamp to_print) {
    fprintf(file, "Real time: %li \n", to_print.real);
    fprintf(file, "User time: %li \n", to_print.user);
    fprintf(file, "System time: %li \n", to_print.system);
}


long get_time(timeval time) {
    return (long)time.tv_sec * 1000000L + (long)time.tv_usec;
}

timestamp stamp_diff(timestamp start, timestamp end) {
    timestamp diff = {end.real - start.real,
                      end.user - start.user,
                      end.system - start.system};
    return diff;
}

void print_time(FILE* file, timestamp start, timestamp end, char* description, int number) {
    timestamp diff = stamp_diff(start,end);
    fprintf(file, "Time of %i %s \n", number, description);
    print_timestamp(file, diff);
}

/** ---------------------------------------------------------------------------------------
 * File handling
 * ---------------------------------------------------------------------------------------
 */

FILE* open_file(char* name){
    FILE *f = fopen(name, "w");
    if(f == NULL) {
        printf("File could not be opened");
        exit(1);
    }
    return f;
}

/** ---------------------------------------------------------------------------------------
 * Helper functions
 * ---------------------------------------------------------------------------------------
 */

void random_string(int length, char* string) {
    const char symbols[] = "abcdefghijklmnoprstqwxyz0123456789+-.,/";
    const int symbols_len = sizeof symbols -1;
    int i;
    for (i = 0; i < (length - 1); ++i) {
        string[i] = symbols[rand()%symbols_len];
    }
    string[i] = '\0';
}

void fill_random_content(string_array* array) {
    int i;
    char *string = malloc((array -> block_size) * sizeof(char));
    for(i = 0; i < array -> size; ++i) {
        random_string(array -> block_size, string);
        add_block(array, i, string);
    }
    free(string);
};

void print_help(){
    printf("Usage of program: \n"
                   "Program will take 3 arguments, which specify operations to test. \n"
                   "Avaliable operations: \n"
                   "create_table size block_size - needs to be the first operation \n"
                   "add number - add number of random blocks \n"
                   "delete number - delete number of random blocks \n"
                   "search value - search for block with nearest ascii sum to value \n"
                   "add_and_delete number - add and delete number of random blocks");
}
