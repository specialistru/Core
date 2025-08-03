// gcc -std=c23 -pthread -lcrypto -o analyze analyze.c
// Windows: нужно собрать с OpenSSL и использовать Windows threads

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#include <bcrypt.h> // для SHA256 (можно OpenSSL, но здесь встроенный)
// Многопоточность Windows
#include <processthreadsapi.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>
#include <openssl/sha.h>
#endif

#include <ctype.h>

#define MAX_PATH_LEN 4096
#define MAX_LINE_LEN 4096

typedef struct FileInfo {
    char *file_id;
    char *filename;
    char *path; // относительный путь
    int lines;
    int lines_of_code;
    int functions_count;
    double comment_ratio;
    char **dependencies; // массив строк (includes)
    int dependencies_count;
    char **functions; // массив имен функций
    char sha256[65]; // hex SHA256
    char *content;
    struct FileInfo *next;
} FileInfo;

typedef struct ThreadArg {
    char filepath[MAX_PATH_LEN];
    char relative_path[MAX_PATH_LEN];
} ThreadArg;

static FileInfo *file_list = NULL;
static pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;

#ifdef _WIN32
CRITICAL_SECTION list_cs;
#endif

// Функции добавления и очистки списка
void add_fileinfo(FileInfo *fi) {
#ifdef _WIN32
    EnterCriticalSection(&list_cs);
#else
    pthread_mutex_lock(&list_mutex);
#endif
    fi->next = file_list;
    file_list = fi;
#ifdef _WIN32
    LeaveCriticalSection(&list_cs);
#else
    pthread_mutex_unlock(&list_mutex);
#endif
}

void free_fileinfos() {
    FileInfo *cur = file_list;
    while (cur) {
        FileInfo *tmp = cur;
        cur = cur->next;
        free(tmp->file_id);
        free(tmp->filename);
        free(tmp->path);
        for (int i=0; i < tmp->dependencies_count; i++) free(tmp->dependencies[i]);
        free(tmp->dependencies);
        for (int i=0; i < tmp->functions_count; i++) free(tmp->functions[i]);
        free(tmp->functions);
        free(tmp->content);
        free(tmp);
    }
}

// Простая генерация UUID (8 символов hex + timestamp)
void generate_file_id(char *buf, size_t size) {
    static unsigned int counter = 0;
    snprintf(buf, size, "%08x%08x", (unsigned int)time(NULL), counter++);
}

// Чтение файла в память, подсчет строк и строк кода (упрощенно)
char *read_file_content(const char *filepath, int *lines, int *lines_of_code, double *comment_ratio) {
    FILE *f = fopen(filepath, "r");
    if (!f) return NULL;

    char *content = NULL;
    size_t cap = 0;
    size_t len = 0;

    char line[MAX_LINE_LEN];
    *lines = 0;
    *lines_of_code = 0;
    int comment_lines = 0;

    bool in_block_comment = false;

    while (fgets(line, sizeof(line), f)) {
        (*lines)++;

        // Добавляем к контенту
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
        content[len] = 0;

        // Анализ строки для кода и комментариев
        char *p = line;
        while (*p && isspace((unsigned char)*p)) p++;
        if (*p == '\0') continue; // пустая строка

        // Проверка комментариев
        if (in_block_comment) {
            comment_lines++;
            if (strstr(p, "*/")) in_block_comment = false;
            continue;
        }

        if (strncmp(p, "/*", 2) == 0) {
            comment_lines++;
            if (!strstr(p, "*/")) in_block_comment = true;
            continue;
        }

        if (strncmp(p, "//", 2) == 0) {
            comment_lines++;
            continue;
        }

        (*lines_of_code)++;
    }

    fclose(f);

    if (*lines > 0)
        *comment_ratio = (double)comment_lines / (double)(*lines);
    else
        *comment_ratio = 0.0;

    return content;
}

