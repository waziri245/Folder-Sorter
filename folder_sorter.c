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
#endif


// Function: Normalize extension
char* normalize_extension(const char* extension);

// Function: Get file extension
const char* get_extension(const char* file);

// Function: Create folder (Windows/Linux)
// Function: Move file (Windows/Linux)
// Function: Sort files in directory (uses above)


// Check OS
string get_os(void);

// READ folder content
void read_folder(string path);

// Function: is_file vs. is_directory
bool is_directory_linux(const char *path);

#if defined(_WIN32) || defined(_WIN64)
    bool is_directory_windows(WIN32_FIND_DATA f);
#endif


// Function: Sorter for Windows
// Function: Sorter for Linux

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
            char pattern[MAX_PATH];
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