# Practicum 2 – Self‑Evaluation  
**Course:** CS5600 Computer Systems  
**Team:** <Name 1>, <Name 2>  
**Emails:** <netID1@>, <netID2@>  
**GitHub README link to rubric copy:** <insert‑google‑sheet‑link‑here>

| Section & Item | Max | Our Score | Where in Code (function / lines) | Notes |
|----------------|-----|-----------|----------------------------------|-------|
| **Part 1 – File write** | | | | |
| Command design (`WRITE`) | 10 | 10 | `client.c do_write`, `server.c handle_write` | Explicit `[RO|RW]` flag |
| Successful upload (TCP)  | 10 | 10 | same as above | Tested with 2 MB file |
| Missing path handled     | 10 | 10 | `server.c handle_write` lines 80‑90 | returns `ERR_BAD_ARGS` |
| **Part 2 – File read** | | | | |
| Command design (`GET`)   | 5  | 5  | `client.c do_get`, `server.c handle_get` | |
| Successful download      | 10 | 10 | see above | |
| Missing path handled     | 5  | 5  | `ERR_FILE_NOT_FOUND` | |
| **Part 3 – File delete** | | | | |
| Command design (`RM`)    | 5  | 5  | `client.c do_rm`, `server.c handle_rm` | |
| File deleted on server   | 10 | 10 | `remove()` + log | |
| Missing path handled     | 5  | 5  | returns `ERR_REMOVE_FAILED` | |
| **Part 4 – Advanced** | | | | |
| 4a Multithreading: thread/conn | 10 | 10 | `pthread_create` in `main` | |
| 4a Thread‑safe (flock)        | 10 | 10 | all handlers use `flock` | |
| 4b Permissions first write    | 10 | 10 | `set_file_permission` | |
| 4b RO/RW enforced             | 10 | 10 | checks in `handle_write` and `handle_rm` | |
| **Overall** | | | | |
| Code & comments quality  | 5  | 5  | — | 50 comment lines |
| Demo / presentation      | 5  | 5  | — | scheduled slot  —  |
| **Total** | **100** | **100** | | |

*We believe our implementation meets all rubric items at the Exemplary level.
Feel free to use the line references above while grading.*