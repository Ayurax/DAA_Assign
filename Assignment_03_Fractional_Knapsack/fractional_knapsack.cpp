#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>

using namespace std;

// Structure to store information about each relief item
struct Item {
    string name;
    double weight;
    double value;
    bool divisible;
    int priority;

    Item(string n, double w, double v, bool d, int p)
        : name(n), weight(w), value(v), divisible(d), priority(p) {}

    // Calculate utility value per kg
    double valuePerWeight() const {
        return value / weight;
    }
};

// Sort items according to Value/Weight ratio
bool compare(const Item& a, const Item& b) {
    return a.valuePerWeight() > b.valuePerWeight();
}

// Fractional Knapsack using Greedy Method
double fractionalKnapsack(vector<Item>& items,
                          double capacity,
                          double& totalWeightCarried) {

    // Step 1: Sort according to value/weight ratio
    sort(items.begin(), items.end(), compare);

    cout << "\n==============================================================\n";
    cout << "        ITEMS SORTED BY VALUE / WEIGHT RATIO\n";
    cout << "==============================================================\n";

    cout << left
         << setw(20) << "Item"
         << setw(12) << "Weight"
         << setw(12) << "Value"
         << setw(12) << "Ratio"
         << setw(12) << "Priority"
         << setw(15) << "Type"
         << endl;

    cout << "--------------------------------------------------------------\n";

    for (const auto& item : items) {
        cout << left
             << setw(20) << item.name
             << setw(12) << item.weight
             << setw(12) << item.value
             << setw(12) << fixed << setprecision(2)
             << item.valuePerWeight()
             << setw(12) << item.priority
             << setw(15)
             << (item.divisible ? "Divisible" : "Indivisible")
             << endl;
    }

    // Variables for final result
    double totalValue = 0.0;
    totalWeightCarried = 0.0;

    cout << "\n==============================================================\n";
    cout << "                 SELECTED RELIEF ITEMS\n";
    cout << "==============================================================\n";

    // Step 2: Greedily select items
    for (const auto& item : items) {

        if (capacity <= 0)
            break;

        // Case 1: Complete item fits
        if (item.weight <= capacity) {

            totalValue += item.value;
            capacity -= item.weight;
            totalWeightCarried += item.weight;

            cout << "\n"
                 << item.name
                 << " -> FULL\n";

            cout << "Weight Taken : " << item.weight << " kg\n";
            cout << "Utility      : " << item.value << " units\n";
            cout << "Ratio        : "
                 << fixed << setprecision(2)
                 << item.valuePerWeight() << "\n";
            cout << "Priority     : " << item.priority << "\n";
        }

        // Case 2: Item does not completely fit
        else {

            // Take fraction only if item is divisible
            if (item.divisible) {

                double takenWeight = capacity;

                double fraction = takenWeight / item.weight;

                double takenValue =
                    fraction * item.value;

                totalValue += takenValue;
                totalWeightCarried += takenWeight;

                capacity = 0;

                cout << "\n"
                     << item.name
                     << " -> PARTIAL\n";

                cout << "Weight Taken : "
                     << fixed << setprecision(2)
                     << takenWeight << " kg\n";

                cout << "Fraction     : "
                     << fixed << setprecision(2)
                     << fraction * 100 << "%\n";

                cout << "Utility      : "
                     << fixed << setprecision(2)
                     << takenValue << " units\n";

                cout << "Ratio        : "
                     << fixed << setprecision(2)
                     << item.valuePerWeight() << "\n";

                cout << "Priority     : "
                     << item.priority << "\n";
            }

            // Case 3: Indivisible item does not fit
            else {

                cout << "\n"
                     << item.name
                     << " -> SKIPPED\n";

                cout << "Reason: Indivisible item does not fit.\n";
            }
        }
    }

    return totalValue;
}

int main() {

    int n;

    cout << "==============================================================\n";
    cout << "       EMERGENCY RELIEF SUPPLY DISTRIBUTION\n";
    cout << "              FRACTIONAL KNAPSACK\n";
    cout << "==============================================================\n";

    // Input number of items
    cout << "\nEnter number of relief items: ";
    cin >> n;

    vector<Item> items;

    // Input item details
    for (int i = 0; i < n; i++) {

        string name;
        double weight;
        double value;
        int divisibleInput;
        int priority;

        cout << "\n--------------------------------------------------------------\n";
        cout << "Item #" << i + 1 << "\n";
        cout << "--------------------------------------------------------------\n";

        cin.ignore();

        cout << "Enter item name: ";
        getline(cin, name);

        cout << "Enter weight (kg): ";
        cin >> weight;

        cout << "Enter utility value: ";
        cin >> value;

        cout << "Is item divisible? (1 = Yes, 0 = No): ";
        cin >> divisibleInput;

        cout << "Enter priority (1 = High, 2 = Medium, 3 = Low): ";
        cin >> priority;

        items.emplace_back(
            name,
            weight,
            value,
            divisibleInput == 1,
            priority
        );
    }

    // Input boat capacity
    double capacity;

    cout << "\nEnter maximum boat capacity (kg): ";
    cin >> capacity;

    // Calculate maximum utility
    double totalWeightCarried;

    double maximumUtility =
        fractionalKnapsack(
            items,
            capacity,
            totalWeightCarried
        );

    // Final report
    cout << "\n==============================================================\n";
    cout << "                     FINAL REPORT\n";
    cout << "==============================================================\n";

    cout << "Total Weight Carried : "
         << fixed << setprecision(2)
         << totalWeightCarried
         << " kg\n";

    cout << "Maximum Utility      : "
         << fixed << setprecision(2)
         << maximumUtility
         << " units\n";

    cout << "==============================================================\n";

    cout << "\nAlgorithm: Fractional Knapsack using Greedy Method\n";
    cout << "Time Complexity: O(n log n)\n";
    cout << "Space Complexity: O(n)\n";

    cout << "\n==============================================================\n";

    return 0;
}
