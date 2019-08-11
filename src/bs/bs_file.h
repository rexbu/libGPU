/**
 * file :	bs_file.h
 * author :	Rex
 * create :	2018-11-04 13:20
 * func : 
 * history:
 */

#ifndef	__BS_FILE_H_
#define	__BS_FILE_H_

#include <dirent.h>
#include "bs_type.h"
#include "bs_object.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct {
    object_t    base;
    char        folder[1024];
    char        file[1024];
    char*       buffer;
    uint32_t    size;
}folder_t;

void* folder_init(void* p);
void folder_destroy(void* p);

status_t folder_open(folder_t* f, const char* folder);
const char* folder_type(folder_t* f, const char* type);
const char* folder_read(folder_t* f, const char* file);
const char* folder_read_file(folder_t* f, const char* file);
    
#ifdef __cplusplus
}
#endif
#endif
