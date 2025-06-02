// -------------------- Header Files --------------------
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#endif

typedef char* string;
// -------------------- Utility Functions (OS-Agnostic) --------------------

// Get the current OS as a string
string get_os(void) {
    #ifdef __linux__
        return "linux";
    #elif defined(_WIN32) || defined(_WIN64)
        return "windows";
    #endif
}

// Get the file extension (after last dot)
const char* get_extension(const char* file) {
    const char *dot = strrchr(file, '.');
    if (!dot || dot == file) {
        return NULL;
    }
    return dot + 1;
}

// Normalize an extension to lowercase
char* normalize_extension(const char* extension) {
    int extension_len = strlen(extension);
    char *lower_extension = malloc(extension_len + 1);
    if (!lower_extension) {
        return NULL;
    }
    for (int i = 0; i < extension_len; i++) {
        lower_extension[i] = tolower(extension[i]);
    }
    lower_extension[extension_len] = '\0';
    return lower_extension;
}

// Check if path is a directory (Linux)
bool is_directory_linux(const char *path) {
    struct stat stat_var;
    if (stat(path, &stat_var) == -1) {
        return false;
    }
    return S_ISDIR(stat_var.st_mode);
}

// Create folder on Linux
void create_folder_linux(const char* base_path, const char* folder_name) {
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/%s", base_path, folder_name);

    struct stat st;
    if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {
        printf("Folder exists\n");
    } else {
    #ifdef _WIN32
        mkdir(full_path);  // Windows needs only one argument
    #else
        mkdir(full_path, 0755);  // Linux/macOS needs mode
    #endif
        printf("Directory created\n");
    }
}


// -------------------- Linux File Sorting --------------------
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

        // Handle file with no extension → "Others"
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

        // Create destination folder and move file
        create_folder_linux(dst_path, lower_extension);
        snprintf(dst_full, sizeof(dst_full), "%s/%s/%s", dst_path, lower_extension, filename);

        if (rename(src_full, dst_full) != 0) {
            perror("Failed to move file");
        }

        free(lower_extension);
    }

    closedir(dir);  
}

// -------------------- Windows-Specific Functions --------------------
#if defined(_WIN32) || defined(_WIN64)

// Check if a path is a directory (Windows)
bool is_directory_windows(const char* path) {
    DWORD attrs = GetFileAttributes(path);
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return (attrs & FILE_ATTRIBUTE_DIRECTORY);
}

// Create folder on Windows
void create_folder_windows(const char* base_path, const char* folder_name) {
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
}

// Windows file sorter
void move_file_windows(const char* src_path, const char* dst_path) {
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
            lower_extension = _strdup("Others");
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
}

#endif // End Windows

// -------------------- Entry Point for Python --------------------
void move_files(const char* src_path, const char* dst_path) {
    #if defined(_WIN32) || defined(_WIN64)
        move_file_windows(src_path, dst_path);
    #else
        move_file_linux(src_path, dst_path);
    #endif
}

// -------------------- Main Function --------------------
int main(void) {
    
    return 0;
}
