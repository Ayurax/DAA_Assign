#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
using namespace std;

struct Edge {
    int to;
    double baseCost;
    double cost;
};

const double INF = numeric_limits<double>::max() / 4;

void findBestCosts(const vector<vector<Edge>>& graph,
                   const vector<int>& stageStart,
                   const vector<int>& stageCount,
                   vector<double>& bestCost,
                   vector<int>& nextNode) {
    int n = static_cast<int>(graph.size());
    int lastStage = static_cast<int>(stageCount.size()) - 1;

    bestCost.assign(n, INF);
    nextNode.assign(n, -1);

    // Any node in the last stage is a destination, so its remaining cost is 0.
    for (int i = 0; i < stageCount[lastStage]; ++i) {
        bestCost[stageStart[lastStage] + i] = 0;
    }

    // Calculate minimum cost backwards, from the last stage to the first.
    for (int stage = lastStage - 1; stage >= 0; --stage) {
        for (int i = 0; i < stageCount[stage]; ++i) {
            int u = stageStart[stage] + i;
            for (const Edge& edge : graph[u]) {
                if (bestCost[edge.to] == INF) continue;
                double candidate = edge.cost + bestCost[edge.to];
                if (candidate < bestCost[u]) {
                    bestCost[u] = candidate;
                    nextNode[u] = edge.to;
                }
            }
        }
    }
}

void printStageZeroCosts(const vector<double>& bestCost,
                         int start, int count) {
    cout << fixed << setprecision(2);
    for (int i = 0; i < count; ++i) {
        int node = start + i;
        if (bestCost[node] == INF)
            cout << "Node " << node << ": unreachable\n";
        else
            cout << "Node " << node << ": cost = " << bestCost[node] << '\n';
    }
}

int main() {
    int stages;
    cout << "Enter number of stages: ";
    if (!(cin >> stages) || stages < 2) {
        cerr << "Please enter at least 2 stages.\n";
        return 1;
    }

    vector<int> stageCount(stages), stageStart(stages);
    int nodes = 0;
    cout << "Enter number of nodes in each stage (" << stages << " values): ";
    for (int i = 0; i < stages; ++i) {
        cin >> stageCount[i];
        if (stageCount[i] < 1) {
            cerr << "Each stage must contain at least one node.\n";
            return 1;
        }
        stageStart[i] = nodes;
        nodes += stageCount[i];
    }

    vector<int> nodeStage(nodes);
    for (int stage = 0; stage < stages; ++stage) {
        for (int i = 0; i < stageCount[stage]; ++i)
            nodeStage[stageStart[stage] + i] = stage;
    }

    int edgeCount;
    cout << "Enter number of edges: ";
    cin >> edgeCount;
    if (edgeCount < 0) {
        cerr << "Number of edges cannot be negative.\n";
        return 1;
    }

    vector<vector<Edge>> graph(nodes);
    cout << "Enter each edge as: u v cost\n";
    for (int i = 0; i < edgeCount; ++i) {
        int u, v;
        double cost;
        cin >> u >> v >> cost;
        if (u < 0 || u >= nodes || v < 0 || v >= nodes ||
            nodeStage[v] <= nodeStage[u] || cost < 0) {
            cerr << "Invalid edge. Edges must go to a later stage and have non-negative cost.\n";
            return 1;
        }
        graph[u].push_back({v, cost, cost});
    }

    vector<double> bestCost;
    vector<int> nextNode;
    findBestCosts(graph, stageStart, stageCount, bestCost, nextNode);

    cout << "\nBest costs from Stage-0 nodes:\n";
    printStageZeroCosts(bestCost, stageStart[0], stageCount[0]);

    int source;
    cout << "\nEnter a source node id in Stage 0 (or -1 to skip): ";
    cin >> source;
    if (source != -1) {
        if (source < stageStart[0] || source >= stageStart[0] + stageCount[0]) {
            cerr << "Source must be a node in Stage 0.\n";
            return 1;
        }
        if (bestCost[source] == INF) {
            cout << "No route from " << source << "\n";
        } else {
            cout << "Path from " << source << ": ";
            int current = source;
            while (current != -1) {
                cout << current;
                current = nextNode[current];
                if (current != -1) cout << " -> ";
            }
            cout << "\nTotal route cost: " << bestCost[source] << '\n';
        }
    }

    int updates;
    cout << "\nEnter number of live edge updates (0 to finish): ";
    cin >> updates;
    for (int i = 0; i < updates; ++i) {
        int u, v;
        double multiplier;
        cout << "Enter edge update (u v multiplier): ";
        cin >> u >> v >> multiplier;
        if (u < 0 || u >= nodes || v < 0 || v >= nodes || multiplier < 0) {
            cerr << "Invalid update.\n";
            return 1;
        }

        bool found = false;
        for (Edge& edge : graph[u]) {
            if (edge.to == v) {
                edge.cost = edge.baseCost * multiplier;
                found = true;
            }
        }
        if (!found) {
            cout << "Edge " << u << " -> " << v << " was not found; update skipped.\n";
            continue;
        }

        // Recalculate the DP table after the traffic/cost change.
        findBestCosts(graph, stageStart, stageCount, bestCost, nextNode);
    }

    if (updates > 0) {
        cout << "\nAfter updates, best costs from Stage-0 nodes:\n";
        printStageZeroCosts(bestCost, stageStart[0], stageCount[0]);
    }

    return 0;
}
