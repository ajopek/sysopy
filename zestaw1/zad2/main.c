#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "string_array_lib.h"
#include "dlfcn.h"

#define NUMBER_OF_TESTS 10000
#define FILE_NAME "raport.txt"
#define DL_NAME "libzad1_shared.so"

typedef struct timespec timespec;
typedef struct timeval timeval;
typedef struct rusage rusage;
typedef enum array_type array_type;

typedef struct timestamp{
    long real;
    long user;
    long system;
} timestamp;

typedef struct functions {

    int (* init)(enum array_type, size_t, int, string_array**);

    void (* delete_array)(string_array*);

    void (*add_block)(string_array*, int, char *);

    void (* delete_block)(string_array*, int);

    int (* find_nearest)(string_array*, int);
} functions;

//TODO: Handle possible array bigger than memory

// Testing functions -------------------------
string_array* create(array_type alloc_type, int size, int block_size, FILE* file, functions);
string_array* create_with_content(array_type type, int size, int block_size, functions);
void add_and_delete(string_array* array_struct, int number_of_blocks, FILE* file, functions);
void search_element(string_array* array_struct, int value, FILE* file, functions);
void add_then_delete(string_array* array_struct, int number_of_blocks, FILE* file, functions);
// Time measurement --------------------------
timestamp get_timestamp();
void print_timestamp(FILE* file,timestamp timestamp);
long get_time(timeval time);
void print_time(FILE* file, timestamp start, timestamp end, char* description, int number);
timestamp stamp_diff(timestamp start, timestamp end);
// File handling -----------------------------
FILE* open_file();
void add_testcase_description(FILE* file, int size, int block_size, array_type alloc_type);
void add_testcase_ending(FILE* file);
// Helper functions --------------------------
void random_string(int length, char* string);
void fill_random_content(string_array* array, functions);
void print_help();
// -------------------------------------------


int main(int argc, char* argv[]) {
    functions functions;
#ifdef DYNAMIC_LOADING
    void *handle = dlopen(DL_NAME, RTLD_LAZY);
    if(!handle) {
        fprintf(stderr, "Could not open library %s: %s\n", DL_NAME, dlerror());
        return(1);
    }
    
    functions.init = dlsym(handle, "init");

    functions.delete_array = dlsym(handle, "delete_array");

    functions.add_block = dlsym(handle, "add_block");

    functions.find_nearest = dlsym(handle, "find_nearest");

    functions.delete_block = dlsym(handle, "delete_block");

#else
    functions.init = &init;

    functions.delete_array = &delete_array;

    functions.add_block = &add_block;

    functions.find_nearest = &find_nearest;

    functions.delete_block = &delete_block;
#endif

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
    if(argc <= 4){
        print_help();
        return 0;
    }
    FILE* file = open_file(FILE_NAME);
    array_type alloc_type;
    int knownt_type = 0;
    for (i = 0; i < argc && i < 5; ++i) {
        if(strcmp("static", argv[argv_i]) == 0 && !created) {
            alloc_type = STATIC;
            knownt_type = 1;

        } else if(strcmp("dynamic", argv[argv_i]) == 0 && !created) {
            alloc_type = DYNAMIC;
            knownt_type = 1;

        } else if(strcmp("create_array", argv[argv_i]) == 0 && !created && knownt_type){
            int size = atoi(argv[++argv_i]);
            int block_size = atoi(argv[++argv_i]);

            add_testcase_description(file, size, block_size, alloc_type);

            array = create(alloc_type, size, block_size, file, functions);
            array_with_content = create_with_content(alloc_type, size, block_size, functions);

            if(array == NULL || array_with_content == NULL){
                fclose(file);
                exit(1);
            }

            created = 1;
        } else if(strcmp("add_then_delete", argv[argv_i]) == 0 && created) {
            num_arg = atoi(argv[++argv_i]);

            add_then_delete(array, num_arg, file, functions);


        } else if(strcmp("search", argv[argv_i]) == 0 && created) {
            num_arg = atoi(argv[++argv_i]);

            search_element(array_with_content, num_arg, file, functions);

        } else if(strcmp("add_and_delete", argv[argv_i]) == 0 && created) {
            num_arg = atoi(argv[++argv_i]);

            add_and_delete(array, num_arg, file, functions);

        } else {
            print_help();
            break;
        }
        argv_i ++;
    }
    add_testcase_ending(file);
    fclose(file);
    if(array != NULL) functions.delete_array(array);
    if(array_with_content != NULL) functions.delete_array(array_with_content);

    #ifdef DYNAMIC_LOADING
        dlclose(handle);
    #endif
    return 0;
}

/** --------------------------------------------------------------------------------------
 * Testing functions
 * ---------------------------------------------------------------------------------------
 */

string_array* create(array_type alloc_type, int size, int block_size, FILE* file, functions functions) {
    //printf("Creating static array \n");
    int i;

    string_array** created_arrays = malloc(sizeof(string_array*) * NUMBER_OF_TESTS);
    timestamp start = get_timestamp();

    for (i = 0; i < NUMBER_OF_TESTS; ++i) {
        created_arrays[i] = malloc(sizeof(string_array));
        if(functions.init(alloc_type, 0, block_size, &created_arrays[i])) {
            int j;
            for (j = 0; j < i ; ++i) {
                functions.delete_array(created_arrays[j]);
            }
            return NULL;
        }
    }
    timestamp end = get_timestamp();
    print_time(file, start, end, "creating array" , NUMBER_OF_TESTS);

    string_array* result = created_arrays[0];
    for (i = 1; i < NUMBER_OF_TESTS; ++i) {
        functions.delete_array(created_arrays[i]);
    }
    free(created_arrays);
    return result;
}

