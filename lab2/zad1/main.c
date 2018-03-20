#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <zconf.h>

typedef struct timespec timespec;
typedef struct timeval timeval;
typedef struct rusage rusage;
typedef enum array_type array_type;

typedef struct timestamp{
    long real;
    long user;
    long system;
} timestamp;

typedef struct record{
    int index;
    char* content;
} record;

typedef enum lib_type {
    SYS,
    LIB
} lib_type;

typedef enum command{
    GENERATE,
    SORT,
    COPY
} command;

//TODO: Add some constraint on file size

// Main functionality ------------------------
void fill_record(record* record, int size);
void generate(char* path, int size, int record_num, lib_type type);
void copy(char* path_from, char* path_to, int size, int record_num, lib_type type);
int swap(FILE* file, int size, lib_type type, record* buffer1, record* buffer2);
void sort(char* path, int size, int record_num, lib_type type);
// Time measurement --------------------------
timestamp get_timestamp();
void print_timestamp(FILE* file,timestamp timestamp);
long get_time(timeval time);
void print_time(FILE* file, timestamp start, timestamp end, char* description, int number);
timestamp stamp_diff(timestamp start, timestamp end);
// File handling -----------------------------
FILE* open_file(char* path, char* flags);
int read_record(record* record, FILE* file, int index, int size, lib_type type);
int write_record(record* record, FILE* file, int size, lib_type type);
// Helper functions --------------------------
void print_help();
char random_byte();
lib_type to_lib_type(char* string);
void print_header(lib_type type, int size, int num);
// -------------------------------------------

int main(int argc, char* argv[]) {
    int i;
    int argv_i = 1;
    if(argc < 4){
        print_help();
        return 0;
    }
    // Get command to execute
    command command1;
    if (strcmp("generate", argv[argv_i]) == 0) {
        command1 = GENERATE;
    } else if (strcmp("sort", argv[argv_i]) == 0) {
        command1 = SORT;
    } else if (strcmp("copy", argv[argv_i]) == 0) {
        command1 = COPY;
    } else {
        print_help();
    }
    char *name1 = NULL;
    char *name2 = NULL;
    int record_num = 0;
    int record_size = 0;
    lib_type type;
    switch (command1) {
        case GENERATE:
            if (argc != 5) print_help();
            name1 = argv[2];
            record_num = atoi(argv[3]);
            record_size = atoi(argv[4]);
            generate(name1, record_size, record_num, SYS);
            break;
        case COPY:
            if (argc != 7) print_help();
            name1 = argv[2];
            name2 = argv[3];
            record_num = atoi(argv[4]);
            record_size = atoi(argv[5]);
            type = to_lib_type(argv[6]);
            copy(name1, name2, record_size, record_num, type);
            break;
        case SORT:
            if (argc != 6) print_help();
            name1 = argv[2];
            record_num = atoi(argv[3]);
            record_size = atoi(argv[4]);
            type = to_lib_type(argv[5]);
            sort(name1, record_size, record_num, type);
    }

    return 0;
}

/** ---------------------------------------------------------------------------------------
 * Main functionality
 * ---------------------------------------------------------------------------------------
 */

void init_record(record** record, int size, int index){
    (*record) = malloc(sizeof(record));
    (*record)->content = malloc(size * sizeof(char));
    (*record)->index = index;
}

void delete_record(record** record) {
    free((*record)->content);
    free(*record);
}

void generate(char* path, int size, int record_num, lib_type type){
    int i;
    record* buffer;
    init_record(&buffer, size, -1);
    FILE* file = open_file(path, "w");
    srand(time(NULL));
    for (i = 0; i < record_num; ++i) {
        fill_record(buffer, size);
        if(write_record(buffer, file, size, type)) {
            fclose(file);
            delete_record(&buffer);
            exit(1);
        }
    }
    delete_record(&buffer);
    fclose(file);
}


void fill_record(record* record, int size){
    int i;
    for(i = 0; i < size; ++i) {
        record->content[i] = random_byte();
    }
}

void copy(char* path_from, char* path_to, int size, int record_num, lib_type type) {
    print_header(type, size, record_num);
    FILE* from = open_file(path_from, "r");
    FILE* to = open_file(path_to, "w");
    FILE* raport = open_file("raport", "a");
    record* buffer;
    init_record(&buffer, size, -1);
    int i;
    timestamp start,end;
    start = get_timestamp();
    for (i = 0; i < record_num; ++i) {
        if(read_record(buffer, from, i, size, type)) {
            fclose(from);
            fclose(to);
            delete_record(&buffer);
            exit(1);
        }
        if(write_record(buffer, to, size, type)) {
            fclose(from);
            fclose(to);
            delete_record(&buffer);
            exit(1);
        }
    }
    end = get_timestamp();
    print_time(raport, start, end, "copy", 1);
    fclose(raport);
    delete_record(&buffer);
    fclose(from);
    fclose(to);
}

