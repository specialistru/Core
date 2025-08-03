// compile with:
// gcc -std=c23 -pthread -lclang -lcrypto -lz -o analyzer analyzer.c
// (libclang и OpenSSL должны быть установлены)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <clang-c/Index.h>
#include <openssl/sha.h>
#include <zlib.h> // для сжатия

#define MAX_PATH 4096
#define THREAD_POOL_SIZE 8

// Структура задачи — обработать файл
typedef struct {
    char filepath[MAX_PATH];
    char relpath[MAX_PATH];
} Task;

// Очередь задач (простейшая)
typedef struct TaskNode {
    Task task;
    struct TaskNode *next;
} TaskNode;

typedef struct {
    TaskNode *head;
    TaskNode *tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool stop;
} TaskQueue;

// Результат файла
typedef struct {
    char file_id[33];
    char filename[MAX_PATH];
    char relpath[MAX_PATH];
    int lines;
    int loc;
    int functions_count;
    int complexity; // пример: суммарная сложность по функциям
    double comment_ratio;
    char sha256[65];
    char *compressed_content; // сжатый контент
    size_t compressed_size;
    char **dependencies;
    int dep_count;
    char **functions;
    int func_count;
} FileResult;

typedef struct FileResultNode {
    FileResult *result;
    struct FileResultNode *next;
} FileResultNode;

// Глобальные структуры
TaskQueue g_task_queue;
FileResultNode *g_results_head = NULL;
FileResultNode *g_results_tail = NULL;
pthread_mutex_t g_results_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t g_threads[THREAD_POOL_SIZE];

// Функции управления задачами, пулом и результатами — добавление/удаление
// ... (ниже будут реализованы)

// --- Утилиты ---

// Генерация ID файла (UUID-like)
void generate_id(char *buf, size_t sz) {
    static unsigned int counter = 0;
    snprintf(buf, sz, "%08x%08x", (unsigned int)time(NULL), counter++);
}

// Подсчёт SHA256 (OpenSSL)
void calc_sha256(const char *data, size_t len, char out[65]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data, len);
    SHA256_Final(hash, &sha256);
    for (int i=0; i<SHA256_DIGEST_LENGTH; i++)
        sprintf(out + i*2, "%02x", hash[i]);
    out[64] = 0;
}

// Сжатие контента gzip (zlib)
int compress_content(const char *data, size_t data_len, char **out, size_t *out_len) {
    uLongf comp_size = compressBound(data_len);
    *out = malloc(comp_size);
    if (!*out) return -1;
    int res = compress2((Bytef *)*out, &comp_size, (const Bytef *)data, data_len, Z_BEST_COMPRESSION);
    if (res != Z_OK) {
        free(*out);
        *out = NULL;
        return -1;
    }
    *out_len = comp_size;
    return 0;
}

// Подсчёт строк, LOC, комментариев (упрощённо)
void analyze_source_text(const char *text, int *lines, int *loc, double *comment_ratio) {
    int line_count = 0, code_lines = 0, comment_lines = 0;
    bool in_block_comment = false;
    const char *p = text;
    char line[4096];
    while (*p) {
        // Получаем строку
        const char *next_line = strchr(p, '\n');
        size_t len = next_line ? (size_t)(next_line - p) : strlen(p);
        if (len >= sizeof(line)) len = sizeof(line)-1;
        memcpy(line, p, len);
        line[len] = 0;

        line_count++;

        char *l = line;
        while (*l && isspace((unsigned char)*l)) l++;
        if (*l == 0) { p = next_line ? next_line+1 : p+len; continue; }

        if (in_block_comment) {
            comment_lines++;
            if (strstr(l, "*/")) in_block_comment = false;
        } else if (strncmp(l, "/*", 2) == 0) {
            comment_lines++;
            if (!strstr(l, "*/")) in_block_comment = true;
        } else if (strncmp(l, "//", 2) == 0) {
            comment_lines++;
        } else {
            code_lines++;
        }

        p = next_line ? next_line+1 : p+len;
    }
    *lines = line_count;
    *loc = code_lines;
    *comment_ratio = line_count > 0 ? (double)comment_lines / line_count : 0.0;
}

// Использование libclang для анализа функций, сложности, includes

typedef struct {
    FileResult *res;
    int func_count;
    int dep_count;
} ClangVisitorData;

CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, CXClientData client_data) {
    ClangVisitorData *data = (ClangVisitorData*)client_data;

    CXCursorKind kind = clang_getCursorKind(cursor);
    if (kind == CXCursor_FunctionDecl) {
        CXString name = clang_getCursorSpelling(cursor);
        const char *func_name = clang_getCString(name);

        // Добавляем функцию в список
        data->res->functions = realloc(data->res->functions, sizeof(char*) * (data->func_count + 1));
        data->res->functions[data->func_count] = strdup(func_name);
        data->func_count++;

        // TODO: Добавить вычисление сложности (можно через подсчёт ветвлений в теле)

        clang_disposeString(name);
    } else if (kind == CXCursor_InclusionDirective) {
        CXString included_file = clang_getIncludedFile(cursor);
        if (clang_getCString(included_file)) {
            data->res->dependencies = realloc(data->res->dependencies, sizeof(char*) * (data->dep_count + 1));
            data->res->dependencies[data->dep_count] = strdup(clang_getCString(included_file));
            data->dep_count++;
        }
        clang_disposeString(included_file);
    }

    return CXChildVisit_Recurse;
}

