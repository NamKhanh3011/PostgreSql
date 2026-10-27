# mini-postgres

A relational database engine modeled directly after PostgreSQL architecture, written in standard C11.

## Subsystems Layout
- **include/common/types.h**: Fundamental database primitives (`BlockNumber`, `TransactionId`, `Lsn`, `ItemPointerData`).
- **include/storage/page.h**: Slotted-Page layout (fixed 8KB pages, line pointer array, `xmin`/`xmax` tuple header).
- **include/storage/buffer_pool.h**: Buffer Pool Manager (frames, Clock Sweep replacement algorithm, pin/unpin).
- **include/index/btree.h**: Balanced B+ Tree index with sibling leaf links for range scans.
- **include/tx/mvcc.h**: Multi-Version Concurrency Control (MVCC) and snapshot isolation rules.
- **include/wal/wal.h**: Write-Ahead Logging (LSN) and ARIES crash recovery algorithm (Analysis, Redo, Undo).
- **include/executor/executor.h**: Volcano Iterator Model (`Init`, `Next`, `Close`) for query execution.
- **include/protocol/protocol.h**: PostgreSQL Frontend/Backend Wire Protocol v3.0 over TCP (compatible with `psql`).

## Build Instructions

### Using CMake
```bash
mkdir build && cd build
cmake ..
cmake --build .
./mini_postgres
```

### Using GCC / Clang
```bash
gcc -std=c11 -Wall -Wextra -Iinclude src/server.c src/storage/page.c -o mini_postgres
./mini_postgres
```
