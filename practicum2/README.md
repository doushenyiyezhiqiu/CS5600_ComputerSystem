# Remote File System – Practicum 2

A compact **multi‑threaded** client/server file‑storage service written in C.

| Component | Highlights |
|-----------|------------|
| **Server (`rfserver`)** | • Listens on **TCP port 2024**<br>• Spawns **one thread per client**<br>• Stores files under `server_data/…`<br>• Per‑file **`flock()`** prevents race conditions when several clients touch the same file<br>• **Permissions** – first `WRITE` sets file to **RW** (default) or **RO**; later ops respect that setting |
| **Client (`rfs`)** | One‑shot CLI<br>• `WRITE &lt;local&gt; &lt;remote&gt; [RO|RW]` – upload<br>• `GET &lt;remote&gt; &lt;local&gt;` – download<br>• `RM &lt;remote&gt;` – delete |

---

## Build

```bash
git clone <your‑repo>
cd rfs                 # directory containing makefile, *.c, *.h
make                   # builds ./rfserver and ./rfs
## Run

### 1  Start the server

```bash
Copy
Edit
./rfserver
# [Server] Listening on port 2024 …
The server automatically creates the top‑level server_data/ folder the first time it runs. If you plan to write into nested paths such as folder/remote.txt, be sure that sub‑directory exists:

```bash
Copy
Edit
mkdir -p server_data/folder
### 2  Operate from one or more client terminals

```bash
Copy
Edit
# create a local file on the *client* side
mkdir -p client1
echo "Hello from client1" > client1/local1.txt

# upload (defaults to read‑write)
./rfs WRITE client1/local1.txt folder/remote.txt

# download
./rfs GET folder/remote.txt download.txt
cat download.txt    # -> Hello from client1

# upload a read‑only file
./rfs WRITE client1/local1.txt folder/rofile.txt RO

# any later write to the same path now fails
./rfs WRITE client1/local1.txt folder/rofile.txt
# -> Server error: ERR_FILE_IS_READ_ONLY

# delete the read‑write file
./rfs RM folder/remote.txt    # returns RM_OK
## Testing Concurrency & Locks

Open two client shells:

```bash
Copy
Edit
# Terminal A – long write (holds exclusive lock)
./rfs WRITE client1/big.txt folder/the_same.txt &

# Terminal B – second client tries to overwrite
./rfs WRITE client1/local1.txt folder/the_same.txt
Terminal B blocks until Terminal A’s LOCK_EX is released, proving that per‑file locking serializes concurrent writes.
While a client is reading (GET), the server applies a shared LOCK_SH, so multiple readers can proceed concurrently but writers wait.

## Source‑Level Tour


File	Purpose / Highlights
common.h	Port constant, buffer sizes, permission_t enum
server.c	Thread creation, flock() logic, permission table
client.c	CLI that builds one request and exchanges data
server.h, client.h	Internal prototypes
makefile	Targets rfserver, rfs, make clean
Key server functions:


Function	Role
handle_write()	Exclusive lock, first‑write permissions
handle_get()	Shared lock for consistent reads
handle_rm()	Exclusive lock before unlink
set_file_permission()	Adds path → RO/RW entry
get_file_permission()	Looks up RO/RW status
client_thread()	Worker for each connected client
## Ideas for Extension

LIST (LS <remoteDir>) to enumerate server directories

Auto‑create nested directories on the server

Replace single‑chunk transfers with a loop for arbitrarily large files

Persist permissions in SQLite or another lightweight store

Implement encryption (Option 4c) – store ciphertext, decrypt on GET

## Clean Up

```bash
Copy
Edit
make clean          # remove rfserver, rfs, *.o
rm -rf server_data  # wipe remote files
Copy
Edit
