// Header Files (platform-specific includes)
#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>


#if defined(_WIN32 ) || defined(_WIN64)
    #include <windows.h>
    #include <shlwapi.h> 
#endif


// Function: Normalize extension
char* normalize_extension(const char* extension);

// Function: Get file extension
const char* get_extension(const char* file);

// Function: Create folder (Windows/Linux)
void create_folder_linux( const char* base_path, const char* folder_name);
void create_folder_windows(const char* base_path, const char* folder_name);

// Function: Move file (Windows/Linux)
void move_file_linux(const char* src_path, const char* dst_path);
void move_file_windows(const char* src_path, const char* dst_path);


// Check OS
string get_os(void);

// READ folder content
void read_folder(string path);

// Function: is_file vs. is_directory
bool is_directory_linux(const char *path);

#if defined(_WIN32) || defined(_WIN64)
    bool is_directory_windows(WIN32_FIND_DATA f);
#endif

void move_files(const char* src_path, const char* dst_path);

// Function: Sorter for Windows
void move_file_windows(const char* src_path, const char* dst_path);
// Function: Sorter for Linux
void move_file_linux(const char* src_path, const char* dst_path);

// Main function
int main(void) {
    printf("OS: %s\n", get_os());
    string path = "/home/h4xor/Desktop";
    read_folder(path);
    string file_name = "file.PDF";
    const char *ext = get_extension(file_name);
    printf("%s\n", ext);
    char *low_ext = normalize_extension(ext);
    if (low_ext != NULL) {
        printf("%s\n", low_ext);    
        free(low_ext);
    }
    else {
        printf("Memory allocation failedn\n");
    }
    
    return 0;
}


// get os
string get_os(void) {
    #ifdef __linux__
        return "linux";
    #elif defined(_WIN32) || defined(_WIN64)
        return "windows";
    #else
        return "unknown"
    #endif
}

// Read folders
void read_folder(string path) {
    string os = get_os();

    if (strcmp(os, "linux") == 0){
        printf("Linux mode\n");
        DIR* dir = opendir(path);
        
        if (dir == NULL) {
            perror("opendir");
        }
        else {
            struct dirent *items;
            while ((items = readdir(dir)) != NULL) {
                if (strcmp(items->d_name, ".") == 0 || strcmp(items->d_name, "..") == 0)
                    continue;
                printf("%s\n", items->d_name);
            }
            closedir(dir);
        }
    }

    else if (strcmp(os, "windows") == 0) {
        #if defined(_WIN32 ) || defined(_WIN64)
            printf("WIndows mode\n");
            char pattern[512];
            snprintf(pattern, sizeof(pattern), "%s\\*", path);
            WIN32_FIND_DATA findData;
            HANDLE hFind = FindFirstFile(pattern, &findData);

            if (hFind == INVALID_HANDLE_VALUE) {
                printf("Error opening directory.\n");
                return;
            }

            do {
                // Skip "." and ".."
                if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
                    continue;

                printf("%s\n", findData.cFileName);
            } while (FindNextFile(hFind, &findData) != 0);

            FindClose(hFind);
        #endif
    }

    else {
        printf("Unsupported\n");
    }

}

#if defined(_WIN32) || defined(_WIN64)
    bool is_directory_windows(WIN32_FIND_DATA f) {
        return f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
    }
#endif

bool is_directory_linux(const char *path) {
    struct stat stat_var;
    
    if (stat(path, &stat_var) == -1) {
        printf("Error occured\n");
        return false;
    }
    
    return S_ISDIR(stat_var.st_mode);
}

const char* get_extension(const char* file){
    const char *dot = strrchr(file, '.');
    if (!dot || dot == file) {
        return NULL;
    }
    return dot + 1;
}


char* normalize_extension(const char* extension) {
    int extension_len = strlen(extension);
    char *lower_extension = malloc(extension_len + 1);
    if (!lower_extension) {
        return NULL;  // memory allocation failed
    }
    for (int i = 0; i < extension_len; i++) {
        lower_extension[i] = tolower(extension[i]);
    }

    lower_extension[extension_len] = '\0';

    return lower_extension;
    
}