bool parse_with_clang(const char *filepath, FileResult *res) {
    CXIndex index = clang_createIndex(0, 0);

    const char *args[] = {
        "-I/usr/include",
        "-I/usr/local/include"
        // Можно добавить пути из системы сборки
    };

    CXTranslationUnit unit = clang_parseTranslationUnit(
        index,
        filepath,
        args, sizeof(args)/sizeof(args[0]),
        NULL, 0,
        CXTranslationUnit_None
    );
    if (!unit) {
        fprintf(stderr, "Error parsing %s\n", filepath);
        clang_disposeIndex(index);
        return false;
    }

    ClangVisitorData data = { .res = res, .func_count=0, .dep_count=0 };
    clang_visitChildren(clang_getTranslationUnitCursor(unit), visitor, &data);

    res->functions_count = data.func_count;
    res->dependencies_count = data.dep_count;

    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);

    return true;
}

// Загрузка файла в память
char *load_file(const char *filepath, size_t *out_size) {
    FILE *f = fopen(filepath, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(size + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }
    fread(buf, 1, size, f);
    buf[size] = 0;
    fclose(f);
    if (out_size) *out_size = size;
    return buf;
}

// Добавление результата в глобальный список
void add_result(FileResult *res) {
    FileResultNode *node = malloc(sizeof(FileResultNode));
    node->result = res;
    node->next = NULL;
    pthread_mutex_lock(&g_results_mutex);
    if (!g_results_tail) {
        g_results_head = node;
        g_results_tail = node;
    } else {
        g_results_tail->next = node;
        g_results_tail = node;
    }
    pthread_mutex_unlock(&g_results_mutex);
}

// Обработка одной задачи (файла)
void process_file(Task *task) {
    FileResult *res = calloc(1, sizeof(FileResult));
    generate_id(res->file_id, sizeof(res->file_id));

    strncpy(res->filename, strrchr(task->filepath, '/') ? strrchr(task->filepath, '/') + 1 : task->filepath, MAX_PATH-1);
    strncpy(res->relpath, task->relpath, MAX_PATH-1);

    size_t size;
    char *content = load_file(task->filepath, &size);
    if (!content) {
        fprintf(stderr, "Can't load file %s\n", task->filepath);
        free(res);
        return;
    }

    analyze_source_text(content, &res->lines, &res->loc, &res->comment_ratio);

    if (!parse_with_clang(task->filepath, res)) {
        fprintf(stderr, "Parsing failed for %s\n", task->filepath);
        // Можно решить как поступить, оставить пустые функции и зависимости
    }

    calc_sha256(content, size, res->sha256);

    if (compress_content(content, size, &res->compressed_content, &res->compressed_size) != 0) {
        fprintf(stderr, "Compression failed for %s\n", task->filepath);
        // Просто сохраним несжатый
        res->compressed_content = strdup(content);
        res->compressed_size = size;
    }

    free(content);

    add_result(res);
}

// Поток воркер
void *worker_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&g_task_queue.mutex);
        while (!g_task_queue.head && !g_task_queue.stop) {
            pthread_cond_wait(&g_task_queue.cond, &g_task_queue.mutex);
        }
        if (g_task_queue.stop && !g_task_queue.head) {
            pthread_mutex_unlock(&g_task_queue.mutex);
            break;
        }
        // Взять задачу из очереди
        TaskNode *node = g_task_queue.head;
        g_task_queue.head = node->next;
        if (!g_task_queue.head) g_task_queue.tail = NULL;
        pthread_mutex_unlock(&g_task_queue.mutex);

        process_file(&node->task);

        free(node);
    }
    return NULL;
}

// Добавить задачу
void enqueue_task(const char *filepath, const char *relpath) {
    TaskNode *node = malloc(sizeof(TaskNode));
    strncpy(node->task.filepath, filepath, MAX_PATH-1);
    strncpy(node->task.relpath, relpath, MAX_PATH-1);
    node->next = NULL;

    pthread_mutex_lock(&g_task_queue.mutex);
    if (g_task_queue.tail) {
        g_task_queue.tail->next = node;
        g_task_queue.tail = node;
    } else {
        g_task_queue.head = g_task_queue.tail = node;
    }
    pthread_cond_signal(&g_task_queue.cond);
    pthread_mutex_unlock(&g_task_queue.mutex);
}

