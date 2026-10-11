/*
 * Assignment 1 - Merge Sort
 * -------------------------
 * Problem : Sort the Olist Brazilian E-Commerce orders dataset in ascending
 *           chronological order of each order's `order_purchase_timestamp`,
 *           using a manually implemented (no std::sort) stable Merge Sort.
 *
 * Dataset : "olist_orders_dataset.csv"
 *           Source : https://www.kaggle.com/datasets/olistbr/brazilian-ecommerce
 *           Columns : order_id, customer_id, order_status,
 *                     order_purchase_timestamp, order_approved_at,
 *                     order_delivered_carrier_date,
 *                     order_delivered_customer_date,
 *                     order_estimated_delivery_date
 *
 * Approach: Divide and Conquer.
 *           1. Divide  : Recursively split the array into two halves.
 *           2. Conquer : Sort each half recursively.
 *           3. Merge   : Merge the two sorted halves into one sorted array.
 *
 * The merge is STABLE: when two timestamps are equal, the record that appeared
 * first in the original input is placed first in the sorted output.
 *
 * A single reusable temporary buffer is allocated once and shared across every
 * merge call so that sorting large datasets does not trigger per-merge memory
 * allocations.
 *
 * Complexity:
 *   Time  : O(n log n) in best, average, and worst cases.
 *   Space : O(n) auxiliary space (one temporary buffer of size n).
 *
 * Compilation (Ubuntu Linux with GCC/G++):
 *   g++ -std=c++17 -O2 -Wall -Wextra -pedantic customer_order_merge_sort.cpp -o merge_sort
 *
 * Usage:
 *   ./merge_sort                                        # default files
 *   ./merge_sort olist_orders_dataset.csv               # custom input
 *   ./merge_sort olist_orders_dataset.csv my_out.csv    # custom input + output
 *
 * Only the C++ Standard Library is used. No external dependencies.
 */

#include <algorithm>
#include <chrono>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Data types
// ---------------------------------------------------------------------------

// Parsed representation of a timestamp "YYYY-MM-DD HH:MM:SS".
struct Timestamp {
    int year   = 0;
    int month  = 0;
    int day    = 0;
    int hour   = 0;
    int minute = 0;
    int second = 0;
    bool valid = false;
};

// A single order record. All original CSV columns are kept in `fields`
// (in their original order) so that every value is preserved; `ts` is the
// parsed sort key (the purchase timestamp).
struct Record {
    std::vector<std::string> fields;
    Timestamp ts;
};

// ---------------------------------------------------------------------------
// Small string helpers
// ---------------------------------------------------------------------------

static std::string trim(const std::string& s) {
    const std::size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    const std::size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

static bool isAllDigits(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    }
    return true;
}

static int toInt(const std::string& s) {
    int value = 0;
    for (char c : s) {
        value = value * 10 + (c - '0');
    }
    return value;
}

// Removes a single trailing carriage return (defensive against CRLF files).
static std::string stripCr(const std::string& s) {
    if (!s.empty() && s.back() == '\r') {
        return s.substr(0, s.size() - 1);
    }
    return s;
}

// ---------------------------------------------------------------------------
// Timestamp parsing and validation
// ---------------------------------------------------------------------------

// Validates and parses the "YYYY-MM-DD HH:MM:SS" format.
// Returns a Timestamp whose `valid` flag is true only when the input is a
// real, well-formed calendar timestamp (including leap-year handling).
static Timestamp parseTimestamp(const std::string& raw) {
    Timestamp t;
    t.valid = false;

    // The canonical format is exactly 19 characters long.
    if (raw.size() != 19) return t;

    // Check the separators at the fixed positions.
    if (raw[4] != '-' || raw[7] != '-' || raw[10] != ' ' ||
        raw[13] != ':' || raw[16] != ':') {
        return t;
    }

    const std::string year   = raw.substr(0, 4);
    const std::string month  = raw.substr(5, 2);
    const std::string day    = raw.substr(8, 2);
    const std::string hour   = raw.substr(11, 2);
    const std::string minute = raw.substr(14, 2);
    const std::string second = raw.substr(17, 2);

    if (!isAllDigits(year)   || !isAllDigits(month)  || !isAllDigits(day)   ||
        !isAllDigits(hour)   || !isAllDigits(minute) || !isAllDigits(second)) {
        return t;
    }

    t.year   = toInt(year);
    t.month  = toInt(month);
    t.day    = toInt(day);
    t.hour   = toInt(hour);
    t.minute = toInt(minute);
    t.second = toInt(second);

    // Range validation.
    if (t.month < 1 || t.month > 12) return t;
    if (t.day < 1 || t.day > 31) return t;
    if (t.hour < 0 || t.hour > 23) return t;
    if (t.minute < 0 || t.minute > 59) return t;
    if (t.second < 0 || t.second > 59) return t;

    // Days-of-month validation with leap-year support.
    static const int daysInMonth[] = {31, 28, 31, 30, 31, 30,
                                      31, 31, 30, 31, 30, 31};
    int maxDay = daysInMonth[t.month - 1];
    const bool isLeap =
        (t.year % 4 == 0 && t.year % 100 != 0) || (t.year % 400 == 0);
    if (t.month == 2 && isLeap) maxDay = 29;
    if (t.day > maxDay) return t;

    t.valid = true;
    return t;
}