#ifdef _WIN32
// SHA256 через Windows API bcrypt
bool sha256_windows(const char *data, size_t len, char out_hex[65]) {
    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_HASH_HANDLE hHash = NULL;
    NTSTATUS status;
    DWORD cbHash = 32, cbData = 0;
    BYTE hash[32];

    status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0);
    if (status != 0) return false;
    status = BCryptCreateHash(hAlg, &hHash, NULL, 0, NULL, 0, 0);
    if (status != 0) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }
    status = BCryptHashData(hHash, (PUCHAR)data, (ULONG)len, 0);
    if (status != 0) goto cleanup;
    status = BCryptFinishHash(hHash, hash, cbHash, 0);
    if (status != 0) goto cleanup;

    for (int i=0; i<32; i++) {
        sprintf(out_hex + i*2, "%02x", hash[i]);
    }
    out_hex[64] = 0;

cleanup:
    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return status == 0;
}
#else
// SHA256 через OpenSSL
bool sha256_openssl(const char *data, size_t len, char out_hex[65]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    if (!SHA256_Init(&sha256)) return false;
    if (!SHA256_Update(&sha256, data, len)) return false;
    if (!SHA256_Final(hash, &sha256)) return false;

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(out_hex + (i * 2), "%02x", hash[i]);
    out_hex[64] = 0;
    return true;
}
#endif

// Экранирование JSON строк
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

// Простейший парсер функций (ищем строки вида "тип имя(параметры) {" в content)
char **parse_functions(const char *content, int *func_count) {
    char **funcs = NULL;
    *func_count = 0;

    const char *p = content;
    while (*p) {
        // Ищем открывающую скобку и фигурную скобку после нее
        const char *paren = strchr(p, '(');
        if (!paren) break;
        const char *brace = strchr(paren, '{');
        if (!brace || brace - paren > 200) { // слишком далеко - пропускаем
            p = paren + 1;
            continue;
        }
        // Возьмем слово перед '(' — имя функции
        const char *name_end = paren;
        const char *name_start = name_end;
        while (name_start > content && (isalnum((unsigned char)name_start[-1]) || name_start[-1] == '_'))
            name_start--;
        if (name_start == name_end) {
            p = paren + 1;
            continue;
        }
        size_t len = name_end - name_start;
        char *func_name = malloc(len + 1);
        memcpy(func_name, name_start, len);
        func_name[len] = 0;

        // Добавляем в массив
        funcs = realloc(funcs, sizeof(char*) * (*func_count + 1));
        funcs[*func_count] = func_name;
        (*func_count)++;

        p = brace + 1;
    }
    return funcs;
}

// Простейший парсер includes - ищем строки #include "..."
char **parse_includes(const char *content, int *inc_count) {
    char **incs = NULL;
    *inc_count = 0;

    const char *p = content;
    while (*p) {
        const char *line_end = strchr(p, '\n');
        if (!line_end) line_end = p + strlen(p);

        if (strncmp(p, "#include", 8) == 0) {
            // ищем " или < >
            const char *start = strchr(p, '\"');
            if (!start) start = strchr(p, '<');
            if (start && start < line_end) {
                char end_char = (*start == '\"') ? '\"' : '>';
                const char *end = strchr(start + 1, end_char);
                if (end && end < line_end) {
                    size_t len = end - start - 1;
                    char *inc_name = malloc(len + 1);
                    memcpy(inc_name, start + 1, len);
                    inc_name[len] = 0;
                    incs = realloc(incs, sizeof(char*) * (*inc_count + 1));
                    incs[*inc_count] = inc_name;
                    (*inc_count)++;
                }
            }
        }
        p = (*line_end) ? line_end + 1 : line_end;
    }
    return incs;
}

#ifdef _WIN32
// Рекурсивный обход для Windows
void process_directory_win(const char *dir_path, const char *relative_path);
DWORD WINAPI thread_func(LPVOID arg);

