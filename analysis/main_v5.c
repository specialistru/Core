// analyzer_win.c
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <clang-c/Index.h>
#include <openssl/sha.h>
#include <zlib.h>

#define MAX_PATH_LEN 4096
#define THREAD_POOL_SIZE 8

typedef struct Task { char filepath[MAX_PATH_LEN], relpath[MAX_PATH_LEN]; struct Task *next; } Task;

static Task *task_head = NULL, *task_tail = NULL;
static CRITICAL_SECTION task_cs;
static CONDITION_VARIABLE task_cv;
static bool stop_flag = false;

typedef struct FileResult { char file_id[33], filename[MAX_PATH_LEN], relpath[MAX_PATH_LEN]; int lines, loc, func_count; double comment_ratio; char sha256[65]; char *compressed; size_t comp_size; char **functions, **dependencies; int dep_count; struct FileResult *next; } FileResult;

static FileResult *results_head = NULL, *results_tail = NULL;
static CRITICAL_SECTION results_cs;

// Утилиты: uuid, sha256, gzip compress…

void generate_id(char *buf) {
    static unsigned int counter = 0;
    snprintf(buf, 33, "%08x%08x", (unsigned int)time(NULL), counter++);
}
void calc_sha256(const char *data, size_t len, char out[65]) {
    unsigned char h[SHA256_DIGEST_LENGTH]; SHA256_CTX s; SHA256_Init(&s); SHA256_Update(&s,data,len); SHA256_Final(h,&s);
    for(int i=0;i<32;i++) sprintf(out+i*2,"%02x",h[i]); out[64]=0;
}
int compress_content(const char *data, size_t len, char **out, size_t *out_len) {
    uLongf bz = compressBound(len); *out = malloc(bz);
    if (!*out) return -1;
    if (compress2((Bytef*)*out, &bz, (const Bytef*)data, len, Z_BEST_COMPRESSION)!=Z_OK) { free(*out); return -1; }
    *out_len = bz; return 0;
}
void analyze_text(const char *text, int *lines, int *loc, double *comment_ratio) {
    int l=0, c=0, cm=0; bool in_block=false;
    for(const char *p=text; *p;) {
        const char *nl=strchr(p,'\n');
        size_t len= nl? (nl-p): strlen(p);
        char buf[4096]; strncpy(buf,p,len<4096?len:4095); buf[len]=0;
        l++;
        char *t=buf; while (*t && (unsigned char)*t<=32) t++;
        if (!*t) { p=nl?nl+1:p+len; continue; }
        if (in_block) { cm++; if (strstr(t,"*/")) in_block=false; }
        else if (strncmp(t,"/*",2)==0) { cm++; if (!strstr(t,"*/")) in_block=true; }
        else if (strncmp(t,"//",2)==0) cm++;
        else c++;
        if (nl) p=nl+1; else p+=len;
    }
    *lines=l; *loc=c; *comment_ratio= l? (double)cm/l:0.0;
}

// Clang visitor
typedef struct { FileResult *res; } CV;
CXChildVisitResult clang_visitor(CXCursor cur, CXCursor parent, CXClientData cd) {
    CV *cv=(CV*)cd;
    if (clang_getCursorKind(cur)==CXCursor_FunctionDecl) {
        CXString s=clang_getCursorSpelling(cur);
        const char *nm=clang_getCString(s);
        if (nm) {
            cv->res->functions = realloc(cv->res->functions,(cv->res->res_count+1)*sizeof(char*));
            cv->res->functions[cv->res->res_count++]=_strdup(nm);
        }
        clang_disposeString(s);
    } else if (clang_getCursorKind(cur)==CXCursor_InclusionDirective) {
        CXFile f = clang_getIncludedFile(cur);
        if (f) {
            CXString fn = clang_getFileName(f);
            const char *nm=clang_getCString(fn);
            if (nm) {
                cv->res->dependencies = realloc(cv->res->dependencies,(cv->res->dep_count+1)*sizeof(char*));
                cv->res->dependencies[cv->res->dep_count++]=_strdup(nm);
            }
            clang_disposeString(fn);
        }
    }
    return CXChildVisit_Recurse;
}

bool parse_with_clang(const char *fp, FileResult *res) {
    CXIndex idx = clang_createIndex(0,0);
    const char *args[] = {"-std=c11","-I.","-I/usr/include"};
    CXTranslationUnit tu = clang_parseTranslationUnit(idx,fp,args,2,NULL,0,CXTranslationUnit_None);
    if (!tu) { clang_disposeIndex(idx); return false; }
    CV cv={res};
    clang_visitChildren(clang_getTranslationUnitCursor(tu),clang_visitor,&cv);
    clang_disposeTranslationUnit(tu);
    clang_disposeIndex(idx);
    return true;
}

char *load_file(const char *fp, size_t *size) {
    FILE *f=fopen(fp,"rb"); if(!f) return NULL;
    fseek(f,0,SEEK_END); long sz=ftell(f); fseek(f,0,SEEK_SET);
    char *buf=malloc(sz+1); fread(buf,1,sz,f); fclose(f); buf[sz]=0; if(size)*size=sz; return buf;
}

// Очередь задач
void enqueue(const char *fp, const char *rel) {
    Task *t=malloc(sizeof(Task)); strcpy(t->filepath,fp); strcpy(t->relpath,rel); t->next=NULL;
    EnterCriticalSection(&task_cs);
    if(task_tail) task_tail->next=t; else task_head=t;
    task_tail=t; WakeConditionVariable(&task_cv);
    LeaveCriticalSection(&task_cs);
}

