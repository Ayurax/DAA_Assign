#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;

struct Movie {
    string title;
    int release_year;
    float rating;
    int watch_time;
};

// Compare movies using the selected field.
int compareMovies(const Movie &a, const Movie &b, int key) {
    if (key == 1) {
        if (a.rating > b.rating) return 1;
        if (a.rating < b.rating) return -1;
    } else if (key == 2) {
        if (a.release_year > b.release_year) return 1;
        if (a.release_year < b.release_year) return -1;
    } else {
        if (a.watch_time > b.watch_time) return 1;
        if (a.watch_time < b.watch_time) return -1;
    }
    return 0;
}

// Partition the array using the last movie as pivot.
int partition(vector<Movie> &movies, int low, int high, int key) {
    Movie pivot = movies[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (compareMovies(movies[j], pivot, key) >= 0) {
            i++;
            swap(movies[i], movies[j]);
        }
    }

    swap(movies[i + 1], movies[high]);
    return i + 1;
}

void quickSort(vector<Movie> &movies, int low, int high, int key) {
    if (low < high) {
        int pi = partition(movies, low, high, key);
        quickSort(movies, low, pi - 1, key);
        quickSort(movies, pi + 1, high, key);
    }
}

// Read the four-column movies.csv file.
vector<Movie> loadMovies(string filename) {
    vector<Movie> movies;
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Error opening " << filename << ".\n";
        return movies;
    }

    string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        string title, yearText, ratingText, watchText;
        stringstream ss(line);

        getline(ss, title, ',');
        getline(ss, yearText, ',');
        getline(ss, ratingText, ',');
        getline(ss, watchText, ',');

        try {
            Movie movie;
            movie.title = title;
            movie.release_year = stoi(yearText);
            movie.rating = stof(ratingText);
            movie.watch_time = stoi(watchText);
            movies.push_back(movie);
        } catch (...) {
            // Ignore rows with invalid values.
        }
    }

    file.close();
    return movies;
}

void displayMovies(const vector<Movie> &movies) {
    int limit = min(10, static_cast<int>(movies.size()));

    cout << "\nTop " << limit << " movies:\n";
    cout << left << setw(5) << "No." << setw(42) << "Title"
         << setw(8) << "Year" << setw(9) << "Rating"
         << "Watch Time (min)\n";

    for (int i = 0; i < limit; i++) {
        cout << left << setw(5) << i + 1
             << setw(42) << movies[i].title.substr(0, 40)
             << setw(8) << movies[i].release_year
             << setw(9) << fixed << setprecision(1) << movies[i].rating
             << movies[i].watch_time << '\n';
    }
}

int main() {
    vector<Movie> movies = loadMovies("movies.csv");

    if (movies.empty()) {
        cout << "No movies loaded.\n";
        return 1;
    }

    cout << "StreamFlix Movie Quick Sort\n";
    cout << "Loaded " << movies.size() << " movies.\n";
    cout << "Sort by:\n1. IMDb Rating\n2. Release Year\n3. Watch Time\n";
    cout << "Enter choice: ";

    int key;
    cin >> key;

    if (key < 1 || key > 3) {
        cout << "Invalid choice.\n";
        return 1;
    }

    auto start = high_resolution_clock::now();
    quickSort(movies, 0, static_cast<int>(movies.size()) - 1, key);
    auto end = high_resolution_clock::now();

    displayMovies(movies);

    duration<double> elapsed = end - start;
    cout << "\nSorted " << movies.size() << " movies in "
         << fixed << setprecision(6) << elapsed.count() << " seconds.\n";

    return 0;
}
