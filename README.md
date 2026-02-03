# Nodepp SQLite Wrapper
A high-performance, asynchronous SQLite database client built on the Nodepp framework. This library utilizes Nodepp's coroutines, events, and promises to provide non-blocking database access, ensuring your application's I/O operations don't freeze the event loop.

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
        db.await("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT);");

        console::log("Database and table initialized successfully.");

    } catch( except_t error ) {
        console::error("Database initialization failed:", error.what());
    }
}
```

## Core API Usage
The library provides three distinct methods for executing SQL, catering to different concurrency needs: Promise-based, Synchronous, and Streaming/Callback.

#### 1. Asynchronous with Promise (.resolve())
This is the recommended modern approach for asynchronous queries. It executes the query in the background and returns a promise that resolves with the complete array of results.

```cpp
db.resolve("SELECT name FROM users WHERE id < 10;")

.then([]( array_t<sql_item_t> results ) {
    console::log("Fetched names:");
    for( auto& row : results ) {
        console::log("- ", row["name"]);
    }
})

.fail([]( except_t error ) {
    console::error("Async query failed:", error.what());
});
```

#### 2. Synchronous/Blocking (.await())
This method executes the command and blocks the current process until the entire result set is collected.

```cpp
try {
    
    auto results = db.await("SELECT COUNT(*) as total FROM users;");
    auto total_users = results[0]["total"];
    console::log("Total users found:", total_users);

} catch( except_t error ) {
    console::error("Synchronous read failed:", error.what());
}
```

### 3. Asynchronous Streaming/Fire-and-Forget (.emit())

Use emit() for non-query commands (INSERT, UPDATE, DELETE) or for processing very large result sets one row at a time via a callback.

**Example (Inserting Data - Fire-and-Forget):**
```cpp
// Non-query command, no callback needed
db.emit("INSERT INTO users (name) VALUES ('Alice');");
db.emit("INSERT INTO users (name) VALUES ('Bob');");
console::log("Inserts dispatched asynchronously.");
```

**Example (Streaming Results):**
```cpp

db.emit("SELECT * FROM large_table;", []( sql_item_t row ) {
    // Process the row immediately without waiting for the full result set.
    console::log("Streaming row with ID:", row["id"]);
});
```

## Compilation
```bash
g++ -o main main.cpp -I ./include -lsqlite3 ; ./main
```

## License
**Nodepp-SQLite** is distributed under the MIT License. See the LICENSE file for more details.