// Поток-воркер
DWORD WINAPI worker(LPVOID _) {
    while(true) {
        EnterCriticalSection(&task_cs);
        while(!task_head && !stop_flag) SleepConditionVariableCS(&task_cv,&task_cs,INFINITE);
        if(stop_flag && !task_head) { LeaveCriticalSection(&task_cs); break; }
        Task *t=task_head; task_head=t->next; if(!task_head) task_tail=NULL;
        LeaveCriticalSection(&task_cs);

        FileResult *res=calloc(1,sizeof(FileResult));
        generate_id(res->file_id);
        strcpy(res->relpath,t->relpath);
        const char *slash = strrchr(t->filepath,'\\'); if (!slash) slash=strrchr(t->filepath,'/'); strcpy(res->filename, slash?slash+1:t->filepath);

        size_t size; char *content=load_file(t->filepath, &size);
        if (!content) { free(res); free(t); continue; }
        analyze_text(content,&res->lines,&res->loc,&res->comment_ratio);
        parse_with_clang(t->filepath,res);
        calc_sha256(content,size,res->sha256);
        if (compress_content(content,size,&res->compressed,&res->comp_size)!=0) { res->compressed=content; res->comp_size=size; } else free(content);

        EnterCriticalSection(&results_cs);
        if(results_tail) results_tail->next=res; else results_head=res;
        results_tail=res;
        LeaveCriticalSection(&results_cs);

        free(t);
    }
    return 0;
}

// Обход WinAPI
void scan_dir(const char *base, const char *rel) {
    char search[MAX_PATH_LEN]; snprintf(search,MAX_PATH_LEN,"%s\\*",base);
    WIN32_FIND_DATAA fd; HANDLE h=FindFirstFileA(search,&fd);
    if(h==INVALID_HANDLE_VALUE) return;
    do {
        if (!strcmp(fd.cFileName,".") || !strcmp(fd.cFileName,"..")) continue;
        char full[MAX_PATH_LEN], nrel[MAX_PATH_LEN];
        snprintf(full,MAX_PATH_LEN,"%s\\%s",base,fd.cFileName);
        snprintf(nrel,MAX_PATH_LEN,rel[0]? "%s\\%s": "%s", rel, fd.cFileName);
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) scan_dir(full,nrel);
        else {
            const char *ext = strrchr(fd.cFileName,'.');
            if (ext && (_stricmp(ext,".c")==0 || _stricmp(ext,".h")==0 || _stricmp(ext,".cpp")==0 || _stricmp(ext,".hpp")==0))
                enqueue(full,nrel);
        }
    } while(FindNextFileA(h,&fd));
    FindClose(h);
}

// Сохранение JSON
void save_json(const char *outfn) {
    FILE *f = fopen(outfn,"w"); if (!f) return;
    fprintf(f,"{\"project_name\":\"MyProject\",\"files\":[\n");
    EnterCriticalSection(&results_cs);
    FileResult *cur=results_head; bool first=true;
    while(cur) {
        if(!first) fprintf(f,",\n");
        first=false;
        fprintf(f,"{\"file_id\":\"%s\",\"filename\":\"%s\",\"path\":\"%s\",\"lines\":%d,\"lines_of_code\":%d,\"comment_ratio\":%.3f,\"functions_count\":%d,\"sha256\":\"%s\",\"dependencies_count\":%d",
            cur->file_id,cur->filename,cur->relpath,cur->lines,cur->loc,cur->comment_ratio,cur->func_count,cur->sha256,cur->dep_count);
        fprintf(f,",\"functions\":[");
        for(int i=0;i<cur->func_count;i++){ if(i)fprintf(f,","); fprintf(f,"\"%s\"",cur->functions[i]); }
        fprintf(f="]"); 
        fprintf(f,",\"dependencies\":[");
        for(int i=0;i<cur->dep_count;i++){ if(i)fprintf(f,","); fprintf(f,"\"%s\"",cur->dependencies[i]); }
        fprintf(f="]"); 
        fprintf(f);

        // Content file
        char cf[MAX_PATH_LEN]; snprintf(cf,MAX_PATH_LEN,"content_%s.gz",cur->file_id);
        FILE *fc = fopen(cf,"wb");
        if(fc){ fwrite(cur->compressed,1,cur->comp_size,fc); fclose(fc); fprintf(f,",\"content_file\":\"%s\"}",cf); }
        else fprintf(f,",\"content_file\":null}");

        cur=cur->next;
    }
    LeaveCriticalSection(&results_cs);
    fprintf(f,"]}");
    fclose(f);
}

int main(int argc,char**argv){
    if(argc<2){ printf("Usage: %s <project_path>\n",argv[0]); return 1; }
    InitializeCriticalSection(&task_cs);
    InitializeCriticalSection(&results_cs);
    SleepConditionVariableCS(&task_cv,&task_cs,0);

    for(int i=0;i<THREAD_POOL_SIZE;i++) CreateThread(NULL,0,worker,NULL,0,NULL);
    scan_dir(argv[1],"");

    EnterCriticalSection(&task_cs);
    stop_flag=true;
    WakeAllConditionVariable(&task_cv);
    LeaveCriticalSection(&task_cs);

    Sleep(1); // wait threads
    save_json("output.json");

    DeleteCriticalSection(&task_cs);
    DeleteCriticalSection(&results_cs);

    printf("Done.\n");
    return 0;
}

