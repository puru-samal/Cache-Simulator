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

#: << COMMENT csim tests
./csim -s 2 -E 3 -b 4 -t traces/csim/dave.trace 
#COMMENT