void process_directory_win(const char *dir_path, const char *relative_path) {
    char search_path[MAX_PATH_LEN];
    WIN32_FIND_DATAA findData;
    HANDLE hFind;

    snprintf(search_path, MAX_PATH_LEN, "%s\\*", dir_path);

    hFind = FindFirstFileA(search_path, &findData);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
            continue;

        char full_path[MAX_PATH_LEN];
        snprintf(full_path, MAX_PATH_LEN, "%s\\%s", dir_path, findData.cFileName);

        char new_relative[MAX_PATH_LEN];
        if (relative_path[0] == 0)
            snprintf(new_relative, MAX_PATH_LEN, "%s", findData.cFileName);
        else
            snprintf(new_relative, MAX_PATH_LEN, "%s\\%s", relative_path, findData.cFileName);

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            process_directory_win(full_path, new_relative);
        } else {
            // Проверка расширения
            if (strstr(findData.cFileName, ".c") || strstr(findData.cFileName, ".h")) {
                // Запуск потока на обработку файла
                ThreadArg *targ = malloc(sizeof(ThreadArg));
                strcpy(targ->filepath, full_path);
                strcpy(targ->relative_path, new_relative);

                HANDLE hThread = CreateThread(NULL, 0, thread_func, targ, 0, NULL);
                if (hThread) {
                    CloseHandle(hThread);
                } else {
                    free(targ);
                }
            }
        }

    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);
}

// Поток для Windows
DWORD WINAPI thread_func(LPVOID arg) {
    ThreadArg *targ = (ThreadArg*)arg;

    int lines=0, loc=0, func_count=0, dep_count=0;
    double comment_ratio=0.0;

    char *content = read_file_content(targ->filepath, &lines, &loc, &comment_ratio);
    if (!content) {
        free(targ);
        return 1;
    }

    FileInfo *fi = malloc(sizeof(FileInfo));
    memset(fi, 0, sizeof(FileInfo));

    generate_file_id((char[33]){0}, 33);
    fi->file_id = malloc(33);
    generate_file_id(fi->file_id, 33);

    fi->filename = _strdup(strrchr(targ->filepath, '\\') ? strrchr(targ->filepath, '\\') + 1 : targ->filepath);
    fi->path = _strdup(targ->relative_path);
    fi->lines = lines;
    fi->lines_of_code = loc;
    fi->comment_ratio = comment_ratio;
    fi->content = content;

    fi->functions = parse_functions(content, &func_count);
    fi->functions_count = func_count;

    fi->dependencies = parse_includes(content, &dep_count);
    fi->dependencies_count = dep_count;

    // SHA256
    sha256_windows(content, strlen(content), fi->sha256);

    add_fileinfo(fi);

    free(targ);
    return 0;
}
#else
// Linux/Unix реализация

pthread_t *threads = NULL;
int threads_count = 0;

void *thread_func(void *arg);

void process_directory_unix(const char *dir_path, const char *relative_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        char full_path[MAX_PATH_LEN];
        snprintf(full_path, MAX_PATH_LEN, "%s/%s", dir_path, entry->d_name);

        char new_relative[MAX_PATH_LEN];
        if (relative_path[0] == 0)
            snprintf(new_relative, MAX_PATH_LEN, "%s", entry->d_name);
        else
            snprintf(new_relative, MAX_PATH_LEN, "%s/%s", relative_path, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) != 0) continue;

        if (S_ISDIR(st.st_mode)) {
            process_directory_unix(full_path, new_relative);
        } else {
            if (strstr(entry->d_name, ".c") || strstr(entry->d_name, ".h")) {
                ThreadArg *targ = malloc(sizeof(ThreadArg));
                strcpy(targ->filepath, full_path);
                strcpy(targ->relative_path, new_relative);

                pthread_t th;
                if (pthread_create(&th, NULL, thread_func, targ) == 0) {
                    threads = realloc(threads, sizeof(pthread_t) * (threads_count + 1));
                    threads[threads_count++] = th;
                } else {
                    free(targ);
                }
            }
        }
    }
    closedir(dir);
}

