#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "string_array_lib.h"

#define _STATIC

// Loading libraries -------------------------
// Testing functions -------------------------
string_array* create(int size, int block_size);
string_array* create_with_content(int size, int block_size);
void add_and_delete(string_array* array_struct, int number_of_blocks);
void search_element(string_array* array_struct, int value);
// Time measurement --------------------------
// Helper functions --------------------------
void random_string(int length, char* string);
void fill_random_content(string_array* array);
// -------------------------------------------

int main(int argc, char* argv[]) {



    srand((unsigned int)time(0));
    string_array* array = create_with_content(12, 10);
    search_element(array, 5);
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
    }
    return result;
 }

#endif

string_array* create_with_content(int size, int block_size){
    printf("Creating static array with content\n");
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
    printf("Adding and deleting random %i blocks \n", number_of_blocks);
    char *string = malloc((array_struct -> block_size) * sizeof(char));
    for (i = 0; i < number_of_blocks ; ++i) {
        int index = rand() % (int) array_struct->size;
        random_string(array_struct -> block_size, string);
        add_block(array_struct, index, string);
        delete_block(array_struct, index);
    }
    free(string);
};


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
