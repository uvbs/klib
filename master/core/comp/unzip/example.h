

class Zip   
{
public:   
    Zip(){_handle=NULL;}   
    ~Zip(){close();}   

public:   
    bool open(const char* lpszFilePath, bool bAppend = false, const char** globalcomment = NULL, zlib_filefunc_def* pzlib_filefunc_def = NULL)   
    {   
        if(is_open()) close();   

        _handle = ::zipOpen2(lpszFilePath, bAppend, globalcomment, pzlib_filefunc_def);   
        return (_handle != NULL);   
    }   

    void close()   
    {   
        if(!is_open()) return;   
        ::zipClose(_handle, NULL);   
        _handle=NULL;   
    }   

    bool add_new(FILE* file,   
        const char* filenameinzip,   
        void* buf,   
        size_t buf_size,   
        const char* comment = NULL,   
        int level = Z_BEST_COMPRESSION,   
        int method = Z_DEFLATED,                  
        const void* extrafield_local = NULL,   
        uInt size_extrafield_local = 0,   
        const void* extrafield_global = NULL,   
        uInt size_extrafield_global = 0   
        )   
    {   
        zip_fileinfo zi;   

        memset(&zi, 0, sizeof(zi));   
        bool bOK = (buf_size != 0) && filetime(file,&zi.tmz_date,&zi.dosDate);   
        if (bOK)   
        {   
            bOK = open_new(filenameinzip, &zi, comment, level, method, extrafield_local,    
                size_extrafield_local, extrafield_global, size_extrafield_global);   
        }   
        if (bOK)   
        {   
            for (unsigned int size_read;;)   
            {   
                size_read = (unsigned int)fread(buf, 1, buf_size, file);   
                if (size_read == 0)   
                {   
                    break;   
                }   
                if (!write_new(buf,size_read))   
                {   
                    break;   
                }                       
            }   
            bOK = close_new();   
        }   
        return bOK;   
    }   

    bool is_open(void) const   
    {   
        return (_handle!=NULL);   
    }   

private:   

    bool open_new(const char* filename,   
        const zip_fileinfo* zipfi,   
        const char* comment = NULL,   
        int level = Z_BEST_COMPRESSION,   
        int method = Z_DEFLATED,                  
        const void* extrafield_local = NULL,   
        uInt size_extrafield_local = 0,   
        const void* extrafield_global = NULL,   
        uInt size_extrafield_global = 0   
        )   
    {   
        return (ZIP_OK == ::zipOpenNewFileInZip(_handle, filename, zipfi, extrafield_local, size_extrafield_local,    
            extrafield_global, size_extrafield_global, comment,   
            (level != 0) ? method : 0,   
            level));   
    }   

    bool write_new(const voidp p, unsigned int len)   
    {   
        if(_handle==NULL) return false;   
        return (::zipWriteInFileInZip(_handle, p, len) >= 0);   
    }   

    bool close_new()   
    {   
        if(_handle==NULL) return false;   
        return (ZIP_OK == ::zipCloseFileInZip(_handle));   
    }   

    // Implementation   
private:   
    zipFile _handle;   
protected:   

    static bool filetime(const char *f, tm_zip* tmzip, unsigned long* dt)   
    {   
        WIN32_FIND_DATA ff32;   
        HANDLE handle = ::FindFirstFile(f,&ff32);   
        bool bOK = (handle != INVALID_HANDLE_value);   
        if (bOK)   
        {   
            FILETIME ftLocal;   
            ::FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftLocal);   
            ::FileTimeToDosDateTime(&ftLocal,((LPWORD)dt)+1,((LPWORD)dt)+0);   
            ::FindClose(handle);   
        }   

        return bOK;   
    }   
    static bool filetime(FILE* file , tm_zip* tmzip, unsigned long* dt)   
    {   
        FILETIME ftModify;   
        bool bOK = (::GetFileTime((HANDLE)_get_osfhandle((int)_fileno(file)), NULL, NULL, &ftModify)==TRUE);   
        if (bOK)   
        {   
            FILETIME ftLocal;   
            FileTimeToLocalFileTime(&ftModify,&ftLocal);   
            FileTimeToDosDateTime(&ftLocal,((LPWORD)dt)+1,((LPWORD)dt)+0);   
        }   
        return bOK;   
    }   
}; 