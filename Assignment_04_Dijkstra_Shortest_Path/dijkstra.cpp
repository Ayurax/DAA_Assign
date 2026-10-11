#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
using namespace std;

struct Road {
    int to;
    int time;
};

struct Route {
    int time;
    vector<int> path;
};

void addRoad(vector<vector<Road>>& graph, int u, int v, int time) {
    graph[u].push_back({v, time});
    graph[v].push_back({u, time});
}

void updateRoad(vector<vector<Road>>& graph, int u, int v, int newTime) {
    for (auto& road : graph[u])
        if (road.to == v) road.time = newTime;
    for (auto& road : graph[v])
        if (road.to == u) road.time = newTime;
}

Route dijkstra(const vector<vector<Road>>& graph, int source,
               const vector<int>& hospitals) {
    const int n = static_cast<int>(graph.size());
    const int INF = numeric_limits<int>::max();
    vector<int> dist(n, INF), parent(n, -1);
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;

    dist[source] = 0;
    pq.push({0, source});

    while (!pq.empty()) {
        auto [time, u] = pq.top();
        pq.pop();
        if (time != dist[u]) continue;

        if (find(hospitals.begin(), hospitals.end(), u) != hospitals.end()) {
            vector<int> path;
            for (int cur = u; cur != -1; cur = parent[cur])
                path.push_back(cur);
            reverse(path.begin(), path.end());
            return {dist[u], path};
        }

        for (const auto& road : graph[u]) {
            if (dist[u] != INF && dist[road.to] > dist[u] + road.time) {
                dist[road.to] = dist[u] + road.time;
                parent[road.to] = u;
                pq.push({dist[road.to], road.to});
            }
        }
    }
    return {-1, {}};
}

void showRoute(const Route& route) {
    if (route.time < 0) {
        cout << "No hospital is reachable from this location.\n";
        return;
    }
    cout << "Route: ";
    for (size_t i = 0; i < route.path.size(); ++i) {
        if (i) cout << " -> ";
        cout << route.path[i];
    }
    cout << "\nTravel time: " << route.time << " minutes\n";
    cout << "Nearest hospital: " << route.path.back() << "\n";
}

int main() {
    // Sample road network: nodes are intersections; weights are minutes.
    const int n = 6;
    vector<vector<Road>> graph(n);
    addRoad(graph, 0, 1, 4);
    addRoad(graph, 0, 2, 2);
    addRoad(graph, 1, 2, 1);
    addRoad(graph, 1, 3, 5);
    addRoad(graph, 2, 3, 8);
    addRoad(graph, 2, 4, 10);
    addRoad(graph, 3, 4, 2);
    addRoad(graph, 3, 5, 6);
    addRoad(graph, 4, 5, 3);

    const int ambulance = 0;
    const vector<int> hospitals = {4, 5};

    cout << "SMART CITY EMERGENCY ROUTE SYSTEM\n";
    cout << "Intersections: 0 to 5; ambulance starts at node 0\n";
    cout << "Hospitals are located at nodes 4 and 5.\n\n";

    cout << "Before traffic update:\n";
    showRoute(dijkstra(graph, ambulance, hospitals));

    cout << "\nTraffic update: road 2 <-> 1 changes from 1 to 12 minutes.\n";
    updateRoad(graph, 1, 2, 12);

    cout << "\nAfter traffic update:\n";
    showRoute(dijkstra(graph, ambulance, hospitals));

    cout << "\nAlgorithm: Dijkstra's shortest path\n";
    cout << "Time complexity: O((V + E) log V) with a priority queue\n";
    return 0;
}
