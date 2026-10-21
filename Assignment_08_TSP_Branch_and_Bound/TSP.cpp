#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <vector>

using namespace std;

using Cost = long long;
using Matrix = vector<vector<Cost>>;

const Cost INF = numeric_limits<Cost>::max() / 4;

struct Node {
    Matrix reducedMatrix;
    vector<int> path;
    vector<bool> visited;
    int currentCity;
    Cost lowerBound;
};

struct CompareByBound {
    bool operator()(const Node& a, const Node& b) const {
        return a.lowerBound > b.lowerBound;
    }
};

// Reduce rows and columns and return the reduction cost.
Cost reduceMatrix(Matrix& matrix) {
    int n = static_cast<int>(matrix.size());
    Cost reductionCost = 0;

    for (int row = 0; row < n; ++row) {
        Cost minimum = INF;
        for (int column = 0; column < n; ++column) {
            minimum = min(minimum, matrix[row][column]);
        }

        if (minimum != INF && minimum > 0) {
            reductionCost += minimum;
            for (int column = 0; column < n; ++column) {
                if (matrix[row][column] != INF) {
                    matrix[row][column] -= minimum;
                }
            }
        }
    }

    for (int column = 0; column < n; ++column) {
        Cost minimum = INF;
        for (int row = 0; row < n; ++row) {
            minimum = min(minimum, matrix[row][column]);
        }

        if (minimum != INF && minimum > 0) {
            reductionCost += minimum;
            for (int row = 0; row < n; ++row) {
                if (matrix[row][column] != INF) {
                    matrix[row][column] -= minimum;
                }
            }
        }
    }

    return reductionCost;
}

// Calculate the real cost of the completed tour.
Cost calculateTourCost(const Matrix& costs, const vector<int>& path) {
    Cost total = 0;

    for (size_t i = 0; i + 1 < path.size(); ++i) {
        Cost edgeCost = costs[path[i]][path[i + 1]];
        if (edgeCost == INF) {
            return INF;
        }
        total += edgeCost;
    }

    Cost returnCost = costs[path.back()][path.front()];
    if (returnCost == INF) {
        return INF;
    }

    return total + returnCost;
}

// Find the minimum-cost tour using Least Cost Branch and Bound.
bool solveTSP(const Matrix& costs, int startCity,
              vector<int>& bestPath, Cost& bestCost) {
    int n = static_cast<int>(costs.size());

    if (n == 0 || startCity < 0 || startCity >= n) {
        return false;
    }

    if (n == 1) {
        bestPath = {startCity, startCity};
        bestCost = 0;
        return true;
    }

    Matrix initialMatrix = costs;
    for (int i = 0; i < n; ++i) {
        initialMatrix[i][i] = INF;
    }

    Cost initialBound = reduceMatrix(initialMatrix);

    Node root;
    root.reducedMatrix = move(initialMatrix);
    root.path = {startCity};
    root.visited.assign(n, false);
    root.visited[startCity] = true;
    root.currentCity = startCity;
    root.lowerBound = initialBound;

    priority_queue<Node, vector<Node>, CompareByBound> liveNodes;
    liveNodes.push(move(root));

    bestCost = INF;
    bestPath.clear();

    while (!liveNodes.empty()) {
        Node current = liveNodes.top();
        liveNodes.pop();

        if (current.lowerBound >= bestCost) {
            continue;
        }

        if (static_cast<int>(current.path.size()) == n) {
            Cost tourCost = calculateTourCost(costs, current.path);
            if (tourCost < bestCost) {
                bestCost = tourCost;
                bestPath = current.path;
                bestPath.push_back(startCity);
            }
            continue;
        }

        int from = current.currentCity;

        for (int nextCity = 0; nextCity < n; ++nextCity) {
            if (current.visited[nextCity] ||
                current.reducedMatrix[from][nextCity] == INF) {
                continue;
            }

            Node child = current;
            Cost selectedEdgeBound = child.reducedMatrix[from][nextCity];
            child.lowerBound += selectedEdgeBound;

            // Do not leave the current city again.
            for (int column = 0; column < n; ++column) {
                child.reducedMatrix[from][column] = INF;
            }

            // Do not enter the next city a second time.
            for (int row = 0; row < n; ++row) {
                child.reducedMatrix[row][nextCity] = INF;
            }

            // Do not return to the start until every city is visited.
            if (static_cast<int>(child.path.size()) + 1 < n) {
                child.reducedMatrix[nextCity][startCity] = INF;
            }

            child.lowerBound += reduceMatrix(child.reducedMatrix);
            child.path.push_back(nextCity);
            child.visited[nextCity] = true;
            child.currentCity = nextCity;

            if (child.lowerBound < bestCost) {
                liveNodes.push(move(child));
            }
        }
    }

    return bestCost != INF;
}

int main() {
    int n;
    cout << "Enter number of cities: ";
    cin >> n;

    if (n <= 0) {
        cout << "Number of cities must be positive.\n";
        return 1;
    }

    Matrix costs(n, vector<Cost>(n));

    cout << "Enter the cost matrix (-1 for no direct route):\n";
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            Cost value;
            cin >> value;

            if (i == j || value == -1) {
                costs[i][j] = INF;
            } else if (value < -1) {
                cout << "Costs must be nonnegative, or -1 for no route.\n";
                return 1;
            } else {
                costs[i][j] = value;
            }
        }
    }

    vector<int> bestPath;
    Cost bestCost;

    if (!solveTSP(costs, 0, bestPath, bestCost)) {
        cout << "No complete tour exists.\n";
        return 0;
    }

    cout << "\nMinimum tour cost: " << bestCost << '\n';
    cout << "Optimal route: ";

    for (size_t i = 0; i < bestPath.size(); ++i) {
        cout << "City " << bestPath[i] + 1;
        if (i + 1 < bestPath.size()) {
            cout << " -> ";
        }
    }
    cout << '\n';

    return 0;
}
