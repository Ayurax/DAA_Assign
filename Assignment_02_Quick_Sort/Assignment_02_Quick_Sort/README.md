# Assignment 2 — Quick Sort

**Problem:** Movie Recommendation System Optimization  
**Language:** C++  
**Dataset:** IMDb Top 1000 Movies (`imdb_top_1000.csv`)

The program sorts the movie records by IMDb rating, release year, runtime, or number of votes. Choose ascending or descending order when it runs. It uses the Quick Sort partition method with the last element as the pivot and keeps each movie's complete row together.

## Compile and run (Ubuntu)
```bash
g++ -std=c++17 -Wall -Wextra -pedantic quick_sort.cpp -o quick_sort
./quick_sort
```

Keep `imdb_top_1000.csv` in the same folder as the program. The sorted records are written to `sorted_movies.csv`.

## Complexity
- Best/Average time: O(n log n)
- Worst-case time: O(n²)
- Average auxiliary space: O(log n) for recursion

Dataset source: https://www.kaggle.com/datasets/harshitshankhdhar/imdb-dataset-of-top-1000-movies-and-tv-shows
