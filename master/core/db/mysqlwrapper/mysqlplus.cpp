///////////////////////////////////////////////////////////////////////////////////////
//// mysqlplus.cpp
///////////////////////////////////////////////////////////////////////////////////////

#include "mysqlplus.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////////////
//// sql_connection_c

sql_connection_c::sql_connection_c()
{
    host = user = password = database = 0;
    opendbbase = 0;
    errc = 0;
}

sql_connection_c::sql_connection_c( char *__database, char *__host, char *__user, char *__password )
{
    host = user = password = database = 0;
    opendbbase = 0;
    errc = 0;

    connect( __database, __host, __user, __password );
}

sql_connection_c::~sql_connection_c()
{
    if ( host )
    {
        delete[] host;
        host = 0;
    }

    if ( user )
    {
        delete[] user;
        user = 0;
    }

    if ( password )
    {
        delete[] password;
        password = 0;
    }
    if ( database )
    {
        delete[] database;
        database = 0;
    }

    OPENDB *odb;
    for ( odb = opendbbase; odb; odb = odb->next )
    {
        mysql_close( &odb->mysql );
    }

    while ( opendbbase )
    {
        odb = opendbbase;
        opendbbase = opendbbase->next;
        if ( odb -> busy )
        {
            //fprintf(stderr,"destroying Database object before Connect object(s)\n");
        }

        delete odb;
    }
}

///////////////////////////////////////////////////////////////////////////////////////

void sql_connection_c::connect( char *__database, char *__host, char *__user, char *__password )
{
    host = new char[ strlen( __host ) + 1 ];
    strcpy( host, __host );

    if ( __user )
    {
        user = new char[ strlen( __user ) + 1 ];
        strcpy( user, __user );
    }
    if ( __password )
    {
        password = new char[ strlen( __password ) + 1 ];
        strcpy( password, __password );
    }

    database = new char[ strlen( __database ) + 1 ];
    strcpy( database, __database );

    freedb( grabdb() ); //// open one connection
}

///////////////////////////////////////////////////////////////////////////////////////

OPENDB *sql_connection_c::grabdb()
{
    OPENDB *odb;
    for ( odb = opendbbase; odb; odb = odb->next )
    {
        if ( !odb->busy )
        {
            break;
        }
    }

    if ( !odb )
    {
        odb = new OPENDB;
        if ( !mysql_init( &odb->mysql ) )
        {
            //fprintf( stderr, "mysql_init() failed\n" );
            errc = 1;
        }
        if ( !mysql_real_connect( &odb->mysql, host, user, password, NULL, 0, NULL, 0 ) )
        {
            //fprintf( stderr, "mysql_connect(%s,%s,***) failed\n", host, user );
            errc = 2;
        }
        if ( mysql_select_db( &odb->mysql, database ) )
        {
            //fprintf( stderr, "mysql_select_db(%s,%s,%s,%s) failed\n", host, user, password, database );
            errc = 3;
        }

        odb->busy = 1;
        odb->next = opendbbase;
        opendbbase = odb;
    }
    else
    {
        odb->busy++;
    }

    return odb;
}

void sql_connection_c::freedb( OPENDB *odb )
{
    odb->busy = 0;
}

short sql_connection_c::errcode()
{
    return errc;
}

///////////////////////////////////////////////////////////////////////////////////////
//// sql_query_c

sql_query_c::sql_query_c()
{
    sql_connection = 0;
    odb = 0;
    res = 0;
    row = 0;

    results.set_size( 0 );
}

sql_query_c::sql_query_c( sql_connection_c *__sql_connection )
{
    sql_connection = __sql_connection;
    odb = sql_connection->grabdb();
    res = 0;
    row = 0;

    results.set_size( 0 );
}

sql_query_c::sql_query_c( sql_connection_c *__sql_connection, char *sql )
{
    sql_connection = __sql_connection;
    odb = sql_connection->grabdb();
    res = 0;
    row = 0;

    try_execute( sql );

    results.set_size( 0 );
}

sql_query_c::~sql_query_c()
{
    for ( int idx = 0; idx < results.n_items(); idx++ )
    {
        delete results[ idx ];
        results[ idx ] = 0;
    }

    if ( res )
    {
        mysql_free_result( res );
    }

    if ( odb )
    {
        sql_connection->freedb( odb );
    }
}

///////////////////////////////////////////////////////////////////////////////////////

int sql_query_c::try_execute( char *sql )
{
    if ( sql_connection && odb && !res )
    {
        if ( !mysql_query( &odb->mysql, sql ) )
        {
            return 1;
        }
    }

    return 0;
}

int sql_query_c::execute( char *sql )
{
    if ( sql_connection && odb && !res )
    {
        if ( try_execute( sql ) )
        {
            res = mysql_store_result( &odb->mysql );
        }
    }

    return (int)res;
}

sql_result_c *sql_query_c::store()
{
    if ( !res )
    {
        return 0;
    }

    sql_result_c *p = new sql_result_c( this, res );
    results.add( p );

    res = 0; //// prevent someone else from freeing this block

    return p;
}

void sql_query_c::free_result( sql_result_c *sql_result )
{
    for ( int idx = 0; idx < results.n_items(); idx++ )
    {
        if ( results[ idx ] == sql_result )
        {
            delete results[ idx ];
            results[ idx ] = 0;
            results.del( idx );
            break;
        }
    }
}

