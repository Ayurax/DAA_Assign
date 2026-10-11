#!/bin/sh
set -eu
g++ -std=c++17 -Wall -Wextra -pedantic dijkstra.cpp -o dijkstra
./dijkstra