void sort(char* path, int size, int record_num, lib_type type) {
    print_header(type, size, record_num);
    FILE* file = open_file(path, "r+");
    FILE* raport = open_file("raport", "a");
    record *buffer1, *buffer2;
    init_record(&buffer1, size, 0);
    init_record(&buffer2, size, 0);
    int i;
    timestamp start,end;
    start = get_timestamp();
    for(i = 0; i < record_num; ++i) {
        if(read_record(buffer2, file, i, size, type)){
            fclose(file);
            delete_record(&buffer1);
            delete_record(&buffer2);
            exit(1);
        };
        int j;
        for (j = i - 1; j >= 0 ; --j){
            if(read_record(buffer1, file, j, size, type)){
                fclose(file);
                delete_record(&buffer1);
                delete_record(&buffer2);
                exit(1);
            };
            if(buffer1->content[0] <= buffer2->content[0]) break;
            if(swap(file, size, type, buffer1, buffer2)){
                fclose(file);
                delete_record(&buffer1);
                delete_record(&buffer2);
                exit(1);
            };
        };
    }
    end = get_timestamp();
    print_time(raport, start, end, "sort", 1);
    fclose(raport);
    fclose(file);
    delete_record(&buffer1);
    delete_record(&buffer2);
}

int swap(FILE* file, int size, lib_type type, record* buffer1, record* buffer2) {
    //swap indexes
    int index = buffer1->index;
    buffer1->index = buffer2->index;
    buffer2->index = index;
    // write contents at new indexes
    if(write_record(buffer1, file, size, type)) return 1;
    if(write_record(buffer2, file, size, type)) return 1;
    return 0;
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

FILE* open_file(char* path, char* flags){
    FILE *f = fopen(path, flags);
    if(f == NULL) {
        printf("File could not be opened");
        exit(1);
    }
    return f;
}

int read_record(record* record, FILE* file, int index, int size, lib_type type){
    int bytes_read;
    record->index = index;
    off_t offset;
    switch(type) {
        case SYS:
            offset = lseek(fileno(file), index * size, SEEK_SET);
            bytes_read = (int)read(fileno(file), record->content, (size_t) size);
            break;
        case LIB:
            offset= fseek(file, index * size, 0);
            bytes_read = (int)fread(record->content, sizeof(char),(size_t) size, file);
            break;
        default:
            return 1;
    }
    if(bytes_read == 0){
        printf("File shorter than specified in arguments");
        return 1;
    } // end of file
    if(offset == -1){
        printf("System call error");
        return 1;
    } // error when changing offset
    if(bytes_read != size){
        printf("File has different format than specified");
        return 1;
    } //file has bad format, or error in record handling
    return 0;
}

int write_record(record* record, FILE* file, int size, lib_type type){
    int bytes_written;
    off_t offset;
    switch(type) {
        case SYS:
            if(record->index != -1) offset = lseek(fileno(file), record->index * size, SEEK_SET);
            bytes_written = (int)write(fileno(file), record->content, (size_t) size);
            break;
        case LIB:
            if(record->index != -1) offset = fseek(file, record->index * size, 0);
            bytes_written = (int)fwrite(record->content, sizeof(char), (size_t)size, file);
            break;
        default:
            return 1;
    }
    if(bytes_written < 0 || offset == -1 || bytes_written != size) {
        printf("System call error");
        return 1;
    } // system call error
    // file has bad format, or error in record handling
    return 0;
}




/** ---------------------------------------------------------------------------------------
 * Helper functions
 * ---------------------------------------------------------------------------------------
 */

void print_help(){
    printf("Usage of program: \n"
                   "copy file1 file2 record_num record_size sys|lib\n"
                   "generate file record_num record_size \n"
                   "sort file record_num record_size sys|lib\n");
    exit(0);
}

char random_byte(){
    const char symbols[] = "abcdefghijklmnoprstqwxyz";
    const int symbols_len = sizeof symbols -1;
    return symbols[rand()%symbols_len];

}

lib_type to_lib_type(char* string){
    if (strcmp(string, "sys") == 0) {
        return SYS;
    } else if (strcmp(string, "lib") == 0) {
        return LIB;
    } else {
        printf("Wrong lib type");
        exit(0);
    }
}

void print_header(lib_type type, int size, int num) {
    FILE* raport = open_file("raport", "a");
    printf("Type: %s, record size: %i, record number: %i \n", type == SYS ? "sys" : "lib", size, num);
    fprintf(raport, "Type: %s, record size: %i, record number: %i \n", type == SYS ? "sys" : "lib", size, num);
    fclose(raport);
}