void *thread_func(void *arg) {
    ThreadArg *targ = (ThreadArg*)arg;

    int lines=0, loc=0, func_count=0, dep_count=0;
    double comment_ratio=0.0;

    char *content = read_file_content(targ->filepath, &lines, &loc, &comment_ratio);
    if (!content) {
        free(targ);
        return NULL;
    }

    FileInfo *fi = malloc(sizeof(FileInfo));
    memset(fi, 0, sizeof(FileInfo));

    fi->file_id = malloc(33);
    generate_file_id(fi->file_id, 33);

    fi->filename = strdup(strrchr(targ->filepath, '/') ? strrchr(targ->filepath, '/') + 1 : targ->filepath);
    fi->path = strdup(targ->relative_path);
    fi->lines = lines;
    fi->lines_of_code = loc;
    fi->comment_ratio = comment_ratio;
    fi->content = content;

    fi->functions = parse_functions(content, &func_count);
    fi->functions_count = func_count;

    fi->dependencies = parse_includes(content, &dep_count);
    fi->dependencies_count = dep_count;

    sha256_openssl(content, strlen(content), fi->sha256);

    add_fileinfo(fi);

    free(targ);
    return NULL;
}
#endif

// Вывод JSON
void output_json(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return;

    fprintf(f, "{\n  \"project_name\": \"linux-6.16\",\n  \"files\": [\n");

    // Перебираем список
    FileInfo *cur = file_list;
    bool first = true;
    while (cur) {
        if (!first) fprintf(f, ",\n");
        first = false;

        fprintf(f, "    {\n");

        fprintf(f, "      \"file_id\": \"%s\",\n", cur->file_id);
        fprintf(f, "      \"filename\": \"");
        escape_json_string(cur->filename, f);
        fprintf(f, "\",\n");

        fprintf(f, "      \"path\": \"");
        escape_json_string(cur->path, f);
        fprintf(f, "\",\n");

        fprintf(f, "      \"lines\": %d,\n", cur->lines);
        fprintf(f, "      \"lines_of_code\": %d,\n", cur->lines_of_code);
        fprintf(f, "      \"comment_ratio\": %.3f,\n", cur->comment_ratio);

        fprintf(f, "      \"functions_count\": %d,\n", cur->functions_count);
        fprintf(f, "      \"functions\": [");
        for (int i=0; i < cur->functions_count; i++) {
            if (i > 0) fprintf(f, ", ");
            fprintf(f, "\"");
            escape_json_string(cur->functions[i], f);
            fprintf(f, "\"");
        }
        fprintf(f, "],\n");

        fprintf(f, "      \"dependencies_count\": %d,\n", cur->dependencies_count);
        fprintf(f, "      \"dependencies\": [");
        for (int i=0; i < cur->dependencies_count; i++) {
            if (i > 0) fprintf(f, ", ");
            fprintf(f, "\"");
            escape_json_string(cur->dependencies[i], f);
            fprintf(f, "\"");
        }
        fprintf(f, "],\n");

        fprintf(f, "      \"sha256\": \"%s\",\n", cur->sha256);

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
#ifdef _WIN32
    InitializeCriticalSection(&list_cs);
#endif

    const char *root_path = 
#ifdef _WIN32
    "C:\\linux-6.16";
#else
    "/home/user/linux-6.16"; // поменяйте путь на Linux
#endif

#ifdef _WIN32
    process_directory_win(root_path, "");
#else
    process_directory_unix(root_path, "");
    // Ждём все потоки
    for (int i=0; i < threads_count; i++) {
        pthread_join(threads[i], NULL);
    }
    free(threads);
#endif

    output_json("output.json");

    free_fileinfos();

#ifdef _WIN32
    DeleteCriticalSection(&list_cs);
#endif

    printf("Готово, JSON сохранён в output.json\n");
    return 0;
}