void sql_query_c::free_result()
{
    if ( sql_connection && odb && res )
    {
        mysql_free_result( res );
        res = 0;
        row = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////

__int64 sql_query_c::insert_id()
{
    if ( sql_connection && odb )
    {
        return mysql_insert_id( &odb->mysql );
    }

    return -1;
}

///////////////////////////////////////////////////////////////////////////////////////

MYSQL_FIELD *sql_query_c::fetch_field()
{
    if ( sql_connection && odb && res )
    {
        return mysql_fetch_field( res );
    }

    return 0;
}

char *sql_query_c::fetch_fieldname()
{
    MYSQL_FIELD *field = 0;

    if ( sql_connection && odb && res )
    {
        field = mysql_fetch_field( res );
    }

    return field ? field->name : "";
}

///////////////////////////////////////////////////////////////////////////////////////

int sql_query_c::ping()
{
    if ( odb )
    {
        return mysql_ping( &odb->mysql );
    }

    return -1;
}

///////////////////////////////////////////////////////////////////////////////////////
//// sql_var_c

sql_var_c::sql_var_c()
{
    value = 0;
}

sql_var_c::sql_var_c( int i )
{
    value = 0;
    *this = i;
}

sql_var_c::sql_var_c( double d )
{
    value = 0;
    *this = d;
}

sql_var_c::sql_var_c( char *s )
{
    value = 0;
    *this = s;
}

sql_var_c::sql_var_c( const char *s )
{
    value = 0;
    *this = (char *)s;
}

sql_var_c::~sql_var_c()
{
    release();
}

///////////////////////////////////////////////////////////////////////////////////////

sql_var_c::operator int ()
{
    if ( !value )
    {
        return 0;
    }

    return atoi( value );
}

sql_var_c::operator double ()
{
    if ( !value )
    {
        return 0;
    }

    return atof( value );
}

sql_var_c::operator char * ()
{
    return value;
}

sql_var_c::operator const char * ()
{
    return value;
}

///////////////////////////////////////////////////////////////////////////////////////

sql_var_c& sql_var_c::operator = ( int i )
{
    release();

    value = new char [ 11 + 1 ];
    itoa( i, value, 10 );
    return *this;
}

sql_var_c& sql_var_c::operator = ( double d )
{
    release();

    value = new char [ 32 ];
    sprintf( value, "%15.5f", d );
    return *this;
}

sql_var_c& sql_var_c::operator = ( char *s )
{
    release();

    value = new char [ strlen( s ) + 1 ];
    strcpy( value, s );
    return *this;
}

sql_var_c& sql_var_c::operator = ( const char *s )
{
    *this = (char *)s;
    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////

void sql_var_c::release()
{
    if ( value )
    {
        delete[] value;
        value = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////
//// sql_row_c

sql_row_c::sql_row_c()
{
    row = 0;
    __allow_null = 1; //// by default, return null values
}

sql_row_c::sql_row_c( MYSQL_ROW __row )
{
    row = __row;
    __allow_null = 1; //// by default, return null values
}

///////////////////////////////////////////////////////////////////////////////////////

void sql_row_c::allow_null( int allow )
{
    __allow_null = allow;
}

///////////////////////////////////////////////////////////////////////////////////////

sql_var_c sql_row_c::operator [] ( int idx )
{
    return sql_var_c( __allow_null ? ( row[ idx ] ) : ( row[ idx ] ? row[ idx ] : "" ) );
}

///////////////////////////////////////////////////////////////////////////////////////
//// sql_field_c

sql_field_c::sql_field_c()
{
    field = 0;
}

sql_field_c::sql_field_c( MYSQL_FIELD *__field )
{
    field = __field;
}

///////////////////////////////////////////////////////////////////////////////////////

char *sql_field_c::get_name()
{
    return field->name;
}

char *sql_field_c::get_default_value()
{
    return field->def;
}

enum_field_types sql_field_c::get_type()
{
    return field->type;
}

unsigned int sql_field_c::get_max_length()
{
    return field->max_length;
}

///////////////////////////////////////////////////////////////////////////////////////
//// sql_result_c

sql_result_c::sql_result_c()
{
    query = 0;
    res = 0;
}

sql_result_c::sql_result_c( sql_query_c *__query, MYSQL_RES *__res )
{
    query = __query;
    res = __res;
}

sql_result_c::~sql_result_c()
{
    if ( res )
    {
        mysql_free_result( res );
    }
}

///////////////////////////////////////////////////////////////////////////////////////

__int64 sql_result_c::n_rows()
{
    if ( query && res )
    {
        return mysql_num_rows( res );
    }

    return 0;
}

sql_row_c sql_result_c::fetch_row()
{
    MYSQL_ROW row = mysql_fetch_row( res );

    return sql_row_c( row );
}

///////////////////////////////////////////////////////////////////////////////////////

unsigned int sql_result_c::n_fields()
{
    if ( query && res )
    {
        return mysql_num_fields( res );
    }

    return 0;
}

sql_field_c sql_result_c::fetch_field( unsigned int field_idx )
{
    if ( query && res )
    {
        MYSQL_FIELD *field = mysql_fetch_field_direct( res, field_idx );
        return sql_field_c( field );
    }

    return sql_field_c();
}