string_array* create_with_content(array_type array_type,int size, int block_size, functions functions){
    //printf("Creating static array with content\n");
    string_array* result = malloc(sizeof(string_array));
    if(functions.init(array_type, size, block_size, &result)) {
        return NULL;
    }
    fill_random_content(result, functions);
    return result;
}

void search_element(string_array* array_struct, int value, FILE* file, functions functions) {
    //printf("Finding nearest \n");
    int i;
    timestamp start = get_timestamp();
    for(i = 0; i < NUMBER_OF_TESTS; ++i){
        functions.find_nearest(array_struct, value);
    }
    timestamp end = get_timestamp();
    print_time(file, start, end, "find nearest element", NUMBER_OF_TESTS);
};

void add_and_delete(string_array* array_struct, int number_of_blocks, FILE* file, functions functions) {
    int i;
    //printf("Adding and deleting random %i blocks \n", number_of_blocks);
    char *string = malloc((array_struct -> block_size) * sizeof(char));
    timestamp start = get_timestamp();
    for (i = 0; i < number_of_blocks ; ++i) {
        int index = i;
        random_string(array_struct -> block_size, string);
        functions.add_block(array_struct, index, string);
        functions.delete_block(array_struct, index);
    }
    timestamp end = get_timestamp();
    print_time(file, start, end, "add and delete", number_of_blocks);
    free(string);
};

void add_then_delete(string_array* array_struct, int number_of_blocks, FILE* file, functions functions) {
    int i;
    //printf("Adding random %i blocks \n", number_of_blocks);
    char *string = malloc((array_struct -> block_size) * sizeof(char));
    timestamp start = get_timestamp();
    for (i = 0; i < number_of_blocks ; ++i) {
        int index = i;
        random_string(array_struct -> block_size, string);
        functions.add_block(array_struct, index, string);
    }

    for (i = 0; i < number_of_blocks ; ++i) {
        int index = i;
        functions.delete_block(array_struct, index);
    }

    timestamp end = get_timestamp();
    print_time(file, start, end, "add then delete", number_of_blocks);
    free(string);
}

/** ---------------------------------------------------------------------------------------
 * Time measurement
 * ---------------------------------------------------------------------------------------
 */

timestamp get_timestamp() {
    timeval time;
    rusage usage;
    gettimeofday(&time, 0);
    getrusage(RUSAGE_SELF, &usage);
    timestamp result = {get_time(time), get_time(usage.ru_utime), get_time(usage.ru_stime)};
    return result;
}

void print_timestamp(FILE* file, timestamp to_print) {
    fprintf(file, "Real time: %li \n", to_print.real);
    fprintf(file, "User time: %li \n", to_print.user);
    fprintf(file, "System time: %li \n", to_print.system);
    fprintf(file, "\n");
    printf("Real time: %li \n", to_print.real);
    printf("User time: %li \n", to_print.user);
    printf("System time: %li \n", to_print.system);
    printf("\n");
}


long get_time(timeval time) {
    return (long)time.tv_sec * 1000000 + (long)time.tv_usec;
}

timestamp stamp_diff(timestamp start, timestamp end) {
    timestamp diff = {end.real - start.real,
                      end.user - start.user,
                      end.system - start.system};
    return diff;
}

void print_time(FILE* file, timestamp start, timestamp end, char* description, int number) {
    timestamp diff = stamp_diff(start, end);
    fprintf(file, "Time of %s, %i times \n", description, number);
    printf("Time of %s, %i times \n", description, number);
    print_timestamp(file, diff);
    fprintf(file, "\n");
    printf("\n");
}

/** ---------------------------------------------------------------------------------------
 * File handling
 * ---------------------------------------------------------------------------------------
 */

FILE* open_file(){
    FILE *f = fopen(FILE_NAME, "a");
    if(f == NULL) {
        printf("File could not be opened");
        exit(1);
    }
    return f;
}

void add_testcase_description(FILE* file, int size, int block_size, array_type alloc_type) {
    char* alloc_type_string = alloc_type == STATIC ? "Static" : "Dynamic";
    fprintf(file, "Array size: %i, Block size: %i, Array allocation type: %s \n", size, block_size, alloc_type_string);
    printf("Array size: %i, Block size: %i, Array allocation type: %s \n", size, block_size, alloc_type_string);
    printf("{ \n \n");
    fprintf(file, "{ \n \n");
}

void add_testcase_ending(FILE* file){
    printf("} \n");
    fprintf(file, "} \n");
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

void fill_random_content(string_array* array, functions functions) {
    int i;
    char *string = malloc((array -> block_size) * sizeof(char));
    for(i = 0; i < array -> size; ++i) {
        random_string(array -> block_size, string);
        functions.add_block(array, i, string);
    }
    free(string);
};

void print_help(){
    printf("Usage of program: \n"
                   "Program will take 4 arguments, which specify operations to test. \n"
                   "Avaliable operations: \n"
                   "static | dynamic - set the type of used array, needst to be first argument\n"
                   "create_table size block_size - needs to be the second argument \n"
                   "add_then_delete number - add number of random blocks then delete them \n"
                   "search value - search for block with nearest ascii sum to value \n"
                   "add_and_delete number - add and delete number of random blocks");
}