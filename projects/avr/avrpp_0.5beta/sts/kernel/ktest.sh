#!/bin/sh
g++ -DAVRPP_KTEST -Wall -Wextra -I../../include ./test.cpp -oktest.out && ./ktest.out
rm -f ./ktest.out


