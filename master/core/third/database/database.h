#ifndef DATABASE_H_
#define DATABASE_H_

#include <cstdio>

typedef unsigned int DBHASH;
typedef struct {
	void *iov_base; /* Pointer to data. */
	size_t iov_len; /* Length of data. */
} IOvec;

const int DB_INSERT  = 1; /* insert new record only */
const int DB_REPLACE = 2; /* replace existing record */
const int DB_STORE   = 3; /* replace or insert */

const int IDXLEN_MIN =      6; /* key, sep, start, sep, length, \n */
const int IDXLEN_MAX =   1024; /* arbitrary */
const int DATLEN_MIN =      2; /* data byte, newline */
const int DATLEN_MAX = 999999; /* arbitrary */

void err_dump(const char[]);

class DB {
 public:
 	void     Open(const char *);
	void     Close();
	char    *Fetch(const char *);
	int      Store(const char *, const char *, int);
	int      Delete(const char *);
	void     Rewind();
	char    *Nextrec(char *);
 private:
	void    _alloc(int);
	void    _free();
	void    _dodelete();
	int	    _find(const char *);
	int     _findfree(int, int);
	DBHASH  _hash(const char *);
	char   *_readdat();
	int     _readidx(int);
	int     _readptr(int);
	void    _writedat(const char *, int, int);
	void    _writeidx(const char *, int, int, int);
	void    _writeptr(int, int);
	// data
	FILE *idxfp;  /* fp for index file */
 	FILE *datfp;  /* fp for data file */
	char *idxbuf; /* malloc'ed buffer for index record */
	char *datbuf; /* malloc'ed buffer for data record*/
	char *name;   /* name db was opened under */
	int idxoff;   /* offset in index file of index record */
	int idxlen;
	int datoff;
	int datlen;
	int ptrval;   /* contents of chain ptr in index record */
	int ptroff;   /* chain ptr offset pointing to this idx record */
	int chainoff; /* offset of hash chain for this index record */
	int hashoff;  /* offset in index file of hash table */
	int nhash;    /* current hash table size */
};

#endif // DATABASE_H_