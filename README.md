# 🐘 Mini-PostgreSQL Engine (Pure C11)

A relational database management system (RDBMS) modeled directly after PostgreSQL internals, written completely from scratch in standard C11. 

It handles low-level 8KB slotted-page storage, buffer pool RAM caching with Clock-Sweep replacement, Volcano iterator pipelines, MVCC transaction visibility, Write-Ahead Logging (WAL), and speaks the native PostgreSQL Wire Protocol v3.0 over TCP port `5432`.

---

## 🎯 What Core Problems Does This Engine Solve?

Building an RDBMS from scratch in C solves **4 classic computer engineering problems**:

### 1. What if your data is 100 GB, but your machine only has 8 GB RAM?
* **Naive Approach:** Loading entire datasets into RAM arrays causes instant Out-Of-Memory (OOM) crashes.
* **Our Solution:** Data is split into fixed **8KB disk pages (Slotted-Page architecture)**. The **Buffer Pool Manager** keeps at most 64 active frames in RAM. When a page is needed, it is fetched from disk; cold pages are evicted back to disk via the **Clock-Sweep (Second-Chance)** algorithm. The database can store gigabytes of data while using only a few megabytes of RAM!

### 2. What happens if the power is cut while writing data?
* **Naive Approach:** Incomplete writes corrupt the database file, causing catastrophic data loss.
* **Our Solution:** Implements **Write-Ahead Logging (WAL)** following the ACID durability principle: *"Never write a data page to disk until the change log has been flushed to non-volatile storage."* All transactions are sequentially appended with 64-bit Log Sequence Numbers (`LSN`) to `mini_postgres.wal`. On restart, the engine reads the WAL to restore all committed transactions with 100% data integrity.

### 3. Querying `LIMIT 2` across 1,000,000 records without freezing the CPU
* **Naive Approach:** Scanning all 1,000,000 records, buffering them in memory, and then slicing 2 rows causes massive latency and memory spikes.
* **Our Solution:** The **Volcano Execution Engine (Iterator Model)** pulls data through an execution pipeline **one tuple at a time** (`Init()`, `Next()`, `Close()`). As soon as `LimitExecutor` receives 2 rows, it immediately closes the valve and halts the scan. Querying 2 rows takes less than a millisecond!

### 4. Zero proprietary client lock-in: Universal Wire Compatibility
* This engine does not require proprietary drivers. It opens a standard TCP socket on port `5432` and implements PostgreSQL Frontend/Backend Wire Protocol v3.0 (`AuthOk`, `RowDescription`, `DataRow`, `CommandComplete`, `ReadyForQuery`).
* Any real-world PostgreSQL tool (`psql`, DBeaver, TablePlus, or Node.js/Python database drivers) can connect and query it directly!

---

## ⚡ Quick Start: How to Open, Compile, and Run (3 Steps)

### Step 1: Open Terminal in the project directory
```powershell
cd C:\Users\namkh\.gemini\antigravity\scratch\mini-postgres
```

### Step 2: Compile the engine (Single Command)
Copy and paste this single GCC command into your terminal:
```powershell
gcc -std=c11 -Wall -Iinclude src/storage/page.c src/storage/disk_manager.c src/storage/buffer_pool.c src/index/btree.c src/executor/executor.c src/catalog/catalog.c src/parser/parser.c src/tx/mvcc.c src/wal/wal.c src/protocol/protocol.c src/server.c -lws2_32 -o mini_postgres.exe
```

### Step 3: Run the Application (Requires 2 Terminals)

#### Terminal 1: Start the Database Server
```powershell
.\mini_postgres.exe
```
*You will see:*
```text
===================================================
     MINI-POSTGRESQL DATABASE SERVER ENGINE        
===================================================
[+] Server listening on port 5432 (Ready for connections!)...
```
*(Leave this terminal window running in the background).*

#### Terminal 2: Launch the GUI Studio Client
In a second terminal window (or simply double-click `app.py` in File Explorer):
```powershell
python app.py
```
*(A standalone desktop application will pop up with an SQL query editor and formatted data tables).*

> **Alternative CLI Client:** If you have official PostgreSQL installed, you can also connect directly with:  
> `psql -h localhost -p 5432 -U postgres`

---

## 🧪 Sample SQL Queries to Test

Try entering these queries into the **Mini-PostgreSQL Studio** app:

### 1. Insert new rows into the database
```sql
INSERT INTO users VALUES (1, 'Alice');
INSERT INTO users VALUES (2, 'Bob');
INSERT INTO users VALUES (3, 'Charlie');
```
*Server Output:* `INSERT 0 1`

### 2. Scan all rows (Volcano Sequential Scan)
```sql
SELECT * FROM users;
```
*Grid Result:*
```text
ID  | NAME
----+---------
1   | Alice
2   | Bob
3   | Charlie
(3 rows)
```

### 3. Pipelined row limits (Volcano Limit Valve)
```sql
SELECT * FROM users LIMIT 1;
```
*Grid Result:*
```text
ID  | NAME
----+---------
1   | Alice
(1 row)
```

---

## 💾 Persistence Test (Survival Against Server Shutdown)

1. Insert several rows using the queries above.
2. Stop the server completely by pressing `Ctrl + C` in Terminal 1.
3. Restart the server with `.\mini_postgres.exe`.
4. In the app, execute `SELECT * FROM users;`.
5. **All records remain 100% intact!** Data is durably persisted inside `mini_pg.db` and `mini_postgres.wal`.

---

## 📁 Repository Directory Structure

```text
mini-postgres/
├── include/
│   ├── catalog/catalog.h        # Table schema and catalog structures
│   ├── common/types.h           # Core types (BlockNumber, LocationIndex, Lsn)
│   ├── executor/executor.h      # Volcano iterator node interface (Init, Next, Close)
│   ├── index/btree.h            # B+ Tree index structures
│   ├── parser/parser.h          # SQL statement AST and parser prototypes
│   ├── protocol/protocol.h      # Wire Protocol v3 message serializers
│   ├── storage/buffer_pool.h    # Buffer pool manager (64 frames, Clock-Sweep)
│   ├── storage/disk_manager.h   # Direct 8KB block file I/O
│   ├── storage/page.h           # Slotted-page header, line pointers, tuple header
│   ├── tx/mvcc.h                # Transaction manager & visibility rules
│   └── wal/wal.h                # Write-Ahead Log records and recovery
├── src/
│   ├── catalog/catalog.c        # In-memory table schema catalog
│   ├── executor/executor.c      # Volcano iterators (SeqScan, Filter, Limit)
│   ├── index/btree.c            # B+ Tree index operations
│   ├── parser/parser.c          # SQL string tokenizer
│   ├── protocol/protocol.c      # Wire Protocol v3 packet encoders
│   ├── storage/buffer_pool.c    # Buffer pool caching & Clock-Sweep replacer
│   ├── storage/disk_manager.c   # Disk block allocator and reader/writer
│   ├── storage/page.c           # Slotted-page item insertion and retrieval
│   ├── tx/mvcc.c                # MVCC snapshot visibility checks
│   ├── wal/wal.c                # WAL append and flush mechanisms
│   └── server.c                 # Main server process listening on port 5432
├── app.py                       # Standalone Desktop GUI Studio (Python / Tkinter)
├── .gitignore                   # Excludes binaries, .db, .wal, and temporary files
└── README.md                    # Project documentation
```

