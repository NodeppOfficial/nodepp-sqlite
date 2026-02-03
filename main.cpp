#include <nodepp/nodepp.h>
#include <sqlite/sqlite.h>

using namespace nodepp;

void onMain() {

    sqlite_t db ("myDB.db");

    db.resolve(R"(
        CREATE TABLE COMPANY(
        ID INT PRIMARY KEY     NOT NULL,
        NAME           TEXT    NOT NULL,
        AGE            INT     NOT NULL,
        ADDRESS        CHAR(50),
        SALARY         REAL );
    )").await();

    db.resolve(R"(
        INSERT INTO COMPANY ( ID, NAME, AGE, ADDRESS, SALARY )
        VALUES (1, 'Paul', 32, 'California', 20000.00 );
    )").await();

    db.resolve(R"(
        INSERT INTO COMPANY ( ID, NAME, AGE, ADDRESS, SALARY )
        VALUES (2, 'John', 32, 'California', 20000.00 );
    )").await();

    db.resolve(R"(
        INSERT INTO COMPANY ( ID, NAME, AGE, ADDRESS, SALARY )
        VALUES (3, 'Mery', 32, 'California', 20000.00 );
    )").await();

    db.resolve(R"(
        INSERT INTO COMPANY ( ID, NAME, AGE, ADDRESS, SALARY )
        VALUES (4, 'Pipi', 32, 'California', 20000.00 );
    )").await();

    db.resolve(R"(
        INSERT INTO COMPANY ( ID, NAME, AGE, ADDRESS, SALARY )
        VALUES (4, 'Pipi', 32, 'California', 20000.00 );
    )")

    .then([=]( array_t<sql_item_t> data ){
        // data
    })

    .fail([=]( except_t err ){
        //err
    });

    db.emit("SELECT * from COMPANY",[]( sql_item_t args ){
        for( auto &x: args.keys() ){
             console::log( x, "->", args[x] );
        }
    });

}
