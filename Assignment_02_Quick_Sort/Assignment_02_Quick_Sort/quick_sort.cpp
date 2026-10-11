#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

struct Movie {
    vector<string> data;
    double rating;
    int year, runtime;
    long long votes;
};

vector<string> splitCSV(const string& line) {
    vector<string> fields;
    string field;
    bool quoted = false;
    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];
        if (c == '"') {
            if (quoted && i + 1 < line.size() && line[i + 1] == '"') {
                field += '"';
                i++;
            } else quoted = !quoted;
        } else if (c == ',' && !quoted) {
            fields.push_back(field);
            field.clear();
        } else field += c;
    }
    fields.push_back(field);
    return fields;
}

string quoteCSV(const string& s) {
    if (s.find_first_of(",\"\r\n") == string::npos) return s;
    string out = "\"";
    for (char c : s) {
        if (c == '"') out += "\"\"";
        else out += c;
    }
    return out + "\"";
}

int number(const string& s) {
    stringstream ss(s);
    int n = 0;
    ss >> n;
    return n;
}

double ratingValue(const Movie& m, int key) {
    if (key == 1) return m.rating;
    if (key == 2) return m.year;
    if (key == 3) return m.runtime;
    return static_cast<double>(m.votes);
}

void quickSort(vector<Movie>& a, int low, int high, int key, bool desc) {
    if (low >= high) return;
    double pivot = ratingValue(a[high], key);
    int i = low - 1;
    for (int j = low; j < high; j++) {
        double value = ratingValue(a[j], key);
        if (desc ? value > pivot : value < pivot) {
            i++;
            swap(a[i], a[j]);
        }
    }
    swap(a[i + 1], a[high]);
    int p = i + 1;
    quickSort(a, low, p - 1, key, desc);
    quickSort(a, p + 1, high, key, desc);
}

int main() {
    ifstream in("imdb_top_1000.csv");
    if (!in) {
        cout << "Could not open imdb_top_1000.csv\n";
        return 1;
    }

    string line;
    getline(in, line);
    vector<string> header = splitCSV(line);
    vector<Movie> movies;
    while (getline(in, line)) {
        vector<string> f = splitCSV(line);
        if (f.size() != header.size()) continue;
        Movie m;
        m.data = f;
        m.year = number(f[2]);
        m.runtime = number(f[4]);
        m.rating = 0;
        stringstream rs(f[6]);
        rs >> m.rating;
        m.votes = 0;
        stringstream vs(f[14]);
        vs >> m.votes;
        movies.push_back(m);
    }
    in.close();

    cout << "Movie Recommendation System - Quick Sort\n";
    cout << "Movies loaded: " << movies.size() << "\n";
    cout << "Sort by: 1.Rating  2.Year  3.Runtime  4.Votes\nChoice: ";
    int key;
    cin >> key;
    if (key < 1 || key > 4) {
        cout << "Invalid choice\n";
        return 1;
    }
    cout << "Order (A ascending / D descending): ";
    char direction;
    cin >> direction;
    bool desc = direction == 'D' || direction == 'd';

    quickSort(movies, 0, static_cast<int>(movies.size()) - 1, key, desc);

    ofstream out("sorted_movies.csv");
    for (size_t i = 0; i < header.size(); i++) {
        if (i) out << ',';
        out << quoteCSV(header[i]);
    }
    out << '\n';
    for (const Movie& m : movies) {
        for (size_t i = 0; i < m.data.size(); i++) {
            if (i) out << ',';
            out << quoteCSV(m.data[i]);
        }
        out << '\n';
    }
    out.close();

    cout << "Sorted records written: " << movies.size() << "\n";
    cout << "Output file: sorted_movies.csv\n";
    cout << "First 10 movies:\n";
    for (int i = 0; i < 10 && i < static_cast<int>(movies.size()); i++)
        cout << i + 1 << ". " << movies[i].data[1]
             << " | Rating: " << movies[i].data[6]
             << " | Year: " << movies[i].data[2] << "\n";
    return 0;
}
