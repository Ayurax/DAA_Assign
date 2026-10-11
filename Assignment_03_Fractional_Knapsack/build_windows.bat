@echo off
setlocal
where g++ >nul 2>nul
if errorlevel 1 (
    echo ERROR: g++ was not found.
    echo Install MinGW-w64 g++ or add its bin folder to PATH, then run this file again.
    pause
    exit /b 1
)

echo Compiling Fractional Knapsack...
g++ -std=c++17 -O2 -Wall -Wextra -pedantic fractional_knapsack.cpp -o Fractional.exe
if errorlevel 1 (
    echo Compilation failed.
    pause
    exit /b 1
)

echo.
echo Build successful. Starting program...
echo.
Fractional.exe
pause
endlocal
