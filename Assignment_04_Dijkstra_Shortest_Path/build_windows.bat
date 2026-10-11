@echo off
g++ -std=c++17 -Wall -Wextra -pedantic dijkstra.cpp -o dijkstra.exe
if errorlevel 1 pause & exit /b 1
dijkstra.exe
pause
