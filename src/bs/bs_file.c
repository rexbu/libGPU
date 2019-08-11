/**
 * file :	bs_file.c
 * author :	Rex
 * create :	2018-11-04 13:20
 * func : 
 * history:
 */

#include <dirent.h>
#include "bs_file.h"
#include "bs_common.h"

void* folder_init(void* p){
    folder_t* f = (folder_t*)p;
    memset(f, 0, sizeof(folder_t));
    return f;
}

void folder_destroy(void* p){
    folder_t* f = (folder_t*)p;
    if (f->buffer!=NULL) {
        free(f->buffer);
        f->buffer = NULL;
        f->size = 0;
    }
}

status_t folder_open(folder_t* f, const char* folder){
    DIR* dir = opendir(folder);
    bs_strcpy(f->folder, sizeof(f->folder), folder);
    if (dir!=NULL) {
        closedir(dir);
        return BS_SUCCESS;
    }
    
    return BS_NOTFOUND;
}

const char* folder_type(folder_t* f, const char* type){
    DIR* dir = opendir(f->folder);
    struct dirent*  ent;
    size_t len = strlen(type);
    while((ent=readdir(dir))!=NULL){
        // ios下为namlen, android下为rectlen
        size_t namelen = strlen(ent->d_name);
        if (memcmp(type, &ent->d_name[namelen - len], len)==0) {
            snprintf(f->file, sizeof(f->file), "%s/%s", f->folder, ent->d_name);
            closedir(dir);
            return f->file;
        }
    }
    closedir(dir);
    return NULL;
}

const char* folder_read(folder_t* f, const char* file){
    struct stat info;
    int st = stat(file, &info);
    if (st != 0) {
        return NULL;
    }
    
    if (f->buffer != NULL) {
        free(f->buffer);
    }
    
    f->buffer = (char*)malloc(info.st_size+1);
    int fd = open(file, O_RDONLY);
    read(fd, f->buffer, info.st_size);
    f->buffer[info.st_size] = '\0';
    f->size = (uint32_t)info.st_size;
    close(fd);
    
    return f->buffer;
}

const char* folder_read_file(folder_t* f, const char* file){
    char path[1024];
    snprintf(path, sizeof(path), "%s/%s", f->folder, file);
    return folder_read(f, path);
}
