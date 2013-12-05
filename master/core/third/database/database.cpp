#include "database.h"
#include <string.h>
#include <stdlib.h>

#define IDXLEN_SZ     9 /* index record length (ASCII chars) */
#define SEP         ':' /* separator char in index record */
#define SPACE       ' ' /* space character */
#define NEWLINE    '\n' /* newline character */

#define PTR_SZ          9	/* size of ptr field in hash chain */
#define PTR_MAX  99999999	/* max file offset = 10**PTR_SZ - 1 */
#define NHASH_DEF   99991	/* default hash table size */
#define FREE_OFF        0	/* free list offset in index file */
#define HASH_OFF   PTR_SZ	/* hash table offset in index file */

void err_dump(const char *msg)
{
	printf("%s\n", msg);
	getchar();
	exit(0);
}

void DB::Open(const char *pathname)
{
	int len;
	char asciiptr[PTR_SZ + 1];
	char hash[(NHASH_DEF + 1) * PTR_SZ + 2]; /* +2 for newline and null */

	// Allocate a DB structure, and the buffers it needs.
	len = strlen(pathname);
	_alloc(len);

	// Initialize hash data.
	this->nhash   = NHASH_DEF;/* hash table size */
	this->hashoff = HASH_OFF;	/* offset in index file of hash table */

	// Open index file and data file.
	strcpy(this->name, pathname);
	strcat(this->name, ".idx");
	this->idxfp = fopen(this->name, "ab+");
	if (this->idxfp != NULL)
		fclose(this->idxfp);
	this->idxfp = fopen(this->name, "rb+");
	strcpy(this->name + len, ".dat");
	this->datfp = fopen(this->name, "ab+");
	if (this->datfp != NULL)
		fclose(this->datfp);
	this->datfp = fopen(this->name, "rb+");

	if (this->idxfp == NULL || this->datfp == NULL) {
		_free();
		return ;
	}
	
	if (fgetc(this->idxfp) == EOF)
	{
		sprintf(asciiptr, "%*d", PTR_SZ, 0);
		hash[0] = 0;
		int i;
		for (i = 0; i < NHASH_DEF + 1; i++)
			strcat(hash, asciiptr);
		strcat(hash, "\n");
		i = strlen(hash);
		if (fwrite(&hash, 1, i, this->idxfp) != i)
			err_dump("Open: index file init write error");
	}
	Rewind();
}

void DB::Close()
{
	_free(); /* closes fps, free buffers & struct */
}

char *DB::Fetch(const char *key)
{
	char *ptr;

	if (_find(key) < 0)
		ptr = NULL;				/* error*/
	else
		ptr = _readdat();	/* return pointer to data */

	return ptr;
}

int DB::Store(const char *key, const char *data, int flag)
{
	int rc, keylen, datlen;
	int pval;

	if (flag != DB_INSERT &&
		flag != DB_REPLACE &&
		flag != DB_STORE)
		return -1;
	keylen = strlen(key);
	datlen = strlen(data) + 1; // +1 for newline at end
	if (datlen < DATLEN_MIN || datlen > DATLEN_MAX)
		err_dump("Store: invalid data length");

	// _find calculates which hash table this new record
	// goes into (this->chainoff), regardless of whether it already
	// exists or not. The following calls to _writeptr change the
	// hash table entry for this chain to point to the new record.
	// The new record is added to the front of the hash chain.
	if (_find(key) < 0) { /* record not found */
		if (flag == DB_REPLACE) {
			rc = -1;
			goto doreturn;
		}

		// read the chain ptr to the first index record on hash chain.
		pval = _readptr(this->chainoff);

		if (_findfree(keylen, datlen) < 0) {
			// Can't find an empty record big enough. Append the
			// new record to the ends of the index and data files.
			_writedat(data, 0, SEEK_END);
			_writeidx(key, 0, SEEK_END, pval);

			// this->idxoff was set by _writeidx.  The new
			// record goes to the front of the hash chain.
			_writeptr(this->chainoff, this->idxoff);
		} else {
			// Reuse an empty record. _findfree removed it from
			// the free list and set both this->datoff and this->idxoff.
			// Reused record goes to the front of the hash chain.
			_writedat(data, this->datoff, SEEK_SET);
			_writeidx(key, this->idxoff, SEEK_SET, pval);
			_writeptr(this->chainoff, this->idxoff);
		}
	} else { // record found
		if (flag == DB_INSERT) {
			rc = 1; // error, record already in this
			goto doreturn;
		}

		// We are replacing an existing record.  We know the new
		// key equals the existing key, but we need to check if
		// the data records are the same size.
		if (datlen != this->datlen) {
			_dodelete();	/* delete the existing record */

			// Reread the chain ptr in the hash table
			// (it may change with the deletion).
			pval = _readptr(this->chainoff);

			// Append new index and data records to end of files.
			_writedat(data, 0, SEEK_END);
			_writeidx(key, 0, SEEK_END, pval);

			// New record goes to the front of the hash chain.
			_writeptr(this->chainoff, this->idxoff);
		} else {
			// Same size data, just replace data record.
			_writedat(data, this->datoff, SEEK_SET);
		}
	}
	rc = 0;		/* OK */

doreturn:
	return rc;
}

