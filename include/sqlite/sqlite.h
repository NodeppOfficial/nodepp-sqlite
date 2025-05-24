#ifndef NODEPP_SQLITE
#define NODEPP_SQLITE

/*────────────────────────────────────────────────────────────────────────────*/

#include <nodepp/nodepp.h>
#include <sqlite3.h>

namespace nodepp { using sql_item_t = map_t<string_t,string_t>; }

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_SQLITE_GENERATOR
#define NODEPP_SQLITE_GENERATOR

namespace nodepp { namespace _sqlite_ { GENERATOR( cb ){
protected:

    array_t<string_t> col;
    int num_fields, x;
    int err;

public:

    template< class T, class U, class V, class Q > coEmit( T& fd, U& res, V& cb, Q& self ){
    gnStart

        num_fields = sqlite3_column_count( res ); for( x=0; x<num_fields; x++ )
        { col.push(string_t((char*)sqlite3_column_name(res,x))); } coYield(1);

        coWait( (err=sqlite3_step(res)) == SQLITE_BUSY );
        if( err != SQLITE_ROW ){ coGoto(2); } do {

            auto object = map_t<string_t,string_t>();

            for( x=0; x<num_fields; x++ ){
                 char* y = (char*) sqlite3_column_text(res,x);
                 object[ col[x] ] = y ? y : "NULL";
            }

        cb( object ); } while(0); coSet(1); return 0;
        coYield(2); sqlite3_finalize( res );

    gnStop
    }

};}}

#endif

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class sqlite_t {
protected:

    struct NODE {
        sqlite3 *fd = nullptr;
        int   state = 1;
    };  ptr_t<NODE> obj;

public:

    virtual ~sqlite_t() noexcept {
        if( obj.count() > 1 || obj->fd == nullptr ){ return; }
        if( obj->state == 0 ){ return; } free();
    }

    /*─······································································─*/

    virtual void free() const noexcept {
        if( obj->fd == nullptr ){ return; }
        if( obj->state == 0 )   { return; }
        sqlite3_close( obj->fd );
        obj->state = 0;
    }

    /*─······································································─*/

    sqlite_t ( string_t db_file ) : obj( new NODE ) {
        if( sqlite3_open( db_file.data(), &obj->fd ) ) {
            process::error( "SQL Error: ", sqlite3_errmsg(obj->fd) );
        }
    }

    sqlite_t () : obj( new NODE ) { obj->state = 0; }

    /*─······································································─*/

    void exec( const string_t& cmd, const function_t<void,sql_item_t>& cb ) const {
        if( cmd.empty() || obj->state==0 || obj->fd==nullptr ){ return; }

        sqlite3_stmt *res; int rc; char* msg; auto self = type::bind( this );

        if( sqlite3_prepare_v2( obj->fd, cmd.get(), -1, &res, NULL ) != SQLITE_OK ) {
            string_t message ( sqlite3_errmsg( obj->fd ) );
            process::error( "SQL Error: ", message );
        }   if( res == NULL ) { return; }

        _sqlite_::cb task; process::add( task, obj->fd, res, cb, self );
    }

    array_t<sql_item_t> exec( const string_t& cmd ) const { array_t<sql_item_t> arr;
        function_t<void,sql_item_t> cb = [&]( sql_item_t args ){ arr.push(args); };
        if( cmd.empty() || obj->state==0 || obj->fd==nullptr ){ return nullptr; }

        sqlite3_stmt *res; int rc; char* msg; auto self = type::bind( this );

        if( sqlite3_prepare_v2( obj->fd, cmd.get(), -1, &res, NULL ) != SQLITE_OK ) {
            string_t message ( sqlite3_errmsg( obj->fd ) );
            process::error( "SQL Error: ", message );
        }   if( res == NULL ) { return nullptr; }

        _sqlite_::cb task; process::await( task, obj->fd, res, cb, self );

        return arr;
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/

#endif
