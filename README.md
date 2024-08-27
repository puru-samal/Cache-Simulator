# Cache Simulator

This program simulates memory accesses via a cache and the number of hits, misses, dirty bytes in cache and dirty bytes evicted based on cache configuration and the operations provided. Tested on `64-bit Ubuntu 22.04.1 LTS (Linux kernel 5.15.0)`.

# Usage

`./csim [-hv] -s <s> -E <E> -b <b> -t <trace>`

- `-h`: Prints help message
- `-v`: Verbose mode: Reports effects of each memory operation
- `-s <s>`: Number of set index bits. (there are 2\*\*s sets)
- `-E <E>`: Number of lines per set (associativity)
- `-b <b>`: Number of block bits. (there are 2\*\*b blocks)
- `-t <trace>`: File name of the memory trace to process

Format for trace files: `op addr,size`

The Cache is implemented as an array of singly-linked lists as it simplifies
the process of maintaining order of elements based on how recently they were
used, which is required to handle eviction under the LRU policy.

# Running

compile with:

    linux> make

Check the correctness:

    linux> ./test-csim

Check the correctness and performance of transpose functions:

    linux> ./test-trans -M 32 -N 32
    linux> ./test-trans -M 1024 -N 1024

Check everything at once:

    linux> ./driver.py

# Project Structure

- `csim.c`: The cache simulator
- `trans.c`: Transpose function(s) [Starter version included]
- `Makefile`: Builds the simulator and tools
- `README`: This file
- `cachelab.c`: Required helper functions
- `cachelab.h`: Required header file
- `csim-ref*`: The executable reference cache simulator
- `driver.py*`: The cache lab driver program, runs test-csim and test-trans
- `test-csim.c`: Tests your cache simulator
- `test-trans.c`: Tests your transpose function
- `ct/`: Code to support address tracing when running the transpose code
- `tracegen-ct.c`: Helper program used by test-trans, which you can run directly.
- `traces-driver.py`: The driver to test the traces you write
- `traces/`: All trace files used in cachelab
- `traces/traces`: Trace you write for the traces portion of the assignment
- `traces/csim`: Traces used by the driver to test your cache simulator (Part A)
