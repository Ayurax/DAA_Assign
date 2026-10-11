#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace std;

struct Supply {
    string name;
    double weight;
    double value;
    bool divisible;

    double ratio() const {
        return value / weight;
    }
};

int main() {
    cout << "===============================================\n";
    cout << "     EMERGENCY RELIEF SUPPLY DISTRIBUTION\n";
    cout << "             FRACTIONAL KNAPSACK\n";
    cout << "===============================================\n\n";

    int n;
    cout << "Enter number of supply items: ";
    if (!(cin >> n) || n <= 0 || n > 1000) {
        cerr << "Please enter a number between 1 and 1000.\n";
        return 1;
    }

    vector<Supply> supplies;
    for (int i = 0; i < n; ++i) {
        Supply item{};
        int divisibleInput;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\nItem " << i + 1 << "\n";
        cout << "Name: ";
        getline(cin, item.name);
        if (item.name.empty()) {
            cerr << "Item name cannot be empty.\n";
            return 1;
        }

        cout << "Weight (kg): ";
        if (!(cin >> item.weight) || item.weight <= 0) {
            cerr << "Weight must be greater than zero.\n";
            return 1;
        }
        cout << "Utility value: ";
        if (!(cin >> item.value) || item.value < 0) {
            cerr << "Utility value cannot be negative.\n";
            return 1;
        }
        cout << "Divisible? (1 = yes, 0 = no): ";
        if (!(cin >> divisibleInput) || (divisibleInput != 0 && divisibleInput != 1)) {
            cerr << "Enter 1 for divisible or 0 for indivisible.\n";
            return 1;
        }
        item.divisible = (divisibleInput == 1);
        supplies.push_back(item);
    }

    double capacity;
    cout << "\nMaximum boat capacity (kg): ";
    if (!(cin >> capacity) || capacity < 0) {
        cerr << "Capacity cannot be negative.\n";
        return 1;
    }

    // Greedy choice: consider the item with the highest utility per kg first.
    sort(supplies.begin(), supplies.end(), [](const Supply& a, const Supply& b) {
        return a.ratio() > b.ratio();
    });

    cout << "\nSupplies sorted by utility per kg:\n";
    cout << left << setw(22) << "Supply" << right
         << setw(12) << "Weight" << setw(12) << "Utility"
         << setw(12) << "Ratio" << setw(14) << "Type" << '\n';
    cout << string(72, '-') << '\n';
    cout << fixed << setprecision(2);
    for (const Supply& item : supplies) {
        cout << left << setw(22) << item.name << right
             << setw(12) << item.weight << setw(12) << item.value
             << setw(12) << item.ratio()
             << setw(14) << (item.divisible ? "Divisible" : "Whole only") << '\n';
    }

    double remaining = capacity;
    double totalWeight = 0.0;
    double totalUtility = 0.0;

    cout << "\nSupply selection:\n";
    for (const Supply& item : supplies) {
        if (remaining <= 0.000000001)
            break;

        if (item.weight <= remaining) {
            cout << "- " << item.name << ": FULL, " << item.weight
                 << " kg, utility " << item.value << '\n';
            remaining -= item.weight;
            totalWeight += item.weight;
            totalUtility += item.value;
        } else if (item.divisible) {
            const double takenWeight = remaining;
            const double fraction = takenWeight / item.weight;
            const double takenUtility = fraction * item.value;
            cout << "- " << item.name << ": PARTIAL (" << fraction * 100.0
                 << "%), " << takenWeight << " kg, utility " << takenUtility << '\n';
            totalWeight += takenWeight;
            totalUtility += takenUtility;
            remaining = 0.0;
        } else {
            cout << "- " << item.name
                 << ": SKIPPED (indivisible item does not fit)\n";
        }
    }

    cout << "\n===============================================\n";
    cout << "                 FINAL REPORT\n";
    cout << "===============================================\n";
    cout << "Boat capacity       : " << capacity << " kg\n";
    cout << "Total weight carried: " << totalWeight << " kg\n";
    cout << "Remaining capacity  : " << remaining << " kg\n";
    cout << "Maximum utility     : " << totalUtility << " units\n";
    cout << "Algorithm           : Fractional Knapsack (Greedy)\n";
    cout << "Time complexity     : O(n log n)\n";
    cout << "Auxiliary space     : O(log n) average for sorting stack\n";
    cout << "===============================================\n";
    return 0;
}