// Рекурсивный обход каталогов (Linux)
void scan_directory(const char *base_path, const char *relpath) {
    DIR *dir = opendir(base_path);
    if (!dir) {
        perror("opendir");
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        char fullpath[MAX_PATH];
        char newrel[MAX_PATH];
        snprintf(fullpath, MAX_PATH, "%s/%s", base_path, entry->d_name);
        if (strlen(relpath) == 0)
            snprintf(newrel, MAX_PATH, "%s", entry->d_name);
        else
            snprintf(newrel, MAX_PATH, "%s/%s", relpath, entry->d_name);

        struct stat st;
        if (stat(fullpath, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                scan_directory(fullpath, newrel);
            } else {
                // Обрабатываем только .c/.h
                if (strstr(entry->d_name, ".c") || strstr(entry->d_name, ".h")) {
                    enqueue_task(fullpath, newrel);
                }
            }
        }
    }
    closedir(dir);
}

// JSON вывод (только метаданные, контент в отдельные файлы)
void save_json(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        perror("fopen");
        return;
    }
    fprintf(f, "{\n  \"project_name\": \"linux-6.16\",\n  \"files\": [\n");
    pthread_mutex_lock(&g_results_mutex);
    FileResultNode *cur = g_results_head;
    bool first = true;
    while (cur) {
        FileResult *r = cur->result;
        if (!first) fprintf(f, ",\n"); else first = false;
        fprintf(f, "    {\n");
        fprintf(f, "      \"file_id\": \"%s\",\n", r->file_id);
        fprintf(f, "      \"filename\": \"%s\",\n", r->filename);
        fprintf(f, "      \"path\": \"%s\",\n", r->relpath);
        fprintf(f, "      \"lines\": %d,\n", r->lines);
        fprintf(f, "      \"lines_of_code\": %d,\n", r->loc);
        fprintf(f, "      \"comment_ratio\": %.3f,\n", r->comment_ratio);
        fprintf(f, "      \"functions_count\": %d,\n", r->functions_count);
        fprintf(f, "      \"complexity\": %d,\n", r->complexity);
        fprintf(f, "      \"functions\": [");
        for (int i=0; i<r->functions_count; i++) {
            if (i>0) fprintf(f, ", ");
            fprintf(f, "\"%s\"", r->functions[i]);
        }
        fprintf(f, "],\n");
        fprintf(f, "      \"dependencies_count\": %d,\n", r->dependencies_count);
        fprintf(f, "      \"dependencies\": [");
        for (int i=0; i<r->dependencies_count; i++) {
            if (i>0) fprintf(f, ", ");
            fprintf(f, "\"%s\"", r->dependencies[i]);
        }
        fprintf(f, "],\n");
        fprintf(f, "      \"sha256\": \"%s\",\n", r->sha256);

        // Запись сжатого контента в отдельный файл
        char content_filename[MAX_PATH];
        snprintf(content_filename, MAX_PATH, "content_%s.gz", r->file_id);
        FILE *fc = fopen(content_filename, "wb");
        if (fc) {
            fwrite(r->compressed_content, 1, r->compressed_size, fc);
            fclose(fc);
            fprintf(f, "      \"content_file\": \"%s\"\n", content_filename);
        } else {
            fprintf(f, "      \"content_file\": null\n");
        }

        fprintf(f, "    }");
        cur = cur->next;
    }
    pthread_mutex_unlock(&g_results_mutex);
    fprintf(f, "\n  ]\n}\n");
    fclose(f);
}

// Очистка результатов
void free_results() {
    pthread_mutex_lock(&g_results_mutex);
    FileResultNode *cur = g_results_head;
    while (cur) {
        FileResultNode *next = cur->next;
        FileResult *r = cur->result;
        for (int i=0; i<r->functions_count; i++) free(r->functions[i]);
        free(r->functions);
        for (int i=0; i<r->dependencies_count; i++) free(r->dependencies[i]);
        free(r->dependencies);
        free(r->compressed_content);
        free(r);
        free(cur);
        cur = next;
    }
    g_results_head = g_results_tail = NULL;
    pthread_mutex_unlock(&g_results_mutex);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <path-to-project>\n", argv[0]);
        return 1;
    }

    // Инициализация очереди задач
    memset(&g_task_queue, 0, sizeof(g_task_queue));
    pthread_mutex_init(&g_task_queue.mutex, NULL);
    pthread_cond_init(&g_task_queue.cond, NULL);

    // Запуск thread pool
    for (int i=0; i<THREAD_POOL_SIZE; i++) {
        pthread_create(&g_threads[i], NULL, worker_thread, NULL);
    }

    // Обход каталогов и добавление задач
    scan_directory(argv[1], "");

    // Сигнал остановки, когда очередь пустая
    pthread_mutex_lock(&g_task_queue.mutex);
    g_task_queue.stop = true;
    pthread_cond_broadcast(&g_task_queue.cond);
    pthread_mutex_unlock(&g_task_queue.mutex);

    // Ожидание потоков
    for (int i=0; i<THREAD_POOL_SIZE; i++) {
        pthread_join(g_threads[i], NULL);
    }

    // Сохранение результата
    save_json("output.json");

    // Освобождение памяти
    free_results();

    pthread_mutex_destroy(&g_task_queue.mutex);
    pthread_cond_destroy(&g_task_queue.cond);
    pthread_mutex_destroy(&g_results_mutex);

    printf("Analysis completed. Results in output.json and compressed content files.\n");

    return 0;
}

