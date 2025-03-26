# Memory Hierarchy Simulation - Practicum I

This project simulates a hierarchical memory system for a message-oriented data store, following the principles of virtual memory, caching, and page replacement. It is designed as part of Practicum I for CS5600 Computer Systems.

## 📦 Project Overview

The simulation explores how real-world systems manage memory using a layered approach:

- **Disk (Secondary Storage)** — All messages are persistently stored on disk.
- **Cache (Main Memory)** — A fixed-size memory cache stores the most recently used messages.
- **Page Replacement Policy** — If the cache is full, a message is evicted using the **Least Recently Used (LRU)** strategy.

---

## 📁 Features & Functionality

### ✅ Part 1: Memory Hierarchy & Storage

- Defines a `Message` structure with:
  - Unique identifier (`id`)
  - Timestamp (`time_sent`)
  - Sender/receiver names
  - Text content
  - Delivery flag (`delivered`)
- All messages are written to and retrieved from disk in binary format.
- Disk files are named: `messages/<id>.msg`

### ✅ Part 2: Caching in Main Memory

- Implements a **fixed-size cache** (`CACHE_CAPACITY = 16`)
- Each message has a **fixed size of 1024 bytes**, aligning with page/block size
- Write-through behavior: All messages are written to both cache and disk
- Lookup first checks cache, and only falls back to disk if needed

### ✅ Part 3: LRU Page Replacement

- Uses a true **LRU (Least Recently Used)** algorithm
- Design is inspired by [LeetCode 146 – LRU Cache](https://leetcode.com/problems/lru-cache/)
- LRU is implemented in C using:
  - A **doubly linked list** for usage ordering
  - A **hash table** for O(1) access

### ✅ Part 4: Evaluation Metrics

- Evaluates cache effectiveness over **1000 random message accesses**
- Reports:
  - Cache hits
  - Cache misses
  - Hit ratio (%)
- Example result:
Cache Hits: 791 Cache Misses: 209 Hit Ratio: 79.10%

---

## 🧪 Running the Project

### 🔨 Build

make

### 📂 Setup

mkdir -p messages

### 🚀 Run

./message_store


### 📚 Source Code Structure
.

├── main.c          # Test harness and evaluation

├── message.h/c     # Message structure + disk I/O

├── cache.h/c       # Cache logic, LRU policy

├── Makefile

└── messages/       # Folder where .msg files are stored

### ⚙️ Configuration

CACHE_CAPACITY → max number of messages in memory (default 16)

MAX_CONTENT_LEN → adjusted so sizeof(Message) = 1024

HASH_SIZE → size of hash table (prime number to reduce collisions)

### 📝 Notes

The LRU cache design closely follows the structure used in LeetCode 146: LRU Cache, adapted into C using structs, pointers, and manual memory management.

All messages are fixed-size and page-aligned to support efficient caching.