int DB::Delete(const char *key)
{
	int rc = 0; // assume record will be found

	if (_find(key) == 0)
		_dodelete();
	else
		rc = -1;

	return rc;
}

void DB::Rewind()
{
	int offset;
	offset = (this->nhash + 1) * PTR_SZ; /* +1 for free list ptr */
	offset ++; // +1 for newline in the end

	// We're just setting the file offset for this process
	// to the start of the index records; no need to lock.
	// +1 below for newline at end of hash table.
	if (fseek(this->idxfp, offset, SEEK_SET) == -1)
		err_dump("Rewind: fseek error");
	this->idxoff = ftell(this->idxfp);
}

char *DB::Nextrec(char *key)
{
	char c;
	char *ptr;

	do {
		// Read next sequential index record.
		if (_readidx(0) < 0) {
			ptr = NULL; // end of index file, EOF.
			goto doreturn;
		}

		// Check if key is all blank (empty record).
		ptr = this->idxbuf;
		while ((c = *ptr++) != 0  &&  c == SPACE)
			continue ;	/* skip until null byte or nonblank */
	} while (c == 0);	/* loop until a nonblank key is found */

	if (key != NULL)
		strcpy(key, this->idxbuf);	/* return key */
	ptr = _readdat();	/* return pointer to data buffer */
	
doreturn:
	return ptr;
}

void DB::_alloc(int namelen)
{
	// Use calloc, to initialize the structure to zero.
	idxfp = NULL;
	datfp = NULL;

	// Allocate room for the name.
	if ((name = static_cast<char *>(malloc(namelen + 5))) == NULL)
		err_dump("_alloc: malloc error for name");

	// Allocate an index buffer and a data buffer.
	if ((idxbuf = static_cast<char *>(malloc(IDXLEN_MAX + 2))) == NULL)
		err_dump("_alloc: malloc error for index buffer");
	if ((datbuf = static_cast<char *>(malloc(DATLEN_MAX + 2))) == NULL)
		err_dump("_alloc: malloc error for data buffer");
}

void DB::_free()
{
	if (this->idxfp != NULL)
		fclose(this->idxfp);
	if (this->datfp != NULL)
		fclose(this->datfp);
	if (this->idxbuf != NULL)
		free(this->idxbuf);
	if (this->datbuf != NULL)
		free(this->datbuf);
	if (this->name != NULL)
		free(this->name);
}

void DB::_dodelete()
{
	char *ptr;
	int i;

	// Set data buffer and key to all blanks.
	for (ptr = this->datbuf, i = 0; i < this->datlen - 1; i++)
		*ptr++ = SPACE;
	*ptr = 0; // null terminate for _writedat
	ptr = this->idxbuf;
	while (*ptr)
		*ptr++ = SPACE;

	// Write the data record with all blanks.
	_writedat(this->datbuf, this->datoff, SEEK_SET);

	// Read the free list pointer.  Its value becomes the
	// chain ptr field of the deleted index record.  This means
	// the deleted record becomes the head of the free list.
	int freeptr, saveptr;
	freeptr = _readptr(FREE_OFF);

	// Save the contents of index record chain ptr,
	// before it's rewritten by _writeidx.
	saveptr = this->ptrval;

	// Rewrite the index record.  This also rewrites the length
	// of the index record, the data offset, and the data length,
	// none of which has changed, but that's OK.
	_writeidx(this->idxbuf, this->idxoff, SEEK_SET, freeptr);

	// Write the new free list pointer.
	_writeptr(FREE_OFF, this->idxoff);

	// Rewrite the chain ptr that pointed to this record being
	// deleted.  Recall that _find sets this->ptroff to
	// point to this chain ptr.  We set this chain ptr to the
	// contents of the deleted record's chain ptr, saveptr.
	_writeptr(this->ptroff, saveptr);
}

