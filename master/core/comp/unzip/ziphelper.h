#pragma once



#include <stdio.h>

#ifndef unzFile
typedef void* unzFile;
typedef void* voidp;
#endif

typedef struct zlib_filefunc_def_s zlib_filefunc_def;
typedef struct unz_file_info_s      unz_file_info;
typedef struct unz_global_info_s    unz_global_info;

typedef unsigned int   uInt;  /* 16 bits or more */
typedef unsigned long  uLong; /* 32 bits or more */

#include "../../inttype.h"

class ziphelper
{
public:   
    ziphelper() : handle_(nullptr) {  }   
    ~ziphelper() ;

    operator unzFile(void) { return handle_; } 

public:   
    bool open(const char* lpszFilePath, zlib_filefunc_def* def = nullptr) ;

    bool is_open(void) const ;
    void close();

    bool get_global_info(unz_global_info *p) const ;
    int get_global_count(void) const ;
    bool get_global_comment(char *szComment, uLong uSizeBuf) const ;

    bool goto_first_file(void) ;
    bool goto_next_file(void) ;
    bool get_current_file_info(unz_file_info *pfile_info,   
        char *szFileName = NULL,   
        uLong fileNameBufferSize = 0,   
        void *extraField = NULL,   
        uLong extraFieldBufferSize = 0,   
        char *szComment = NULL,   
        uLong commentBufferSize = 0) const  ;

    bool extract(const char* dir);
    bool open_current_file(const char* password = NULL) ;
    int read_current_file(voidp buf, unsigned int len) ;
    bool extract_current_file(FILE* file);
    bool close_current_file();

    bool attach(unzFile uf) ;
    unzFile detach(void) ;


private:   
    unzFile handle_;
};

