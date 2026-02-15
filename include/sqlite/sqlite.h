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

namespace nodepp { namespace _sqlite_ { GENERATOR( cb ){
protected:

    array_t<string_t>  col;
    int num_fields, x, err;

public:

    template< class U, class V, class Q > coEmit( U& ctx, V& cb, Q& self ){
    coBegin ; coWait( self->is_used()==1 ); self->use();
        if( cb.null() ){ sqlite3_finalize(ctx); self->release(); coEnd; }

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
        sqlite3_finalize(ctx); self->release();

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
        sqlite3 *fd = nullptr;
        int   state = 0;
    };  ptr_t<NODE> obj;

public:

    sqlite_t ( string_t path ) : obj( new NODE ) {
        if( sqlite3_open( path.data(), &obj->fd ) ) {
        throw except_t( "SQL Error: ", sqlite3_errmsg(obj->fd) );
        return; } obj->state = STATE::SQL_STATE_OPEN;
    }

   ~sqlite_t () noexcept { if( obj.count() > 1 ){ return; } free(); }

    sqlite_t () : obj( new NODE ) { obj->state = STATE::SQL_STATE_CLOSE; }

    /*─······································································─*/

    promise_t<array_t<sql_item_t>,except_t> resolve( const string_t& cmd ) const { 
           queue_t<sql_item_t> arr; auto self = type::bind( this );
    return promise_t<array_t<sql_item_t>,except_t>([=]( 
        res_t<array_t<sql_item_t>> res, 
        rej_t<except_t> /*------*/ rej 
    ){

        function_t<void,sql_item_t> cb ([=]( sql_item_t args ){ arr.push(args); });
        
        if( cmd.empty() || self->is_closed() || self->obj->fd==nullptr )
          { rej(except_t( "SQL Error: closed" )); return; } sqlite3_stmt *ctx;

        if( sqlite3_prepare_v2( self->obj->fd, cmd.get(), -1, &ctx, NULL ) != SQLITE_OK ) 
          { rej(except_t( "SQL Error: ", sqlite3_errmsg( self->obj->fd ) )); return; } 
        
        if( ctx == NULL ) 
          { rej(except_t( "SQL Error: something went wrong" )); return; }

        auto task = type::bind( _sqlite_::cb() ); process::add([=](){
            while( (*task)( ctx, cb, self )>=0 ){ return 1; }
            res( arr.data() ); return -1; 
        }); 
    
    }); }

    /*─······································································─*/

    expected_t<array_t<sql_item_t>,except_t>
    await( const string_t& cmd ) const { return resolve( cmd ).await(); }

    /*─······································································─*/

    optional_t<except_t>
    emit( const string_t& cmd, function_t<void,sql_item_t> cb=nullptr ) const {
        
        if( cmd.empty() || is_closed() || obj->fd==nullptr )
          { return except_t( "SQL Error: closed" ); } sqlite3_stmt *ctx;

        if( sqlite3_prepare_v2( obj->fd, cmd.get(), -1, &ctx, NULL ) != SQLITE_OK )
          { return except_t( "SQL Error: ", sqlite3_errmsg( obj->fd ) ); }
        
        if( ctx==NULL ) { return except_t( "SQL Error: something went wrong" ); }

        _sqlite_::cb task; auto self = type::bind( this );
        process::add( task, ctx, cb, self );

    return nullptr; }

    /*─······································································─*/

    bool is_closed()    const noexcept { return obj->state & STATE::SQL_STATE_CLOSE; }
    bool is_used()      const noexcept { return obj->state & STATE::SQL_STATE_USED ; }
    void close()        const noexcept { /*--*/ obj->state = STATE::SQL_STATE_CLOSE; }
    void use()          const noexcept { /*--*/ obj->state|= STATE::SQL_STATE_USED ; }
    void release()      const noexcept { /*--*/ obj->state&=~STATE::SQL_STATE_USED ; }
    bool is_available() const noexcept { return !is_closed(); }

    /*─······································································─*/

    void free() const noexcept {
        if( obj->fd == nullptr ){ return; }
        if( is_closed() ) /*-*/ { return; }
        close(); sqlite3_close(obj->fd);
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/

#endif
