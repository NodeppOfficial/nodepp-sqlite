#ifndef NODEPP_SQLITE
#define NODEPP_SQLITE

/*────────────────────────────────────────────────────────────────────────────*/

#include <nodepp/nodepp.h>
#include <sqlite3.h>

namespace nodepp { using sql_item_t = map_t<string_t,string_t>; }

namespace nodepp { class sqlite_t {
protected:

    struct NODE {
        sqlite3 *fd = nullptr;
        int   state = 1;
    };  ptr_t<NODE> obj;

    static int callback( void* data, int argc, char **argv, char **azColName ) {
        sql_item_t arguments; if( data==nullptr || argc<=0 ) { return 0; }

        process::next(); for ( auto x=0; x<argc; x++ )
        { arguments[ azColName[x] ] = argv[x] ? argv[x] : "NULL"; }
        (*type::cast<function_t<void,sql_item_t>>(data))( arguments );

        return 0;
    }

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
        if( obj->state == 0 || obj->fd == nullptr ){ return; } char* msg;
        if( sqlite3_exec( obj->fd, cmd.data(), callback, (void*)&cb, &msg) != SQLITE_OK ){
            string_t message ( msg ); sqlite3_free( msg );
            process::error( "SQL Error: ", message );
        }
    }

    array_t<sql_item_t> exec( const string_t& cmd ) const { array_t<sql_item_t> res;
        if( obj->state == 0 || obj->fd == nullptr ){ return nullptr; } char* msg;
        function_t<void,sql_item_t> cb = [&]( sql_item_t args ){ res.push( args ); };
        if( sqlite3_exec( obj->fd, cmd.data(), callback, (void*)&cb, &msg) != SQLITE_OK ){
            string_t message ( msg ); sqlite3_free( msg );
            process::error( "SQL Error: ", message );
        }   return res;
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/

#endif
