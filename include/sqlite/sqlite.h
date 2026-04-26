/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOfficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_SQLITE
#define NODEPP_SQLITE

/*────────────────────────────────────────────────────────────────────────────*/

#include <nodepp/nodepp.h>
#include <nodepp/expected.h>
#include <nodepp/optional.h>
#include <nodepp/promise.h>
#include <nodepp/event.h>

#include <sqlite3.h>

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_SQLITE_GENERATOR
#define NODEPP_SQLITE_GENERATOR

namespace nodepp { namespace _sqlite_ { GENERATOR( pipe ){
protected:

    array_t<string_t>  col;
    int num_fields, x, err;

public:

    template< class U, class V > coEmit( U& ctx, V& cb ){
    coBegin ;

        if( cb.null() ){ sqlite3_finalize(ctx); coEnd; }

        num_fields = sqlite3_column_count( ctx ); for( x=0; x<num_fields; x++ )
        { col.push(string_t((char*)sqlite3_column_name(ctx,x))); }

        coYield(1); coWait((err=sqlite3_step(ctx)) == SQLITE_BUSY );
        if( err!=SQLITE_ROW ) { coGoto(2); } do {

            auto object = map_t<string_t,string_t>();

            for( x=0; x<num_fields; x++ ){
                 auto y=(char*) sqlite3_column_text(ctx,x);
                 object[col[x]] = y ? y : "NULL";
            }

        cb(object); } while(0); coGoto(1); coYield(2);
        sqlite3_finalize(ctx);

    coFinish }

};}}

#endif

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_SQL_ITEM
#define NODEPP_SQL_ITEM
namespace nodepp { using sql_item_t = map_t<string_t,string_t>; }
#endif

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class sqlite_t {
protected:

    enum STATE {
        SQL_STATE_UNKNOWN = 0b00000000,
        SQL_STATE_OPEN    = 0b00000001,
        SQL_STATE_USED    = 0b10000000,
        SQL_STATE_CLOSE   = 0b00000010
    };

    struct NODE {
        sqlite3 *fd=nullptr; /**/ int state=0;
       ~NODE() { if( fd ){ sqlite3_close(fd); } }
    };  ptr_t<NODE> obj;

    void use()     const noexcept { obj->state|= STATE::SQL_STATE_USED ; }
    void release() const noexcept { obj->state&=~STATE::SQL_STATE_USED ; }

public:

    sqlite_t ( string_t path ) : obj( new NODE ) {
        
        if( sqlite3_open( path.data(), &obj->fd ) ) {
            string_t msg = sqlite3_errmsg( obj->fd );
            throw except_t( "SQL Error: ", msg );
        }

    //  sqlite3_busy_timeout( obj->fd, 1000 );
        obj->state = STATE::SQL_STATE_OPEN;

    }

   ~sqlite_t () noexcept { if( obj.count() > 1 ){ return; } free(); }

    sqlite_t () : obj( new NODE ) { obj->state = STATE::SQL_STATE_CLOSE; }

    /*─······································································─*/

    expected_t< sqlite_t, except_t >
    emit( const string_t& cmd, function_t<void,sql_item_t> cb=nullptr ) const noexcept {
    except_t err; do {

        if( is_used() )
          { return except_t( "SQL Error: client is already used" ); }
        
        if( cmd.empty() || is_closed() || obj->fd==nullptr )
          { err = except_t( "SQL Error: closed" ); break; } 
          
        sqlite3_stmt *ctx; use();

        if( sqlite3_prepare_v2( obj->fd, cmd.get(), -1, &ctx, NULL ) != SQLITE_OK )
          { err = except_t( "SQL Error: ", sqlite3_errmsg( obj->fd ) ); break; }
        
        if( ctx == NULL )
          { err = except_t( "SQL Error: something went wrong" ); break; }

        _sqlite_::pipe pipe; 
        
        while( pipe( ctx, cb )==1 ){ /*unused*/ }

       /*----*/ release(); return *this; 
    } while(0); release(); return  err ; }

    expected_t<ptr_t<sql_item_t>,except_t> resolve( const string_t& cmd ) const noexcept { 
    except_t err; do {

        if( is_used() )
          { return except_t( "SQL Error: client is already used" ); }
        
        if( cmd.empty() || is_closed() || obj->fd==nullptr )
          { err = except_t( "SQL Error: closed" ); break; } 
          
        sqlite3_stmt *ctx; use();

        if( sqlite3_prepare_v2( obj->fd, cmd.get(), -1, &ctx, NULL ) != SQLITE_OK )
          { err = except_t( "SQL Error: ", sqlite3_errmsg( obj->fd ) ); break; }
        
        if( ctx == NULL )
          { err = except_t( "SQL Error: something went wrong" ); break; }

        queue_t<sql_item_t> list; _sqlite_::pipe pipe;

        function_t<void,sql_item_t> cb ([&]( sql_item_t item ){
            list.push( item );
        }); 
        
        while( pipe( ctx, cb )==1 ){ /*unused*/ }

       /*----*/ release(); return list.data(); 
    } while(0); release(); return err /*--*/ ; }

    /*─······································································─*/

    bool is_closed()    const noexcept { return obj->state & STATE::SQL_STATE_CLOSE; }
    bool is_used()      const noexcept { return obj->state & STATE::SQL_STATE_USED ; }
    void close()        const noexcept { /*--*/ obj->state = STATE::SQL_STATE_CLOSE; }
    bool is_available() const noexcept { return !is_closed(); }

    /*─······································································─*/

    void free() const noexcept {
        if( obj->fd == nullptr ){ return; }
        if( is_closed() ) /*-*/ { return; } close();
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/