# NodePP SQLite: Embedded SQL Database for NodePP

This project provides a lightweight and self-contained SQLite database binding for the NodePP asynchronous and event-driven C++ framework. SQLite is a popular, zero-configuration, transactional SQL database engine. By integrating SQLite with NodePP, you can easily embed a powerful relational database directly into your NodePP applications without the need for a separate server.

## Key Features

* **Embedded Database:** Provides a file-based SQL database that is directly integrated into your application.
* **Asynchronous Operations:** Offers asynchronous APIs for executing SQL queries, preventing blocking of the NodePP event loop.
* **Simple API:** Provides an easy-to-use interface for opening databases, executing queries, and handling results within NodePP.
* **Lightweight:** SQLite itself is a small and efficient database engine, adding minimal overhead to your NodePP application.
* **No External Dependencies:** SQLite is self-contained, requiring no external server installation.

## Dependencies
```bash
#libsqlite3-dev
🪟: pacman -S mingw-w64-ucrt-x86_64-sqlite3
🐧: sudo apt install libsqlite3-dev
```

## Example
```cpp
#include <nodepp/nodepp.h>
#include <sqlite/sqlite.h>

using namespace nodepp;

void onMain() {

    sqlite_t db ("myDB.db");

    db.exec(R"(
        CREATE TABLE COMPANY(
        ID INT PRIMARY KEY     NOT NULL,
        NAME           TEXT    NOT NULL,
        AGE            INT     NOT NULL,
        ADDRESS        CHAR(50),
        SALARY         REAL );
    )");

    db.exec(R"(
        INSERT INTO COMPANY ( ID, NAME, AGE, ADDRESS, SALARY )
        VALUES (1, 'Paul', 32, 'California', 20000.00 );
    )");

    db.exec(R"(
        INSERT INTO COMPANY ( ID, NAME, AGE, ADDRESS, SALARY )
        VALUES (2, 'John', 32, 'California', 20000.00 );
    )");

    db.exec(R"(
        INSERT INTO COMPANY ( ID, NAME, AGE, ADDRESS, SALARY )
        VALUES (3, 'Mery', 32, 'California', 20000.00 );
    )");

    db.exec(R"(
        INSERT INTO COMPANY ( ID, NAME, AGE, ADDRESS, SALARY )
        VALUES (4, 'Pipi', 32, 'California', 20000.00 );
    )");

    db.exec("SELECT * from COMPANY",[]( sql_item_t args ){
        for( auto &x: args.keys() ){
             console::log( x, "->", args[x] );
        }
    });

}
```

## Compilation
```bash
g++ -o main main.cpp -I ./include -lsqlite3 ; ./main
```
