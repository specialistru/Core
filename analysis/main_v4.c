#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <clang-c/Index.h>
#include <openssl/sha.h>
#include <zlib.h>

#define MAX_PATH 4096
#define THREAD_POOL_SIZE 8

typedef struct {
    char filepath[MAX_PATH];
    char relpath[MAX_PATH];
} Task;

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

typedef struct {
    char file_id[33];
    char filename[MAX_PATH];
    char relpath[MAX_PATH];
    int lines;
    int loc;
    int functions_count;
    int complexity; // Можно считать суммарную сложность по функциям, здесь 0
    double comment_ratio;
    char sha256[65];
    char *compressed_content;
    size_t compressed_size;
    char **dependencies;
    int dependencies_count;
    char **functions;
} FileResult;

typedef struct FileResultNode {
    FileResult *result;
    struct FileResultNode *next;
} FileResultNode;

static TaskQueue g_task_queue;
static FileResultNode *g_results_head = NULL;
static FileResultNode *g_results_tail = NULL;
static pthread_mutex_t g_results_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t g_threads[THREAD_POOL_SIZE];

// Утилиты

static void generate_id(char *buf, size_t sz) {
    static unsigned int counter = 0;
    snprintf(buf, sz, "%08x%08x", (unsigned int)time(NULL), counter++);
}

static void calc_sha256(const char *data, size_t len, char out[65]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data, len);
    SHA256_Final(hash, &sha256);
    for (int i=0; i<SHA256_DIGEST_LENGTH; i++)
        sprintf(out + i*2, "%02x", hash[i]);
    out[64] = 0;
}

