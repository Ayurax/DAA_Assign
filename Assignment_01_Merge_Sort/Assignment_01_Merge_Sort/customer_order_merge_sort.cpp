#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

struct Order {
    string timestamp;
    string row;       // Keep the complete original CSV row unchanged.
    string orderId;
    string status;
};

// Split a CSV row while respecting quoted fields and escaped quotes.
vector<string> splitCSV(const string& line) {
    vector<string> fields;
    string field;
    bool quoted = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char ch = line[i];
        if (ch == '"') {
            if (quoted && i + 1 < line.size() && line[i + 1] == '"') {
                field += '"';
                ++i;
            } else quoted = !quoted;
        } else if (ch == ',' && !quoted) {
            fields.push_back(field);
            field.clear();
        } else field += ch;
    }
    fields.push_back(field);
    return fields;
}

bool validTimestamp(const string& s) {
    if (s.size() != 19 || s[4] != '-' || s[7] != '-' || s[10] != ' ' ||
        s[13] != ':' || s[16] != ':') return false;
    for (size_t i = 0; i < s.size(); ++i) {
        if (i == 4 || i == 7 || i == 10 || i == 13 || i == 16) continue;
        if (s[i] < '0' || s[i] > '9') return false;
    }
    return true;
}

int findColumn(const vector<string>& header, const string& name) {
    for (size_t i = 0; i < header.size(); ++i)
        if (header[i] == name) return static_cast<int>(i);
    return -1;
}

// Merge is stable: equal timestamps are taken from the left half first.
void merge(vector<Order>& orders, vector<Order>& temp, int left, int mid, int right) {
    for (int i = left; i <= right; ++i) temp[i] = orders[i];
    int i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right) {
        if (temp[i].timestamp <= temp[j].timestamp)
            orders[k++] = move(temp[i++]);
        else
            orders[k++] = move(temp[j++]);
    }
    while (i <= mid) orders[k++] = move(temp[i++]);
    while (j <= right) orders[k++] = move(temp[j++]);
}

void mergeSort(vector<Order>& orders, vector<Order>& temp, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(orders, temp, left, mid);
    mergeSort(orders, temp, mid + 1, right);
    merge(orders, temp, left, mid, right);
}

int main(int argc, char* argv[]) {
    string inputFile = argc > 1 ? argv[1] : "olist_orders_dataset.csv";
    string outputFile = argc > 2 ? argv[2] : "sorted_olist_orders_dataset.csv";
    ifstream input(inputFile);
    if (!input) {
        cerr << "Error: cannot open input file: " << inputFile << '\n';
        return 1;
    }

    string headerLine;
    if (!getline(input, headerLine)) {
        cerr << "Error: input file is empty or has no header.\n";
        return 1;
    }
    if (!headerLine.empty() && headerLine.back() == '\r') headerLine.pop_back();
    vector<string> header = splitCSV(headerLine);
    int timeCol = findColumn(header, "order_purchase_timestamp");
    int idCol = findColumn(header, "order_id");
    int statusCol = findColumn(header, "order_status");
    if (timeCol == -1) {
        cerr << "Error: order_purchase_timestamp column was not found.\n";
        return 1;
    }

    vector<Order> orders;
    string line;
    int badRows = 0, lineNo = 1;
    auto readStart = chrono::steady_clock::now();
    while (getline(input, line)) {
        ++lineNo;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        vector<string> fields = splitCSV(line);
        if (fields.size() != header.size() || timeCol >= static_cast<int>(fields.size()) ||
            !validTimestamp(fields[timeCol])) {
            cerr << "Warning: skipped malformed row at line " << lineNo << '\n';
            ++badRows;
            continue;
        }
        Order order;
        order.timestamp = fields[timeCol];
        order.row = line;
        if (idCol >= 0) order.orderId = fields[idCol];
        if (statusCol >= 0) order.status = fields[statusCol];
        orders.push_back(move(order));
    }
    input.close();
    auto readEnd = chrono::steady_clock::now();

    auto sortStart = chrono::steady_clock::now();
    if (!orders.empty()) {
        vector<Order> temp(orders.size());
        mergeSort(orders, temp, 0, static_cast<int>(orders.size()) - 1);
    }
    auto sortEnd = chrono::steady_clock::now();

    auto writeStart = chrono::steady_clock::now();
    ofstream output(outputFile);
    if (!output) {
        cerr << "Error: cannot create output file: " << outputFile << '\n';
        return 1;
    }
    output << headerLine << '\n';
    for (const Order& order : orders) output << order.row << '\n';
    output.close();
    if (!output) {
        cerr << "Error: failed while writing output file.\n";
        return 1;
    }
    auto writeEnd = chrono::steady_clock::now();

    auto readMs = chrono::duration<double, milli>(readEnd - readStart).count();
    auto sortMs = chrono::duration<double, milli>(sortEnd - sortStart).count();
    auto writeMs = chrono::duration<double, milli>(writeEnd - writeStart).count();
    cout << "=== Merge Sort on Olist Orders ===\n"
         << "Input file  : " << inputFile << '\n'
         << "Output file : " << outputFile << '\n'
         << "Sort column : order_purchase_timestamp\n"
         << "Total records processed: " << orders.size() << '\n'
         << "Malformed records skipped: " << badRows << "\n\n"
         << "First 10 sorted records (ascending timestamp):\n";
    size_t limit = min<size_t>(10, orders.size());
    for (size_t i = 0; i < limit; ++i) {
        cout << i + 1 << ". ";
        if (!orders[i].orderId.empty()) cout << orders[i].orderId << " | ";
        if (!orders[i].status.empty()) cout << orders[i].status << " | ";
        cout << orders[i].timestamp << '\n';
    }
    cout << "\n--- Timing ---\n"
         << "File read time      : " << readMs << " ms\n"
         << "Sorting time        : " << sortMs << " ms\n"
         << "File write time     : " << writeMs << " ms\n";
    return 0;
}