int DB::_find(const char *key)
{
	// Calculate the hash value for this key, then calculate the
	// byte offset of corresponding chain ptr in hash table.
	// This is where our search starts.  First we calculate the
	// offset in the hash table for this key.
	this->chainoff = (_hash(key) * PTR_SZ) + this->hashoff;
	this->ptroff = this->chainoff;

	// Get the offset in the index file of first record
	// on the hash chain (can be 0).
	int offset, nextoffset;
	offset = _readptr(this->ptroff);
	while (offset != 0) {
		nextoffset = _readidx(offset);
		if (strcmp(this->idxbuf, key) == 0)
			break; // found a match
		this->ptroff = offset; /* offset of this (unequal) record */
		offset = nextoffset; /* next one to compare */
	}

	// offset == 0 on error (record not found).
	return (offset == 0 ? -1 : 0);
}

int DB::_findfree(int keylen, int datlen)
{
	int rc;
	int offset, nextoffset, saveoffset;
	
	// Read the free list pointer.
	saveoffset = FREE_OFF;
	offset = _readptr(saveoffset);

	while (offset != 0) {
		nextoffset = _readidx(offset);
		if (strlen(this->idxbuf) == keylen && this->datlen == datlen)
			break; // found a match
		saveoffset = offset;
		offset = nextoffset;
	}

	if (offset == 0)
		rc = -1; // no match found
	else {
		// Found a free record with matching sizes.
		// The index record was read in by _readidx above,
		// which sets this->ptrval.  Also, saveoffset points to
		// the chain ptr that pointed to this empty record on
		// the free list.  We set this chain ptr to this->ptrval,
		// which removes the empty record from the free list.
		_writeptr(saveoffset, this->ptrval);
		rc = 0;

		// Notice also that _readidx set both this->idxoff
		// and this->datoff.  This is used by the caller, Store,
		// to write the new index record and data record.
	}

	return rc;
}

DBHASH DB::_hash(const char *key)
{
	DBHASH hval = 0;
	while (*key)
	{
		// equivalent to: hash = 65599*hash + (*str++);
		hval = (*key++) + (hval << 6) + (hval << 16) - hval;
	}
	return (hval % NHASH_DEF);
}

char *DB::_readdat()
{
	if (fseek(this->datfp, this->datoff, SEEK_SET) == -1)
		err_dump("_readdat: fseek error");
	if (fread(this->datbuf, 1, this->datlen, this->datfp) != this->datlen)
		err_dump("_readdat: read error");
	if (this->datbuf[this->datlen-1] != NEWLINE) // sanity check
		err_dump("_readdat: missing newline");
	this->datbuf[this->datlen-1] = 0; // replace newline with null
	return this->datbuf; // return pointer to data record
}

int DB::_readidx(int offset)
{
	// Position index file and record the offset.  Nextrec
	// calls us with offset==0, meaning read from current offset.
	// We still need to call fseek to record the current offset.
	int seek = (offset == 0 ? SEEK_CUR : SEEK_SET);
	if (fseek(this->idxfp, offset, seek) == -1)
		err_dump("_readidx: fseek error");
	this->idxoff = ftell(this->idxfp);

	// Read the ascii chain ptr and the ascii length at
	// the front of the index record.  This tells us the
	// remaining size of the index record.
	char *ptr1, *ptr2;
	char asciiptr[PTR_SZ + 1], asciilen[IDXLEN_SZ + 1];
	IOvec iov[2];
	iov[0].iov_base = asciiptr;
	iov[0].iov_len  = PTR_SZ;
	iov[1].iov_base = asciilen;
	iov[1].iov_len  = IDXLEN_SZ;

	int i;
	i  = fread(iov[0].iov_base, 1, iov[0].iov_len, this->idxfp);
	i += fread(iov[1].iov_base, 1, iov[1].iov_len, this->idxfp);
	if (i != PTR_SZ + IDXLEN_SZ) {
		if (i == 0 && offset == 0)
			return -1;		/* EOF for Nextrec */
		err_dump("_readidx: fread error of index record");
	}

	// This is our return value; always >= 0.
	asciiptr[PTR_SZ] = 0;        /* null terminate */
	this->ptrval = atoi(asciiptr); /* offset of next key in chain */

	asciilen[IDXLEN_SZ] = 0;     /* null terminate */
	if ((this->idxlen = atoi(asciilen)) < IDXLEN_MIN ||
	  this->idxlen > IDXLEN_MAX)
		err_dump("_readidx: invalid length");

	// Now read the actual index record.  We read it into the key
	// buffer that we malloced when we opened the database.
	if ((i = fread(this->idxbuf, 1, this->idxlen, this->idxfp)) != this->idxlen)
		err_dump("_readidx: read error of index record");
	if (this->idxbuf[this->idxlen-1] != NEWLINE)	// sanity check
		err_dump("_readidx: missing newline");
	this->idxbuf[this->idxlen-1] = 0;	// replace newline with null

	// Find the separators in the index record.
	if ((ptr1 = strchr(this->idxbuf, SEP)) == NULL)
		err_dump("_readidx: missing first separator");
	*ptr1++ = 0; // replace SEP with null

	if ((ptr2 = strchr(ptr1, SEP)) == NULL)
		err_dump("_readidx: missing second separator");
	*ptr2++ = 0; // replace SEP with null

	if (strchr(ptr2, SEP) != NULL)
		err_dump("_readidx: too many separators");

	// Get the starting offset and length of the data record.
	if ((this->datoff = atoi(ptr1)) < 0)
		err_dump("_readidx: starting offset < 0");
	if ((this->datlen = atoi(ptr2)) <= 0 || this->datlen > DATLEN_MAX)
		err_dump("_readidx: invalid length");
	return this->ptrval; // return offset of next key in chain
}

