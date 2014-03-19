#include "stdafx.h"
#define DRIVER
#define SHOWSTATS 
#define DUMP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "huffman.h"
#if defined(SHOWSTATS)
#define STATS(x) x
CountType HuffBytesHdr; 
CountType HuffBytesOut;
#else
#define STATS(x)
#endif
CountType HuffBytesIn;
struct _HuffTree {
	CountType count;
	LinkType child[2];
	LinkType parent;
	char *hcode;
	int bitcount;
} HuffTree[MAXNODES];
LinkType HuffRoot;
LinkType HuffCount;
char *HuffTable = NULL;
char *HuffTablePtr; 
unsigned HuffBytes;
#ifdef DUMP
static void HuffDump ( LinkType n )
{
	LinkType i;
	printf ( "root is %d\n", HuffRoot );
	printf ( "No. Synt count Parent Left Right BitNo Bits\n" );
	for ( i = 0; i <= n; i++ )
	{
		if ( HuffTree[i].count )
		{
			printf( "%3d.", i );
			if ( i < MAXSYMBOLS )
			{
				if ( isprint ( i ))
					printf ( " %c ",i );
				else
					printf ( "x%.2X", i );
			}
			else
				printf ( "n/a" );
			printf ( " %7ld %5d %5d %5d",HuffTree[i].count,HuffTree[i].parent,HuffTree[i].child[LEFT],HuffTree[i].child[RIGHT] );
			if ( HuffTree[i].hcode )
			{
				int j, k;
				int byte, bit;
				j = HuffTree[i].bitcount;
				printf ( " %5d ", j );
				for ( k = 0; k < j; k++ )
				{
					byte = k / 8;
					bit = k - byte * 8;
					if ( HuffTree[i].hcode[byte] & ( 1 << ( 7-bit )))
						printf ( "1" );
					else
						printf ( "0" );
				}
			}
			printf ( "\n" );
		}
	}
	printf( "\n" );
}
#define DUMPTREE(x) HuffDump(x)
#else
#define DUMPTREE(x)
#endif
/* scan the input file and acquire statistics */
static int HuffScan ( FILE *infile )
{
	int c;
	HuffBytesIn = 0;
	while (( c = fgetc ( infile )) != EOF )
	{
		HuffTree[c].count++;
		HuffBytesIn++;
	}
	if ( ferror ( infile ))
		return 1;
	else {
		rewind ( infile );
		return 0;
	}
}
/* walk the tree, either to get statistics or build bit table */
static void HuffWalk ( LinkType k, int depth )
{
	int dir, bitno, set, byte, bit;
	LinkType w, trace;
	for ( dir = 0; dir < 2; dir++ )
	{
		if ( HuffTree[k].child[dir] == -1 )
		{
			if (dir == LEFT) /* Left and right are same */
			{
				if ( ! HuffTablePtr )
					HuffBytes += ( depth +7 ) / 8;
				else
				{
					/* second pass, build the codes */
					HuffTree[k].bitcount = depth;
					HuffTree[k].hcode = HuffTablePtr;
					bitno = depth - 1;
					/* run up the parent links */
					for ( trace = k, w = HuffTree[k].parent;w; trace = w, w = HuffTree[w].parent )
					{
						/* which way did we come? */ 
						if ( HuffTree[w].child[LEFT] == trace )
							set = LEFT;
						else
							set = RIGHT;
						/* record the bit */
						if ( set )
						{
							byte = bitno / 8;
							bit = bitno - byte * 8;
							HuffTablePtr[byte] |=1 << ( 7 - bit );
						}
						bitno--;
					}
					HuffTablePtr += ( depth + 7 ) / 8;
				}
			}
		}
		else
			HuffWalk ( HuffTree[k].child[dir], depth + 1 );
	}
}
/* build the internal nodes */
static int HuffBuild()
{
	LinkType i, k, lo1, lo2;
	/* count active nodes */
	for ( i = 0; i < MAXSYMBOLS; i++ )
		if ( HuffTree[i].count )
			HuffCount++;
		/* ensure we have at least two active nodes */
		if ( HuffCount < 1 )
			return 2;
		if ( HuffCount == 1 )
		{
			for ( i = 0; i < MAXSYMBOLS; i++ )
				if ( HuffTree[i].count == 0 )
				{
					HuffTree[i].count++;
					HuffCount++;
					break;
				}
		}
		HuffRoot = MAXSYMBOLS + HuffCount - 2;
		/* build the internal nodes */
		for ( i = MAXSYMBOLS; i <= HuffRoot; i++ )
		{
			/* first, find two smallest nodes */
			for ( k = 0; k < i; k++ )
			{
				if ( HuffTree[k].count && !HuffTree[k].parent )
				{
					lo1 = k;
					k++;
					break;
				}
			}
			for ( ; k < i; k++ )
			{
				if ( HuffTree[k].count && !HuffTree[k].parent )
				{
					lo2 = k;
					k++;
					break;
				}
			}
			for ( ; k < i; k++ )
			{
				if ( HuffTree[k].count && !HuffTree[k].parent )
				{
					if ( HuffTree[k].count < HuffTree[lo1].count )
						lo1=k;
					else if ( HuffTree[k].count < HuffTree[lo2].count )
						lo2=k;
				}
			}
			/* now, build the new node and update the tree */
			HuffTree[i].count = HuffTree[lo1].count+HuffTree[lo2].count;
			HuffTree[i].child[LEFT] = lo1;
			HuffTree[i].child[RIGHT] = lo2;
			HuffTree[lo1].parent = HuffTree[lo2].parent = i;
		}
		/* build the bit codes */
		HuffBytes = 0;
		HuffTablePtr = NULL;
		HuffWalk ( HuffRoot, 0 );
		HuffTable = (char *) malloc ( HuffBytes );
		memset ( HuffTable, 0, HuffBytes );
		if (! HuffTable )
			return 3;
		HuffTablePtr = HuffTable;
		HuffWalk ( HuffRoot, 0 );
		return 0;
}
/* create the output file */
static int HuffCompress ( FILE *infile, FILE *outfile ) 
{
	LinkType i;
	struct _Header header;
	int outchar, outbit, c;
	char sig[] = SIGNATURE;
	STATS ( HuffBytesHdr = 0 );
	STATS ( HuffBytesOut = 0 );
	/* signature bytes */
	fwrite( sig, strlen(sig) + 1, 1, outfile );
	STATS ( HuffBytesHdr+= strlen ( sig ) + 1 );
	/* the root pointer */
	fwrite ( &HuffRoot, sizeof ( HuffRoot ), 1, outfile );
	STATS ( HuffBytesHdr += sizeof ( HuffRoot ));
	/* the number of internal nodes */
	fwrite (&HuffCount, sizeof ( HuffCount ), 1, outfile );
	STATS ( HuffBytesHdr += sizeof ( HuffCount ));
	/* the character count */
	fwrite ( &HuffBytesIn, sizeof ( HuffBytesIn ), 1, outfile);
	STATS ( HuffBytesHdr += sizeof ( HuffBytesIn));
	/* the active nodes */
	for ( i = 0; i <= HuffRoot; i++ )
	{
		if ( HuffTree[i].count )
		{
			header.index = i;
			header.child[0] = HuffTree[i].child[0];
			header.child[1] = HuffTree[i].child[1];
			fwrite ( &header, sizeof ( header ), 1, outfile);
			STATS( HuffBytesHdr += sizeof ( header ));
		}
	}
	/* now, compress the input file */
	outchar = 0; /* build up output bytes here */
	outbit = 0;
	while (( c = fgetc ( infile )) != EOF )
	{
		int k, count, byte, bit, set;
		char *s = HuffTree[c].hcode;
		count = HuffTree[c].bitcount;
		/* translate character into bit codes */
		for ( k = 0; k < count; k++ )
		{
			byte = k / 8;
			bit = k - byte * 8;
			if ( s[byte] & ( 1 << (7-bit)))
				set = 1;
			else
				set = 0;
			if ( set )
				outchar |= 1 << ( 7 - outbit );
			outbit++;
			if ( outbit == 8 )
			{
				fputc ( outchar, outfile );
				outchar = 0;
				outbit = 0;
				STATS ( HuffBytesOut++ );
			}
		}
	}
	/* do the last byte, if necessary */
	if ( outbit )
	{
		fputc ( outchar, outfile );
		STATS ( HuffBytesOut++ );
	}
	if ( ferror ( infile ))
		return 1;
	if ( ferror ( outfile ))
		return 4;
	return 0;
}
int HuffEncode ( FILE *infile, FILE *outfile )
{
	int retval;
	LinkType i;
	/* set all counts to zero */
	HuffRoot = 0;
	HuffCount = 0;
	memset ( HuffTree, 0, sizeof ( HuffTree ));
	for ( i = 0; i < MAXSYMBOLS; i++)
	{
		HuffTree[i].child[LEFT] = -1;
		HuffTree[i].child[RIGHT] = -1;
	}
	/* do frequency counts */
	if ( retval = HuffScan ( infile ))
		goto done;
	/* build the tree */
	if ( retval = HuffBuild() )
		goto done;
	DUMPTREE ( HuffRoot );
	/* compress the data file */
	retval = HuffCompress ( infile, outfile );
#if defined(SHOWSTATS)
	if ( ! retval )
	{
		printf ( "The input file contained %lu bytes\n",HuffBytesIn );
		printf ( "The output file contained %lu header byte, and %lu data bytes\n",HuffBytesHdr, HuffBytesOut );
		printf ( "output file %lu%% the size of input file\n",(( HuffBytesHdr + HuffBytesOut ) * 100 ) / HuffBytesIn );
	}
#endif
done:
	fclose ( infile );
	fclose ( outfile );
	return ( retval );
}
#ifdef DRIVER
int main ( int argc, char *argv[] )
{
	FILE *infile, *outfile;
	int retval;
	if ( argc != 3 )
	{
		fprintf( stderr, "Usage: huffenc infile outfile\n" );
		return EXIT_FAILURE;
	}
	infile = fopen ( argv[1], "r+b" );
	if ( infile == NULL )
	{
		fprintf ( stderr, "can't open ts for input\n", argv[1] );
		return EXIT_FAILURE;
	}
	outfile = fopen ( argv[2], "w+b" );
	if ( outfile == NULL )
	{
		fprintf ( stderr, "can't open %s for output\n", argv[2]);
		return EXIT_FAILURE;
	}
	if ( retval = HuffEncode ( infile, outfile ))
	{
		printf( "compression failed: " );
		if ( retval == 1 )
			printf ( "input error\n" );
		else
			if ( retval == 2 )
				printf ( "empty tree\n" );
			else
				if ( retval == 3 )
					printf ( "malloc failed\n" );
				else
					if ( retval == 4 )
						printf ( "output error\n" );
					else
						printf("\n");
					return retval;
	}
	else
	{
		printf	( "%s was compressed into %s\n",argv[1],argv[2] );
		return EXIT_SUCCESS;
	}
}
#endif