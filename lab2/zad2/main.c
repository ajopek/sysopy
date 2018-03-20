#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <math.h>
#include <ftw.h>
#include <string.h>
#include <time.h>

typedef struct dirent dirent;


// dir handling-------------------
DIR* open_dir(char* name);
void print_file_info(dirent* file, struct stat* stat_buffer, char* abs_path);
char* get_absolute_path(char* path, char* file_name);
int traverse_directory(char* path, int (*comparator)(time_t, time_t), time_t date);
// comparators -------------------
int greater(time_t, time_t);
int equal(time_t, time_t);
int smaller(time_t, time_t);
// Helper functions --------------
void print_help();
time_t parse_string_date(char* string);

int (*compare)(time_t, time_t);
time_t date;

int fn_nftw(const char *path, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    char* absolute_path = malloc(4096);
    realpath(path, absolute_path);

    if ((tflag == FTW_F) && compare(date, sb->st_mtime)) {
        print_file_info(NULL, sb, path);
    }
    free(absolute_path);
    return 0;
}

int main(int argc, char* argv[]) {
    int i;
    int argv_i = 1;
    if(argc < 6){
        print_help();
        return 0;
    }

    // get dir path
    char* path = argv[1];

    // get date
    char *text_date = malloc(20);
    strcpy(text_date, argv[3]);
    strcat(text_date, " ");
    strcat(text_date, argv[4]);

    date = parse_string_date(text_date);

    int nftw_bit = atoi(argv[5]);

    if (strcmp("=", argv[2]) == 0) {
        compare = &equal;
        if(nftw_bit) {
            int nopenfd = 100;
            int flags = 0;
            nftw(path, &fn_nftw, nopenfd, flags);
        }
        else traverse_directory(path, &equal, parse_string_date(date));
    } else if (strcmp(">", argv[2]) == 0) {
        compare = &greater;
        if(nftw_bit) {
            int nopenfd = 100;
            int flags = 0;
            nftw(path, &fn_nftw, nopenfd, flags);
        }
        else traverse_directory(path, &greater, parse_string_date(date));
    } else if (strcmp("<", argv[2]) == 0) {
        compare = &smaller;
        if(nftw_bit) {
            int nopenfd = 100;
            int flags = 0;
            nftw(path, &fn_nftw, nopenfd, flags);
        }
        else traverse_directory(path, &smaller, parse_string_date(date));
    } else {
        print_help();
    }
    return 0;

}

int traverse_directory(char* path, int (*comparator)(time_t, time_t), time_t date) {
    DIR* dir = open_dir(path);
    dirent* entry;
    if(dir == NULL) return 0;
    while(1) {
        entry = readdir(dir);
        if(entry == NULL) break;
        if(strcmp(entry->d_name, "..") == 0
           || strcmp(entry->d_name, ".") == 0) continue;
        char* abs_path = get_absolute_path(path, entry->d_name);
        if(entry->d_type == DT_REG) {
            struct stat* stat_buffer = malloc(sizeof(struct stat));
            stat(abs_path, stat_buffer);
            if(comparator(date, stat_buffer->st_mtime)) {
                print_file_info(entry, stat_buffer, abs_path);
            }
            free(stat_buffer);
        }
        if(entry->d_type == DT_DIR) {
            traverse_directory(abs_path, comparator, date);
        }
        free(abs_path);
    }
    closedir(dir);
    free(entry);
    return 0;
}

DIR* open_dir(char* name) {
    DIR* result = opendir(name);
    if(!result) {
        printf("Opening dir %s failed \n", name);
    }
    return result;
}

char* get_absolute_path(char* path, char* file_name) {
    size_t path_length = strlen(path);
    size_t filename_length = strlen(file_name);

    int append_slash = path_length > 0 && path[path_length - 1] != '/';
    char* absolute_path = malloc(
            path_length + append_slash + filename_length + 1);
    strncpy(absolute_path, path, path_length);
    if (append_slash) strncpy(absolute_path + path_length, "/", 1);
    strncpy(absolute_path + path_length + append_slash, file_name, filename_length);
    absolute_path[path_length + append_slash + filename_length] = '\0';
    char* result = malloc(4096);
    realpath(absolute_path, result);
    free(absolute_path);

    return result;
}

char* get_permissions(struct stat* stat_buffer) {
    char* permissions = malloc(sizeof(char) * 11);
    permissions[0] = S_ISDIR(stat_buffer->st_mode) ? 'd' : '-';
    permissions[1] = (stat_buffer->st_mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (stat_buffer->st_mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (stat_buffer->st_mode & S_IXUSR) ? 'x' : '-';
    permissions[4] = (stat_buffer->st_mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (stat_buffer->st_mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (stat_buffer->st_mode & S_IXGRP) ? 'x' : '-';
    permissions[7] = (stat_buffer->st_mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (stat_buffer->st_mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (stat_buffer->st_mode & S_IXOTH) ? 'x' : '-';
    permissions[10] = '\0';
    return permissions;
}

void print_file_info(dirent* file, struct stat* stat_buffer, char* abs_path) {
    char* permissions =  get_permissions(stat_buffer);
    printf("%s\t%lld\t%s\t%s \n",
           abs_path,
           (long long int)stat_buffer->st_size,
           permissions,
           ctime(&(stat_buffer->st_mtime))
    );
    free(permissions);
}

int greater(time_t a, time_t b){
    return difftime(a, b) > 0.0001 ? 1 : 0;
};
int equal(time_t a, time_t b) {
    return fabs(difftime(a, b)) < 0.0001 ? 1 : 0;
};
int smaller(time_t a, time_t b) {
    return difftime(a, b) < 0.0001 ? 1 : 0;
};

void print_help(){
    printf("Usage of program: \n"
                   "dir_name > | < | = Y-m-d H:M:S ntfs(0 | 1)\n");
    exit(0);
}

time_t parse_string_date(char* string) {
    struct tm time = {0, 2, 10, 1, 1, 2018};
    strptime(string, "%Y-%m-%d %H:%M:%S", &time);
    return mktime(&time);
};