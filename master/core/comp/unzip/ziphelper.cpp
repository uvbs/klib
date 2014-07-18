
#undef  _MSC_VER
#define _MSC_VER 1600

#include "ziphelper.h"

#include <stdio.h>
#include <tchar.h>
#include <string>

#include "unzip\zip.h"
#include "unzip\unzip.h"

#include <zlib.h>
#include <direct.h>

#ifdef _WIN32
#include <windows.h>
#endif




ziphelper::~ziphelper(void)
{
    close();
}

bool ziphelper::open(const char* lpszFilePath, zlib_filefunc_def* def /*= NULL*/)   
{   
    if(is_open())
    {
        close();   
    }

    handle_ = ::unzOpen2(lpszFilePath, def);   
    return (handle_ != NULL);   
}   

bool ziphelper::is_open(void) const
{
    return handle_ != NULL;
}   

void ziphelper::close()   
{   
    if(handle_==NULL) 
    {
        return;
    }

    ::unzClose(handle_);   
    handle_ = NULL;   
}   

bool ziphelper::get_global_info(unz_global_info *p) const   
{   
    if(!is_open()) 
    {
        return false;   
    }

    return (UNZ_OK == ::unzGetGlobalInfo(handle_, p));   
}   

int ziphelper::get_global_count(void) const   
{   
    if(!is_open()) 
    {
        return 0;   
    }

    unz_global_info info;   
    return (::unzGetGlobalInfo(handle_, &info) == UNZ_OK) ? info.number_entry : 0;   
}   

bool ziphelper::get_global_comment(char *szComment, uLong uSizeBuf) const   
{   
    if(!is_open()) 
    {
        return false;   
    }

    return (UNZ_OK == ::unzGetGlobalComment(handle_, szComment, uSizeBuf));   
}   

bool ziphelper::goto_first_file(void)   
{   
    if(!is_open()) 
    {
        return false;   
    }

    return (UNZ_OK == ::unzGoToFirstFile(handle_));   
}   

bool ziphelper::goto_next_file(void)   
{   
    if(!is_open()) 
    {
        return false;   
    }

    return (UNZ_OK == ::unzGoToNextFile(handle_));   
}   

bool ziphelper::get_current_file_info(unz_file_info *pfile_info,   
                                      char *szFileName /*= NULL*/,   
                                      uLong fileNameBufferSize /*= 0*/,   
                                      void *extraField /*= NULL*/,   
                                      uLong extraFieldBufferSize /*= 0*/,   
                                      char *szComment /*= NULL*/,   
                                      uLong commentBufferSize /*= 0*/) const   
{   
    if(!is_open()) 
    {
        return false;   
    }

    return (UNZ_OK == ::unzGetCurrentFileInfo(handle_,   
        pfile_info,   
        szFileName,   
        fileNameBufferSize,   
        extraField,   
        extraFieldBufferSize,   
        szComment,   
        commentBufferSize));   
}   

int mymkdir(const char* dirname)
{
    int ret=0;
#ifdef WIN32
    ret = _mkdir(dirname);
#else
#ifdef unix
    ret = mkdir (dirname,0775);
#endif
#endif
    return ret;
}


int makedir (const char *newdir)
{
    char *buffer ;
    char *p;
    int  len = (int)strlen(newdir);

    if (len <= 0)
        return 0;

    buffer = (char*)malloc(len+1);
    strcpy(buffer, newdir);

    if (buffer[len-1] == '/' || buffer[len-1] == '\\') 
    {
        buffer[len-1] = '\0';

        if (mymkdir(buffer) == 0)
        {
            free(buffer);
            return 1;
        }
    }

    p = buffer + 1;
    while (1)
    {
        char hold;

        while(*p && *p != '\\' && *p != '/')
        {
            p++;
        }

        hold = *p;
        *p = 0;

        if (hold == 0)
            break;

        if ((mymkdir(buffer) == -1) && (errno == ENOENT))
        {
            printf("couldn't create directory %s\n",buffer);
            free(buffer);
            return 0;
        }

        *p++ = hold;
    }

    free(buffer);
    return 1;
}

