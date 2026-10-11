# Assignment 4 — Dijkstra's Shortest Path

**Scenario:** Smart Traffic Management for Emergency Vehicles.

This C++ program represents intersections as nodes and roads as edges. Road weights represent travel time in minutes. It runs Dijkstra's algorithm from the ambulance's starting point to find the nearest hospital, then updates one road's travel time and recalculates the route.

## Files
- `dijkstra.cpp` — source code.
- `Output.txt` — output from running the program.
- `README.md` — these instructions.
- `build_windows.bat` — builds and runs with MinGW g++ on Windows.
- `build_linux.sh` — builds and runs with g++ on Linux/Ubuntu.

## Run on Ubuntu
```bash
g++ -std=c++17 -Wall -Wextra -pedantic dijkstra.cpp -o dijkstra
./dijkstra
```

## Run on Windows (MinGW)
Double-click `build_windows.bat`, or run:
```powershell
g++ -std=c++17 -Wall -Wextra -pedantic dijkstra.cpp -o dijkstra.exe
.\dijkstra.exe
```

No external dataset is required; a small example road network is defined in the program.
