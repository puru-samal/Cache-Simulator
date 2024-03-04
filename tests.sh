#!/bin/bash
# Give permission: chmod 700 tests.sh
# Run with -> ./tests.sh

make

: << COMMENT csim missing arguments test
./csim -s -E 1 -b 0 -t traces/csim/dave.trace 
./csim -s 1 -E -b 0 -t traces/csim/dave.trace 
./csim -s 1 -E 1 -b -t traces/csim/dave.trace 
./csim -s 1 -E 1 -b 1 -t
COMMENT

: << COMMENT csim missing flags test
./csim -E 1 -b 0 -t traces/csim/dave.trace 
./csim -s 1 -b 0 -t traces/csim/dave.trace 
./csim -s 1 -E 1 -t traces/csim/dave.trace 
./csim -s 1 -E 1 -b 1
COMMENT

: << COMMENT csim incorrect values test
./csim -s 65 -E 1 -b 0 -t traces/csim/dave.trace 
./csim -s 64 -E -4 -b 0 -t traces/csim/dave.trace 
./csim -s 0 -E 1 -b 65 -t traces/csim/dave.trace 
./csim -s 33 -E 1 -b 33 -t traces/csim/dave.trace 
COMMENT

<<<<<<< HEAD
#: << COMMENT csim tests
./csim -s 2 -E 3 -b 4 -t traces/csim/dave.trace 
#COMMENT


=======
: << COMMENT csim tests
./csim -v -s 0 -E 1 -b 0 -t traces/csim/wide.trace 
./csim -v -s 2 -E 1 -b 2 -t traces/csim/wide.trace 
./csim -v -s 3 -E 2 -b 2 -t traces/csim/load.trace 
./csim -v -s 1 -E 1 -b 1 -t traces/csim/yi2.trace 
./csim -v -s 4 -E 2 -b 4 -t traces/csim/yi.trace 
./csim -v -s 2 -E 1 -b 4 -t traces/csim/dave.trace 
./csim -v -s 2 -E 1 -b 3 -t traces/csim/trans.trace 
./csim -v -s 2 -E 2 -b 3 -t traces/csim/trans.trace 
./csim -v -s 14 -E 1024 -b 3 -t traces/csim/trans.trace 
./csim -v -s 5 -E 1 -b 5 -t traces/csim/trans.trace 
./csim -v -s 5 -E 1 -b 5 -t traces/csim/long.trace 
COMMENT

#: << COMMENT memory leak tests
#valgrind --leak-check=full ./csim -v -s 14 -E 1024 -b 3 -t traces/csim/trans.trace 
#gdb --args ./csim -v -s 0 -E 1 -b 0 -t traces/csim/wide.trace 
#COMMENT

>>>>>>> 126ffe2 (trans_32 added)