bool ziphelper::extract(const char* dir)
{
    char filename_inzip[256];
    int err = UNZ_OK;
    bool ret = false;
    std::string str_root_path = dir;
    if (str_root_path.empty())
    {
        return false;
    }
    if (str_root_path[str_root_path.size() - 1] != '/' &&
        str_root_path[str_root_path.size() - 1] != '\\')
    {
        str_root_path.append("/");
    }

    unz_file_info file_info;
    //this->goto_first_file();

    int item_count = this->get_global_count();
    for (int index = 0; index < item_count; index++)
    {
        if (open_current_file())
        {
            ret = get_current_file_info(
                                &file_info,
                                filename_inzip,
                                sizeof(filename_inzip));

            if (!ret)
            {
                return false;
                break;
            }

            std::string str_target = str_root_path ;
            str_target.append(filename_inzip);
            
            if (0x0010 & file_info.external_fa)
            {
                makedir(str_target.c_str());
            }
            else
            {
                FILE* hfile = fopen(str_target.c_str(), "wb");
                if (nullptr == hfile)
                {
                    makedir(str_target.c_str());
                    hfile = fopen(str_target.c_str(), "wb");
                    if (nullptr == hfile)
                    {
                        this->close_current_file();
                        return false;
                    }
                }

                if (nullptr != hfile)
                {
                    this->extract_current_file(hfile);
                    fclose(hfile);
                }
            }

            this->close_current_file();
        }

        if ((index + 1) < item_count)
        {
            if (!this->goto_next_file())
            {
                return false;
            }
        }
    }

    return true;
}

bool ziphelper::open_current_file(const char* password /*= NULL*/)   
{   
    if(!is_open()) 
    {
        return false;   
    }

    return (UNZ_OK == ::unzOpenCurrentFilePassword(handle_, password));   
}   

int ziphelper::read_current_file(voidp buf, unsigned int len)   
{   
    if(!is_open()) 
    {
        return 0;   
    }

    return ::unzReadCurrentFile(handle_, buf, len);   
}   

bool ziphelper::extract_current_file(FILE* file)   
{
    char buf[2048];
    size_t size_buf = sizeof(buf);

    bool bOK = true;
    for (bOK = true; bOK; )   
    {   
        int bytes = read_current_file(buf, (unsigned int)size_buf);   
        if (bytes < 0)    
        {   
            bOK = false;   
        }    
        else if (bytes > 0)   
        {   
            if (fwrite(buf, bytes, 1, file) != 1)
            {   
                bOK = false;
            }   
        }   
        else   
        {   
            break;   
        }   
    }   
    return bOK;   
}   

bool ziphelper::close_current_file()   
{   
    if(!is_open()) 
    {
        return false;   
    }
    return (UNZ_OK == ::unzCloseCurrentFile(handle_));   
}   

bool ziphelper::attach(unzFile uf)   
{   
    if(is_open()) return false;   
    handle_=uf;   
    return handle_!=NULL;   
}   

unzFile ziphelper::detach(void)   
{   
    unzFile ret = handle_;   
    handle_=NULL;   
    return ret;   
}   

bool change_file_date(const char* filename, uLong dosdate, tm_unz tmu_date)   
{
#ifdef _WIN32
    HANDLE hFile = CreateFileA(filename,GENERIC_READ | GENERIC_WRITE, 0,NULL,OPEN_EXISTING,0,NULL);   
    bool bOK = (hFile != INVALID_HANDLE_VALUE);   
    if (bOK)   
    {   
        FILETIME ftm,ftLocal,ftLastAcc;
        bOK = (GetFileTime(hFile,NULL,&ftLastAcc,NULL)==TRUE);
        DosDateTimeToFileTime((WORD)(dosdate>>16),(WORD)dosdate,&ftLocal);
        LocalFileTimeToFileTime(&ftLocal,&ftm);
        SetFileTime(hFile,&ftm,&ftLastAcc,&ftm);
        CloseHandle(hFile);
    }   
    return bOK;

#else
    return false;
#endif
}

