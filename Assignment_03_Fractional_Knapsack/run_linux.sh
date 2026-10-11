#!/bin/sh
set -eu
g++ -std=c++17 -O2 -Wall -Wextra -pedantic fractional_knapsack.cpp -o fractional_knapsack_linux
exec ./fractional_knapsack_linux
