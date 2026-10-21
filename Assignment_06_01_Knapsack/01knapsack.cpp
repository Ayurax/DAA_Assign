#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
using namespace std;

// 1. Recursive approach (brute force): O(2^N)
int recursiveKnapsack(const vector<int>& wt, const vector<int>& val,
                      int capacity, int n) {
    if (n == 0 || capacity == 0)
        return 0;

    if (wt[n - 1] <= capacity) {
        int include = val[n - 1] +
            recursiveKnapsack(wt, val, capacity - wt[n - 1], n - 1);
        int exclude = recursiveKnapsack(wt, val, capacity, n - 1);
        return max(include, exclude);
    }

    return recursiveKnapsack(wt, val, capacity, n - 1);
}

// 2. Memoization approach: O(N * W) time and space
int memoizedKnapsack(const vector<int>& wt, const vector<int>& val,
                     int capacity, int n, vector<vector<int>>& dp) {
    if (n == 0 || capacity == 0)
        return 0;

    if (dp[n][capacity] != -1)
        return dp[n][capacity];

    if (wt[n - 1] <= capacity) {
        int include = val[n - 1] +
            memoizedKnapsack(wt, val, capacity - wt[n - 1], n - 1, dp);
        int exclude = memoizedKnapsack(wt, val, capacity, n - 1, dp);
        dp[n][capacity] = max(include, exclude);
    } else {
        dp[n][capacity] = memoizedKnapsack(wt, val, capacity, n - 1, dp);
    }

    return dp[n][capacity];
}

// 3. Tabulation approach: build the DP table from smaller subproblems
int tabulationKnapsack(const vector<int>& wt, const vector<int>& val,
                       int capacity, const vector<string>& names) {
    int n = static_cast<int>(wt.size());
    vector<vector<int>> dp(n + 1, vector<int>(capacity + 1, 0));

    for (int i = 1; i <= n; i++) {
        for (int w = 0; w <= capacity; w++) {
            dp[i][w] = dp[i - 1][w];
            if (wt[i - 1] <= w) {
                int include = val[i - 1] + dp[i - 1][w - wt[i - 1]];
                dp[i][w] = max(dp[i][w], include);
            }
        }
    }

    cout << "\nDynamic Programming Table:\n";
    cout << setw(8) << "Items\\W";
    for (int w = 0; w <= capacity; w++)
        cout << setw(6) << w;
    cout << '\n';

    for (int i = 0; i <= n; i++) {
        cout << setw(8) << i;
        for (int w = 0; w <= capacity; w++)
            cout << setw(6) << dp[i][w];
        cout << '\n';
    }

    // Trace the table backwards to find which items were selected.
    vector<int> selected;
    int remaining = capacity;
    for (int i = n; i > 0; i--) {
        if (dp[i][remaining] != dp[i - 1][remaining]) {
            selected.push_back(i - 1);
            remaining -= wt[i - 1];
        }
    }
    reverse(selected.begin(), selected.end());

    cout << "\nSelected Relief Items:\n";
    int totalWeight = 0;
    if (selected.empty()) {
        cout << "No items selected.\n";
    }
    for (int index : selected) {
        cout << names[index] << " | Weight: " << wt[index]
             << " kg | Utility: " << val[index] << '\n';
        totalWeight += wt[index];
    }

    cout << "\nMaximum Total Utility: " << dp[n][capacity] << '\n';
    cout << "Total Weight: " << totalWeight << " kg\n";
    cout << "Remaining Capacity: " << capacity - totalWeight << " kg\n";
    return dp[n][capacity];
}

int main() {
    int n, capacity;

    cout << "========================================\n";
    cout << " DISASTER RELIEF RESOURCE ALLOCATION\n";
    cout << "========================================\n";
    cout << "Enter number of relief items: ";
    cin >> n;

    if (!cin || n <= 0) {
        cout << "Error: Number of items must be positive.\n";
        return 1;
    }

    vector<string> names(n);
    vector<int> wt(n), val(n);
    cout << "\nEnter item name, weight (kg), and utility.\n";
    cout << "Use names without spaces, e.g., FirstAidKit.\n";

    for (int i = 0; i < n; i++) {
        cout << "\nItem " << i + 1 << " name: ";
        cin >> names[i];
        cout << "Weight (kg): ";
        cin >> wt[i];
        cout << "Utility value: ";
        cin >> val[i];

        if (!cin || wt[i] <= 0 || val[i] < 0) {
            cout << "Error: Weight must be positive and utility must be non-negative.\n";
            return 1;
        }
    }

    cout << "\nEnter truck capacity (kg): ";
    cin >> capacity;
    if (!cin || capacity < 0) {
        cout << "Error: Capacity cannot be negative.\n";
        return 1;
    }

    cout << "\n--- Recursive Approach ---\n";
    cout << "Maximum Utility: "
         << recursiveKnapsack(wt, val, capacity, n) << '\n';

    vector<vector<int>> memo(n + 1, vector<int>(capacity + 1, -1));
    cout << "\n--- Memoization Approach ---\n";
    cout << "Maximum Utility: "
         << memoizedKnapsack(wt, val, capacity, n, memo) << '\n';

    cout << "\n--- Tabulation Approach ---\n";
    int result = tabulationKnapsack(wt, val, capacity, names);

    cout << "\n========== COMPLEXITY ANALYSIS ==========\n";
    cout << "Recursive Time: O(2^N)\n";
    cout << "Memoization Time: O(N * W)\n";
    cout << "Tabulation Time: O(N * W)\n";
    cout << "DP Space Complexity: O(N * W)\n";
    cout << "\nFinal Optimal Utility: " << result << '\n';
    return 0;
}