// Returns true when timestamp `a` is strictly earlier than timestamp `b`.
static bool timestampEarlier(const Timestamp& a, const Timestamp& b) {
    if (a.year != b.year)   return a.year < b.year;
    if (a.month != b.month) return a.month < b.month;
    if (a.day != b.day)     return a.day < b.day;
    if (a.hour != b.hour)   return a.hour < b.hour;
    if (a.minute != b.minute) return a.minute < b.minute;
    return a.second < b.second;
}

// ---------------------------------------------------------------------------
// CSV parsing and writing (RFC-4180 style: quoted fields, embedded commas,
// escaped double-quotes ("") and empty fields)
// ---------------------------------------------------------------------------

static std::vector<std::string> parseCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;
    std::size_t i = 0;

    while (i < line.size()) {
        const char c = line[i];
        if (inQuotes) {
            if (c == '"') {
                // Doubled quote ("") is an escaped quote inside a quoted field.
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    field += '"';
                    i += 2;
                } else {
                    inQuotes = false;
                    i += 1;
                }
            } else {
                field += c;
                i += 1;
            }
        } else {
            if (c == '"') {
                inQuotes = true;
                i += 1;
            } else if (c == ',') {
                fields.push_back(field);
                field.clear();
                i += 1;
            } else {
                field += c;
                i += 1;
            }
        }
    }

    fields.push_back(field);  // last field (or the only one)
    return fields;
}

// Escapes a field for CSV output, quoting only when necessary.
static std::string csvEscape(const std::string& s) {
    bool needsQuote = false;
    for (char c : s) {
        if (c == ',' || c == '"' || c == '\r' || c == '\n') {
            needsQuote = true;
            break;
        }
    }
    if (!needsQuote) return s;

    std::string out;
    out.reserve(s.size() + 2);
    out += '"';
    for (char c : s) {
        if (c == '"') {
            out += "\"\"";
        } else {
            out += c;
        }
    }
    out += '"';
    return out;
}