static int compress_content(const char *data, size_t data_len, char **out, size_t *out_len) {
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

static void analyze_source_text(const char *text, int *lines, int *loc, double *comment_ratio) {
    int line_count = 0, code_lines = 0, comment_lines = 0;
    bool in_block_comment = false;
    const char *p = text;
    char line[4096];
    while (*p) {
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

// Clang AST visitor для функций и include

typedef struct {
    FileResult *res;
} ClangVisitorData;

static CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, CXClientData client_data) {
    ClangVisitorData *data = (ClangVisitorData*)client_data;
    CXCursorKind kind = clang_getCursorKind(cursor);

    if (kind == CXCursor_FunctionDecl) {
        CXString name = clang_getCursorSpelling(cursor);
        const char *func_name = clang_getCString(name);
        if (func_name) {
            data->res->functions = realloc(data->res->functions, sizeof(char*) * (data->res->functions_count + 1));
            data->res->functions[data->res->functions_count++] = strdup(func_name);
        }
        clang_disposeString(name);
    } else if (kind == CXCursor_InclusionDirective) {
        CXFile included_file = clang_getIncludedFile(cursor);
        if (included_file) {
            CXString included_name = clang_getFileName(included_file);
            const char *inc_name_c = clang_getCString(included_name);
            if (inc_name_c) {
                data->res->dependencies = realloc(data->res->dependencies, sizeof(char*) * (data->res->dependencies_count + 1));
                data->res->dependencies[data->res->dependencies_count++] = strdup(inc_name_c);
            }
            clang_disposeString(included_name);
        }
    }

    return CXChildVisit_Recurse;
}

static bool parse_with_clang(const char *filepath, FileResult *res) {
    CXIndex index = clang_createIndex(0, 0);
    if (!index) return false;

    const char *args[] = {
        "-std=c11", "-I/usr/include", "-I/usr/local/include"
    };

    CXTranslationUnit unit = clang_parseTranslationUnit(
        index,
        filepath,
        args, sizeof(args)/sizeof(args[0]),
        NULL, 0,
        CXTranslationUnit_None
    );
    if (!unit) {
        clang_disposeIndex(index);
        return false;
    }

    ClangVisitorData data = {.res = res};
    clang_visitChildren(clang_getTranslationUnitCursor(unit), visitor, &data);

    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);

    return true;
}

static char *load_file(const char *filepath, size_t *out_size) {
    FILE *f = fopen(filepath, "rb");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long size = ftell(f);
    if (size < 0) { fclose(f); return NULL; }
    if (fseek(f, 0, SEEK_SET) != 0) { fclose(f); return NULL; }
    char *buf = malloc(size + 1);
    if (!buf) { fclose(f); return NULL; }
    size_t read_size = fread(buf, 1, size, f);
    fclose(f);
    if (read_size != (size_t)size) { free(buf); return NULL; }
    buf[size] = 0;
    if (out_size) *out_size = size;
    return buf;
}

static void add_result(FileResult *res) {
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

static void process_file(Task *task) {
    FileResult *res = calloc(1, sizeof(FileResult));
    if (!res) return;
    generate_id(res->file_id, sizeof(res->file_id));

    // filename
    const char *slash = strrchr(task->filepath, '/');
    if (!slash) slash = strrchr(task->filepath, '\\');
    if (slash) strncpy(res->filename, slash + 1, MAX_PATH - 1);
    else strncpy(res->filename, task->filepath, MAX_PATH - 1);

    strncpy(res->relpath, task->relpath, MAX_PATH - 1);

    size_t size = 0;
    char *content = load_file(task->filepath, &size);
    if (!content) {
        fprintf(stderr, "Failed to load file %s\n", task->filepath);
        free(res);
        return;
    }

    analyze_source_text(content, &res->lines, &res->loc, &res->comment_ratio);

    if (!parse_with_clang(task->filepath, res)) {
        fprintf(stderr, "Clang parse failed for %s\n", task->filepath);
        // продолжаем — просто без функций/зависимостей
    }

    calc_sha256(content, size, res->sha256);

    if (compress_content(content, size, &res->compressed_content, &res->compressed_size) != 0) {
        fprintf(stderr, "Compression failed for %s, storing raw\n", task->filepath);
        res->compressed_content = malloc(size);
        if (res->compressed_content) {
            memcpy(res->compressed_content, content, size);
            res->compressed_size = size;
        }
    }

    free(content);

    add_result(res);
}

static void *worker_thread(void *arg) {
    (void)arg;
    while (true) {
        pthread_mutex_lock(&g_task_queue.mutex);
        while (!g_task_queue.head && !g_task_queue.stop) {
            pthread_cond_wait(&g_task_queue.cond, &g_task_queue.mutex);
        }
        if (g_task_queue.stop && !g_task_queue.head) {
            pthread_mutex_unlock(&g_task_queue.mutex);
            break;
        }
        TaskNode *node = g_task_queue.head;
        g_task_queue.head = node->next;
        if (!g_task_queue.head) g_task_queue.tail = NULL;
        pthread_mutex_unlock(&g_task_queue.mutex);

        process_file(&node->task);
        free(node);
    }
    return NULL;
}

static void enqueue_task(const char *filepath, const char *relpath) {
    TaskNode *node = malloc(sizeof(TaskNode));
    if (!node) return;
    strncpy(node->task.filepath, filepath, MAX_PATH - 1);
    node->task.filepath[MAX_PATH - 1] = 0;
    strncpy(node->task.relpath, relpath, MAX_PATH - 1);
    node->task.relpath[MAX_PATH - 1] = 0;
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

static void scan_directory(const char *base_path, const char *relpath) {
    DIR *dir = opendir(base_path);
    if (!dir) {
        fprintf(stderr, "opendir failed: %s\n", base_path);
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        char fullpath[MAX_PATH];
        char newrel[MAX_PATH];

        if (strlen(base_path) + strlen(entry->d_name) + 2 > MAX_PATH) {
            fprintf(stderr, "Path too long, skipping: %s/%s\n", base_path, entry->d_name);
            continue;
        }

        snprintf(fullpath, MAX_PATH, "%s/%s", base_path, entry->d_name);

        if (strlen(relpath) == 0)
            snprintf(newrel, MAX_PATH, "%s", entry->d_name);
        else
            snprintf(newrel, MAX_PATH, "%s/%s", relpath, entry->d_name);

        struct stat st;
        if (stat(fullpath, &st) != 0) {
            fprintf(stderr, "stat failed: %s\n", fullpath);
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            scan_directory(fullpath, newrel);
        } else if (S_ISREG(st.st_mode)) {
            const char *ext = strrchr(entry->d_name, '.');
            if (ext && (strcmp(ext, ".c") == 0 || strcmp(ext, ".h") == 0 || strcmp(ext, ".cpp") == 0 || strcmp(ext, ".hpp") == 0)) {
                enqueue_task(fullpath, newrel);
            }
        }
    }
    closedir(dir);
}

static void save_json(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "Failed to open output file %s\n", filename);
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

        // Записываем сжатое содержимое в отдельный файл
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

static void free_results() {
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
        fprintf(stderr, "Usage: %s <path-to-project>\n", argv[0]);
        return 1;
    }

    memset(&g_task_queue, 0, sizeof(g_task_queue));
    pthread_mutex_init(&g_task_queue.mutex, NULL);
    pthread_cond_init(&g_task_queue.cond, NULL);

    for (int i=0; i<THREAD_POOL_SIZE; i++) {
        if (pthread_create(&g_threads[i], NULL, worker_thread, NULL) != 0) {
            fprintf(stderr, "Failed to create thread %d\n", i);
            return 1;
        }
    }

    scan_directory(argv[1], "");

    pthread_mutex_lock(&g_task_queue.mutex);
    g_task_queue.stop = true;
    pthread_cond_broadcast(&g_task_queue.cond);
    pthread_mutex_unlock(&g_task_queue.mutex);

    for (int i=0; i<THREAD_POOL_SIZE; i++) {
        pthread_join(g_threads[i], NULL);
    }

    save_json("output.json");

    free_results();

    pthread_mutex_destroy(&g_task_queue.mutex);
    pthread_cond_destroy(&g_task_queue.cond);
    pthread_mutex_destroy(&g_results_mutex);

    printf("Analysis completed. Results saved to output.json and content_*.gz files.\n");

    return 0;
}
