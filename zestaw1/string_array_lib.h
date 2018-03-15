//
// Created by artur on 09.03.18.
//

#ifndef ZESTAW1_STRING_ARRAY_LIB_H
#define ZESTAW1_STRING_ARRAY_LIB_H

#define STATIC_SIZE 1000

// Type of allocated array
enum array_type{STATIC, DYNAMIC};

extern char static_array[STATIC_SIZE][STATIC_SIZE];

typedef struct string_array string_array;

struct string_array{
    char** array;
    size_t size; // for static, array is size x size, for dynamic size is number of blocks
    enum array_type type; // indicates the type of array
    int block_size;
};

// Creates array based on provided type (STATIC or DYNAMIC) and provided size
// Returns array size
int init(enum array_type type, size_t size, int block_size, string_array** result);

// Delete the table
void delete_array(string_array* array_struct);

void add_block(string_array* array_struct, int index, char *string);

// Finds nearest block, based on closest sum of ascii codes in block, to the analogous sum in block under provided index
int find_nearest(string_array* array_struct, int index);

// Deletes block under provided index
void delete_block(string_array* array_struct, int index);


#endif //ZESTAW1_STRING_ARRAY_LIB_H
