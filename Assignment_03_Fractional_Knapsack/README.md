# Assignment 3 — Fractional Knapsack

**Scenario:** Emergency Relief Supply Distribution

The program asks for the name, weight, utility value, and divisibility of each supply item, plus the boat's capacity. It calculates `utility / weight`, sorts supplies from highest to lowest ratio, and then uses a greedy selection process:

- Take a whole item when it fits.
- If it does not fit and can be divided, take the remaining fraction.
- If it cannot be divided, skip it and check the next item.

No external dataset is required. `Sample_Input.txt` contains the values used to create `Output.txt`.

## Files

- `fractional_knapsack.cpp` — source code.
- `Fractional.exe` — generated on Windows by `build_windows.bat`.
- `fractional_knapsack_linux` — prebuilt Linux x86-64 executable.
- `Sample_Input.txt` — test input values.
- `Output.txt` — output from an actual run of the program.
- `build_windows.bat` — compiles and runs the program on Windows with MinGW-w64 g++ installed.
- `run_linux.sh` — compiles and runs the program on Ubuntu/Linux.

## Run on Windows

Open this folder in Windows and double-click `build_windows.bat`. It uses the `g++` compiler and creates `Fractional.exe` in the same folder. Or run these commands in PowerShell:

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic fractional_knapsack.cpp -o Fractional.exe
.\Fractional.exe
```

## Run on Ubuntu/Linux

```bash
chmod +x run_linux.sh
./run_linux.sh
```

## Complexity

Sorting the items takes `O(n log n)` time, and the selection pass takes `O(n)`, so the overall time complexity is `O(n log n)`. Sorting uses `O(log n)` auxiliary stack space on average with typical `std::sort` implementations.

**Note:** The assignment scenario says to skip indivisible items that do not fit. That behavior is implemented here. For a mixture of divisible and indivisible items, this ratio-based greedy rule is not guaranteed to find the global optimum in every possible input; the classic Fractional Knapsack guarantee assumes items are divisible.
