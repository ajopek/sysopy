//
// Created by artur on 09.03.18.
//
#include <stdlib.h>
#include <string.h>
#include "string_array_lib.h"


#define STATIC_SIZE 1000

// Array used when user initializes a static array
char static_array[STATIC_SIZE][STATIC_SIZE];

/** ---------------------------------------------------------------------------------------
 * Library API
 * ---------------------------------------------------------------------------------------
 */


//TODO: Check malloc for 0 return

// Helpers declarations
// ----------------------------------------------------
void create_static(char*** array);

int create_dynamic(size_t size, char*** array);

int inside_array(string_array* array_struct, int index);

void clean_block(string_array* array_struct,int index);

int ascii_sum(string_array* array_struct, int index);
// ----------------------------------------------------

int init(enum array_type type, size_t size, int block_size, string_array** result) {
    size_t array_size;
    int create_result, i;
    // Initialize the proper array, base on type
    switch (type) {
        case STATIC:
            create_static(&((*result)->array));
            array_size = STATIC_SIZE;
            for(i = 0; i < STATIC_SIZE; i++) {
                (*result)->array[i] = "";
            }
            break;
        case DYNAMIC:
            create_result = create_dynamic(size, &((*result)->array));
            if(create_result) return 1;
            array_size = size;
            for(i = 0; i < size; i++) {
                (*result) -> array[i] = NULL;
            }
            break;
        default:
            return NULL;
    }
    // Rest of array properties
    (*result) -> size = array_size;
    (*result) -> type = type;
    (*result) -> block_size = block_size;
    return 0;
}

void delete_array(string_array *array_struct) {
    int i;
    if (array_struct->type == DYNAMIC) {
        for (i = 0; i < array_struct -> size; i++) {
            free(array_struct -> array[i]);
        }
        free(array_struct -> array);
    } else {
        for(i = 0; i < STATIC_SIZE; i++) {
            clean_block(array_struct, i);
        }
    }
    free(array_struct);
}

void delete_block(string_array* array_struct, int index) {
    if (array_struct->type == DYNAMIC) {
        if (inside_array(array_struct, index)) {
            free(array_struct -> array[index]);
            array_struct -> array[index] = NULL;
        }
    } else {
        clean_block(array_struct, index);
    }
}

void add_block(string_array *array_struct, int index, char* string) {
    int string_len = (int) strlen(string);
    if(string_len > array_struct -> block_size) return;
    switch (array_struct->type) {
        case STATIC:
            array_struct -> array[index] = string;
            break;
        case DYNAMIC:
            if(array_struct -> array[index] != NULL) delete_block(array_struct, index);
            array_struct -> array[index] = malloc((string_len+1) * sizeof(char));
            strcpy(array_struct -> array[index], string);
            break;
    }
}

int find_nearest(string_array *array_struct, int value) {
    int i = 0;
    int sum = value;
    int smallest_difference = abs(sum - ascii_sum(array_struct, i));
    int result = 0;
    for (i = 0; i < array_struct->size; i++) {
        int current_sum = ascii_sum(array_struct, i);
        if(smallest_difference >= abs(sum - current_sum)) {
            smallest_difference = abs(sum - current_sum);
            result = i;
        }
    }
    return result;
}

/** ---------------------------------------------------------------------------------------
 * Helper functions
 * ---------------------------------------------------------------------------------------
 */
// Initialize static array
void create_static(char ***array) {
    *array = (char **) static_array;
}

// Initialize dynamic array
int create_dynamic(size_t size, char*** array) {
    *array = malloc(size * sizeof(char *));
    if(*array) return 0;
    return 1; // allocating memory failed
}

// Check if index is in bounds of provided  array struct
int inside_array(string_array* array_struct, int index) {
    return index >= 0 && index < array_struct->size;
}

void clean_block(string_array* array_struct, int index) {
    array_struct -> array[index] = "";
}

int ascii_sum(string_array* array_struct, int index) {
    int i = 0;
    int sum = 0;
    if(array_struct -> array[index] == NULL) return index;
    while(array_struct -> array[index][i] != NULL){
        sum += array_struct -> array[index][i];
        i++;
    }
    return sum;
}





