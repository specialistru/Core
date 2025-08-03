#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <stdbool.h>

typedef struct FileInfo {
    char *filename;
    char *path; // путь относительно корня
    int lines;
    int lines_of_code;
    char *content;
    struct FileInfo *next;
} FileInfo;

FileInfo *file_list = NULL;
char root_path[MAX_PATH] = {0};

bool ends_with(const char *str, const char *suffix) {
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr) return false;
    return _stricmp(str + lenstr - lensuffix, suffix) == 0;
}

void add_fileinfo(FileInfo *fi) {
    fi->next = file_list;
    file_list = fi;
}

void free_fileinfos() {
    while (file_list) {
        FileInfo *tmp = file_list;
        file_list = file_list->next;
        free(tmp->filename);
        free(tmp->path);
        free(tmp->content);
        free(tmp);
    }
}

char *read_file_content(const char *filepath, int *lines, int *lines_of_code) {
    FILE *f = fopen(filepath, "r");
    if (!f) return NULL;

    char *content = NULL;
    size_t cap = 0;
    size_t len = 0;
    char line[4096];
    *lines = 0;
    *lines_of_code = 0;

    while (fgets(line, sizeof(line), f)) {
        (*lines)++;
        // простая проверка, не пустая ли строка, не комментарий
        bool is_code_line = false;
        for (char *p = line; *p; p++) {
            if (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\r') {
                if (*p != '/' && *p != '*') { // грубая проверка, можно улучшить
                    is_code_line = true;
                    break;
                }
            }
        }
        if (is_code_line) (*lines_of_code)++;

        size_t linelen = strlen(line);
        if (len + linelen + 1 > cap) {
            cap = (cap + linelen + 1024) * 2;
            content = realloc(content, cap);
            if (!content) {
                fclose(f);
                return NULL;
            }
        }
        memcpy(content + len, line, linelen);
        len += linelen;
    }
    if (content) content[len] = 0;

    fclose(f);
    return content;
}

void escape_json_string(const char *input, FILE *out) {
    for (const char *p = input; *p; p++) {
        switch (*p) {
        case '\"': fputs("\\\"", out); break;
        case '\\': fputs("\\\\", out); break;
        case '\b': fputs("\\b", out); break;
        case '\f': fputs("\\f", out); break;
        case '\n': fputs("\\n", out); break;
        case '\r': fputs("\\r", out); break;
        case '\t': fputs("\\t", out); break;
        default:
            if ((unsigned char)*p < 0x20) {
                fprintf(out, "\\u%04x", (unsigned char)*p);
            } else {
                fputc(*p, out);
            }
        }
    }
}

void process_directory(const char *dir_path, const char *relative_path) {
    WIN32_FIND_DATAA findData;
    char search_path[MAX_PATH];

    snprintf(search_path, MAX_PATH, "%s\\*", dir_path);

    HANDLE hFind = FindFirstFileA(search_path, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
            continue;

        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s\\%s", dir_path, findData.cFileName);

        char new_relative_path[MAX_PATH];
        if (relative_path[0] == '\0')
            snprintf(new_relative_path, MAX_PATH, "%s", findData.cFileName);
        else
            snprintf(new_relative_path, MAX_PATH, "%s\\%s", relative_path, findData.cFileName);

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            process_directory(full_path, new_relative_path);
        } else {
            if (ends_with(findData.cFileName, ".c") || ends_with(findData.cFileName, ".h")) {
                int lines = 0, loc = 0;
                char *content = read_file_content(full_path, &lines, &loc);
                if (!content) continue;

                FileInfo *fi = malloc(sizeof(FileInfo));
                fi->filename = _strdup(findData.cFileName);
                fi->path = _strdup(relative_path);
                fi->lines = lines;
                fi->lines_of_code = loc;
                fi->content = content;
                fi->next = NULL;

                add_fileinfo(fi);
            }
        }

    } while (FindNextFileA(hFind, &findData));
    FindClose(hFind);
}

void output_json(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return;

    fprintf(f, "{\n  \"project_name\": \"linux-6.16\",\n  \"files\": [\n");

    FileInfo *cur = file_list;
    bool first = true;
    while (cur) {
        if (!first) fprintf(f, ",\n");
        first = false;

        fprintf(f, "    {\n");
        fprintf(f, "      \"filename\": \"");
        escape_json_string(cur->filename, f);
        fprintf(f, "\",\n");

        fprintf(f, "      \"path\": \"");
        escape_json_string(cur->path, f);
        fprintf(f, "\",\n");

        fprintf(f, "      \"lines\": %d,\n", cur->lines);
        fprintf(f, "      \"lines_of_code\": %d,\n", cur->lines_of_code);

        fprintf(f, "      \"content\": \"");
        escape_json_string(cur->content, f);
        fprintf(f, "\"\n");

        fprintf(f, "    }");
        cur = cur->next;
    }
    fprintf(f, "\n  ]\n}\n");
    fclose(f);
}

int main(void) {
    // Укажи исходную папку
    const char *root = "C:\\linux-6.16";
    root_path[0] = '\0';

    process_directory(root, "");

    output_json("output.json");

    free_fileinfos();
    printf("JSON сформирован в output.json\n");
    return 0;
}