int DB::_readptr(int offset)
{
	char asciiptr[PTR_SZ + 1];
	if (fseek(this->idxfp, offset, SEEK_SET) == -1)
		err_dump("_readptr: fseek error to ptr field");
	if (fread(&asciiptr, 1, PTR_SZ, this->idxfp) != PTR_SZ)
		err_dump("_readptr: read error of ptr field");
	asciiptr[PTR_SZ] = 0; // null terminate
	return atoi(asciiptr);
}

void DB::_writedat(const char *data, int offset, int whence)
{
	IOvec iov[2];
	static char newline = NEWLINE;

	if (fseek(this->datfp, offset, whence) == -1)
		err_dump("_writedat: fseek error");
	this->datoff = ftell(this->datfp);
	this->datlen = strlen(data) + 1;	// datlen includes newline
	iov[0].iov_base = const_cast<char *>(data);
	iov[0].iov_len  = this->datlen - 1;
	iov[1].iov_base = &newline;
	iov[1].iov_len  = 1;
	int writelen = 0;
	writelen  = fwrite(iov[0].iov_base, 1, iov[0].iov_len, this->datfp);
	writelen += fwrite(iov[1].iov_base, 1, iov[1].iov_len, this->datfp);
	if (writelen != this->datlen)
		err_dump("_writedat: fwrite error of data record");
}

void DB::_writeidx(const char *key, int offset, int whence, int pval)
{
	IOvec iov[2];
	char asciiptrlen[PTR_SZ + IDXLEN_SZ +1];
	int len;

	if ((this->ptrval = pval) < 0 || pval > PTR_MAX)
		err_dump("_writeidx: invalid ptr");
	sprintf(this->idxbuf, "%s%c%d%c%d\n", key, SEP, this->datoff, SEP, this->datlen);
	if ((len = strlen(this->idxbuf)) < IDXLEN_MIN || len > IDXLEN_MAX)
		err_dump("_writeidx: invalid length");
	sprintf(asciiptrlen, "%*d%*d", PTR_SZ, pval, IDXLEN_SZ, len);

	// debug
	//sprintf("%s\n", asciiptrlen);
	//system("pause");

	// Position the index file and record the offset.
	if (fseek(this->idxfp, offset, whence) == -1)
		err_dump("_writeidx: fseek error");
	this->idxoff = ftell(this->idxfp);

	iov[0].iov_base = asciiptrlen;
	iov[0].iov_len  = PTR_SZ + IDXLEN_SZ;
	iov[1].iov_base = this->idxbuf;
	iov[1].iov_len  = len;

	int writelen = 0;
	writelen  = fwrite(iov[0].iov_base, 1, iov[0].iov_len, this->idxfp);
	writelen += fwrite(iov[1].iov_base, 1, iov[1].iov_len, this->idxfp);
	if (writelen != PTR_SZ + IDXLEN_SZ + len)
		err_dump("_writeidx: fwrite error of index record");
}

void DB::_writeptr(int offset, int ptrval)
{
	char asciiptr[PTR_SZ + 1];

	if (ptrval < 0 || ptrval > PTR_MAX)
		err_dump("_writeptr: invalid ptr");
	sprintf(asciiptr, "%*d", PTR_SZ, ptrval);

	if (fseek(this->idxfp, offset, SEEK_SET) == -1)
		err_dump("_writeptr: fseek error to ptr field");
	if (fwrite(asciiptr, 1, PTR_SZ, this->idxfp) != PTR_SZ)
		err_dump("_writeptr: write error of ptr field");
}