void create_folder_linux( const char* base_path, const char* folder_name) {
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/%s", base_path, folder_name);

    struct stat st;
    
    if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {
        printf("Folder exists\n");
    }

    else {
        if (mkdir(full_path, 0755) == 0) {
            printf("Directory created\n");
        }
        
        else {
            perror("Error Creating Directory");
        }
        
    }
    
}

void create_folder_windows(const char* base_path, const char* folder_name) {
    #if defined(_WIN32) || defined(_WIN64)
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s\\%s", base_path, folder_name);
        
        DWORD attrs = GetFileAttributes(full_path);
        if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
            printf("Folder Exists\n");
        } else {
            if (CreateDirectory(full_path, NULL)) {
                printf("Directory Created\n");
            } else {
                printf("Error Creating Directory. Code: %lu\n", GetLastError());
            }
        }
    #endif
}


void move_file_linux(const char* src_path, const char* dst_path) {
    struct dirent *items;
    DIR* dir = opendir(src_path);
    if (!dir) {
        perror("Couldn't open directory");
        return;
    }

    while ((items = readdir(dir)) != NULL) {
        if (strcmp(items->d_name, ".") == 0 || strcmp(items->d_name, "..") == 0)
            continue;

        const char* filename = items->d_name;
        const char* extensions = get_extension(filename);
        char* lower_extension;

        if (!extensions) {
            lower_extension = strdup("Others");
        } else {
            lower_extension = normalize_extension(extensions);
            if (!lower_extension) {
                perror("Memory allocation failed");
                continue;
            }
        }

        char src_full[512], dst_full[512];
        snprintf(src_full, sizeof(src_full), "%s/%s", src_path, filename);

        if (is_directory_linux(src_full)) {
            free(lower_extension);
            continue;
        }

        create_folder_linux(dst_path, lower_extension);
        snprintf(dst_full, sizeof(dst_full), "%s/%s/%s", dst_path, lower_extension, filename);

        if (rename(src_full, dst_full) != 0) {
            perror("Failed to move file");
        }

        free(lower_extension);
    }

    closedir(dir);  
}



void move_file_windows(const char* src_path, const char* dst_path) {
    #if defined(_WIN32) || defined(_WIN64)
        char pattern[512];
        snprintf(pattern, sizeof(pattern), "%s\\*", src_path);
        WIN32_FIND_DATA findData;

        HANDLE hFind = FindFirstFile(pattern, &findData);
        if (hFind == INVALID_HANDLE_VALUE) {
            printf("Error opening directory.\n");
            return;
        }

        do {
            if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
                continue;

            const char* filename = findData.cFileName;
            const char* extension = get_extension(filename);
            char* lower_extension;

            if (!extension) {
                lower_extension = strdup("Others");
            } else {
                lower_extension = normalize_extension(extension);
                if (!lower_extension) {
                    printf("Memory allocation failed\n");
                    continue;
                }
            }

            char src_full[512], dst_full[512];
            snprintf(src_full, sizeof(src_full), "%s\\%s", src_path, filename);

            if (is_directory_windows(src_full)) {
                free(lower_extension);
                continue;
            }

            create_folder_windows(dst_path, lower_extension);
            snprintf(dst_full, sizeof(dst_full), "%s\\%s\\%s", dst_path, lower_extension, filename);

            if (!MoveFile(src_full, dst_full)) {
                printf("Error moving file: %lu\n", GetLastError());
            }

            free(lower_extension);
        } while (FindNextFile(hFind, &findData) != 0);

        FindClose(hFind);
    #endif
}


void move_files(const char* src_path, const char* dst_path) {
    #if defined(_WIN32) || defined(_WIN64)
        move_file_windows(src_path, dst_path);
    #else
        move_file_linux(src_path, dst_path);
    #endif
}

    