// Returns the (0-based) column index whose trimmed header equals `name`,
// or -1 if no such column exists.
static int findColumn(const std::vector<std::string>& header,
                      const std::string& name) {
    for (std::size_t i = 0; i < header.size(); ++i) {
        if (trim(header[i]) == name) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// ---------------------------------------------------------------------------
// Merge Sort (manual, stable, reusable temporary buffer)
// ---------------------------------------------------------------------------

// Merges the two sorted sub-arrays arr[left..mid] and arr[mid+1..right].
// Elements are first copied from `arr` into the shared `temp` buffer, then
// written back into `arr` in sorted order. Because equal timestamps are taken
// from the left sub-array first, the merge is stable.
static void merge(std::vector<Record>& arr, std::vector<Record>& temp,
                  int left, int mid, int right) {
    // Copy the range that is about to be merged into the shared buffer.
    for (int i = left; i <= right; ++i) {
        temp[i] = arr[i];
    }

    int i = left;      // cursor for the left  sub-array
    int j = mid + 1;   // cursor for the right sub-array
    int k = left;      // cursor for the merged result

    while (i <= mid && j <= right) {
        // Take from the left when its timestamp is NOT strictly later than the
        // right one. On equal timestamps this selects the left element first,
        // which is exactly what makes the merge stable.
        if (!timestampEarlier(temp[j].ts, temp[i].ts)) {
            arr[k] = temp[i];
            ++i;
        } else {
            arr[k] = temp[j];
            ++j;
        }
        ++k;
    }

    // Drain whatever remains (only one of these loops will actually run).
    while (i <= mid) {
        arr[k] = temp[i];
        ++i;
        ++k;
    }
    while (j <= right) {
        arr[k] = temp[j];
        ++j;
        ++k;
    }
}

// Recursive divide-and-conquer driver.
static void mergeSortRec(std::vector<Record>& arr, std::vector<Record>& temp,
                         int left, int right) {
    if (left >= right) return;  // base case: 0 or 1 element
    const int mid = left + (right - left) / 2;  // avoids integer overflow
    mergeSortRec(arr, temp, left, mid);         // sort left half
    mergeSortRec(arr, temp, mid + 1, right);    // sort right half
    merge(arr, temp, left, mid, right);         // merge the two halves
}

// Public entry point. Allocates the temporary buffer once and reuses it.
static void mergeSort(std::vector<Record>& arr) {
    if (arr.empty()) return;
    std::vector<Record> temp(arr.size());
    mergeSortRec(arr, temp, 0, static_cast<int>(arr.size()) - 1);
}

// ---------------------------------------------------------------------------
// File I/O
// ---------------------------------------------------------------------------

// Reads the dataset into `records`. On success returns 0 and fills:
//   - header      : the column names from the header row
//   - sortCol     : index of "order_purchase_timestamp"
//   - malformedCount : number of rows skipped (invalid field count or
//                      bad/missing purchase timestamp). Skipped rows are
//                      reported to stderr; nothing is discarded silently.
static int readOrders(const std::string& inputFile,
                      std::vector<std::string>& header,
                      std::vector<Record>& records,
                      int& sortCol, int& malformedCount) {
    malformedCount = 0;

    std::ifstream in(inputFile);
    if (!in.is_open()) {
        std::cerr << "Error: Could not open input file '" << inputFile
                  << "' for reading.\n";
        return 1;
    }

    std::string line;

    // --- Header -----------------------------------------------------------
    if (!std::getline(in, line)) {
        // Completely empty file (no lines at all).
        std::cerr << "Notice: Input file '" << inputFile << "' is empty "
                  << "(0 bytes). No records were processed.\n";
        return 0;
    }
    line = stripCr(line);

    header = parseCsvLine(line);
    if (header.empty()) {
        std::cerr << "Error: Header row is empty.\n";
        return 1;
    }

    sortCol = findColumn(header, "order_purchase_timestamp");
    if (sortCol < 0) {
        std::cerr << "Error: The input file must contain an "
                  << "'order_purchase_timestamp' column.\n";
        return 1;
    }

    const int maxCol = static_cast<int>(header.size()) - 1;

    // --- Data records -----------------------------------------------------
    int lineNumber = 1;  // the header occupied line 1
    while (std::getline(in, line)) {
        ++lineNumber;
        line = stripCr(line);
        if (line.empty()) continue;  // skip blank lines

        std::vector<std::string> fields = parseCsvLine(line);

        // A well-formed row must carry exactly one field per header column.
        if (static_cast<int>(fields.size()) != maxCol + 1) {
            std::cerr << "Warning: Line " << lineNumber
                      << ": expected " << (maxCol + 1)
                      << " fields but found " << fields.size()
                      << "; skipping row.\n";
            ++malformedCount;
            continue;
        }

        if (sortCol >= static_cast<int>(fields.size())) {
            std::cerr << "Warning: Line " << lineNumber
                      << ": missing purchase timestamp field; skipping row.\n";
            ++malformedCount;
            continue;
        }

        // Validate the purchase-timestamp format. A row whose sort key cannot
        // be parsed cannot be sorted; report it clearly rather than discarding
        // it silently.
        Timestamp ts = parseTimestamp(trim(fields[sortCol]));
        if (!ts.valid) {
            std::cerr << "Warning: Line " << lineNumber
                      << ": invalid purchase timestamp '"
                      << fields[sortCol]
                      << "' (expected YYYY-MM-DD HH:MM:SS); skipping row.\n";
            ++malformedCount;
            continue;
        }

        Record rec;
        rec.fields = std::move(fields);
        rec.ts = ts;
        records.push_back(std::move(rec));
    }

    in.close();
    return 0;
}

// Writes the header and every (sorted) record, preserving all original
// columns and their values. Returns 0 on success.
static int writeOrders(const std::string& outputFile,
                       const std::vector<std::string>& header,
                       const std::vector<Record>& records) {
    std::ofstream out(outputFile);
    if (!out.is_open()) {
        std::cerr << "Error: Could not open output file '" << outputFile
                  << "' for writing.\n";
        return 1;
    }

    // Header row.
    for (std::size_t i = 0; i < header.size(); ++i) {
        if (i != 0) out << ',';
        out << csvEscape(header[i]);
    }
    out << '\n';

    // Data rows.
    for (const Record& r : records) {
        for (std::size_t i = 0; i < r.fields.size(); ++i) {
            if (i != 0) out << ',';
            out << csvEscape(r.fields[i]);
        }
        out << '\n';
    }

    if (!out) {
        std::cerr << "Error: A failure occurred while writing to '"
                  << outputFile << "'.\n";
        return 1;
    }
    out.close();
    return 0;
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    const std::string inputFile =
        (argc > 1) ? std::string(argv[1]) : "olist_orders_dataset.csv";
    const std::string outputFile =
        (argc > 2) ? std::string(argv[2]) : "sorted_olist_orders_dataset.csv";

    auto totalStart = std::chrono::high_resolution_clock::now();

    std::vector<std::string> header;
    std::vector<Record> records;
    int sortCol = -1;
    int malformedCount = 0;

    // --- Read (timed) -----------------------------------------------------
    auto readStart = std::chrono::high_resolution_clock::now();
    const int readStatus = readOrders(inputFile, header, records,
                                      sortCol, malformedCount);
    auto readEnd = std::chrono::high_resolution_clock::now();
    const double readMs =
        std::chrono::duration<double, std::milli>(readEnd - readStart).count();

    if (readStatus != 0) {
        // Fatal error (missing file or bad header); nothing to sort.
        return readStatus;
    }

    // --- Sort (timed separately) -----------------------------------------
    auto sortStart = std::chrono::high_resolution_clock::now();
    mergeSort(records);
    auto sortEnd = std::chrono::high_resolution_clock::now();
    const double sortMs =
        std::chrono::duration<double, std::milli>(sortEnd - sortStart).count();

    // --- Write (timed) ----------------------------------------------------
    auto writeStart = std::chrono::high_resolution_clock::now();
    const int writeStatus = writeOrders(outputFile, header, records);
    auto writeEnd = std::chrono::high_resolution_clock::now();
    const double writeMs =
        std::chrono::duration<double, std::milli>(writeEnd - writeStart).count();

    if (writeStatus != 0) {
        return writeStatus;
    }

    auto totalEnd = std::chrono::high_resolution_clock::now();
    const double totalMs =
        std::chrono::duration<double, std::milli>(totalEnd - totalStart).count();

    // --- Report -----------------------------------------------------------
    const int orderIdCol = findColumn(header, "order_id");
    const int statusCol  = findColumn(header, "order_status");

    std::cout << "=== Merge Sort on Olist Orders ==="
              << "\nInput file  : " << inputFile
              << "\nOutput file : " << outputFile
              << "\nSort column : order_purchase_timestamp"
              << "\nTotal records processed: " << records.size()
              << "\nMalformed records skipped: " << malformedCount << "\n\n";

    std::cout << "First 10 sorted records (ascending by order_purchase_timestamp):\n";
    const std::size_t displayCount = std::min<std::size_t>(10, records.size());
    for (std::size_t i = 0; i < displayCount; ++i) {
        const Record& r = records[i];
        std::string prefix;
        if (orderIdCol >= 0) prefix += r.fields[orderIdCol];
        if (statusCol >= 0)  prefix += " | " + r.fields[statusCol];
        std::cout << "  " << (i + 1) << ". " << prefix << " | "
                  << r.fields[sortCol] << "\n";
    }
    if (records.empty()) {
        std::cout << "  (no records to display)\n";
    }

    std::cout << "\n--- Timing ---\n"
              << "File read time      : " << std::fixed
              << std::setprecision(4) << readMs << " ms\n"
              << "Sorting time        : " << std::fixed
              << std::setprecision(4) << sortMs << " ms\n"
              << "File write time     : " << std::fixed
              << std::setprecision(4) << writeMs << " ms\n"
              << "Total processing time: " << std::fixed
              << std::setprecision(4) << totalMs << " ms\n";

    return 0;
}
