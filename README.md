# Nodepp SQLite Wrapper
...

## Dependencies & Cmake Integration
```bash
#libsqlite3-dev
🪟: pacman -S mingw-w64-ucrt-x86_64-sqlite3
🐧: sudo apt install libsqlite3-dev
```
```bash
include(FetchContent)

FetchContent_Declare(
	nodepp
	GIT_REPOSITORY   https://github.com/NodeppOfficial/nodepp
	GIT_TAG          origin/main
	GIT_PROGRESS     ON
)
FetchContent_MakeAvailable(nodepp)

FetchContent_Declare(
	nodepp-sqlite
	GIT_REPOSITORY   https://github.com/NodeppOfficial/nodepp-sqlite
	GIT_TAG          origin/main
	GIT_PROGRESS     ON
)
FetchContent_MakeAvailable(nodepp-sqlite)

#[...]

target_link_libraries( #[...]
	PUBLIC nodepp nodepp-sqlite #[...]
)
```

## Getting Started
```cpp
#include <nodepp/nodepp.h>
#include <sqlite/sqlite.h>

using namespace nodepp;

void main() {
    // 1. Establish connection (synchronously wait for connection)
    try {
        auto db = sqlite::add("my_database.db");

        // 2. Execute a command synchronously
        if(
            !db.emit("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT);");
        ){ throw except_t( "something went wrong" ); }

        console::log("Database and table initialized successfully.");

    } catch( except_t error ) {
        console::error("Database initialization failed:", error.what());
    }
}
```

## Compilation
```bash
g++ -o main main.cpp -I ./include -lsqlite3 ; ./main
```

## License
**Nodepp-SQLite** is distributed under the MIT License. See the LICENSE file for more details.
