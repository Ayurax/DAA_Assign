#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

using Graph = vector<set<int>>;

struct Course {
    string name;
    int students;
};

struct Room {
    string name;
    int capacity;
};

struct Schedule {
    vector<int> color;
    int slotCount;
};

// Create an edge between every pair of subjects taken by one student.
Graph buildConflictGraph(int courseCount,
                         const vector<vector<int>>& studentCourses) {
    Graph graph(courseCount);

    for (const auto& enrolled : studentCourses) {
        for (size_t i = 0; i < enrolled.size(); ++i) {
            for (size_t j = i + 1; j < enrolled.size(); ++j) {
                int a = enrolled[i];
                int b = enrolled[j];

                if (a != b) {
                    graph[a].insert(b);
                    graph[b].insert(a);
                }
            }
        }
    }

    return graph;
}

// Return the first slot not already used by a neighboring subject.
int smallestAvailableColor(int vertex, const Graph& graph,
                           const vector<int>& colors) {
    set<int> used;
    for (int neighbor : graph[vertex]) {
        if (colors[neighbor] != -1) {
            used.insert(colors[neighbor]);
        }
    }

    int color = 0;
    while (used.count(color)) {
        ++color;
    }
    return color;
}

// Greedy: process the courses in the supplied order.
Schedule greedyColoring(const Graph& graph, vector<int> order) {
    int n = static_cast<int>(graph.size());
    vector<int> colors(n, -1);

    for (int vertex : order) {
        colors[vertex] = smallestAvailableColor(vertex, graph, colors);
    }

    int slotCount = 0;
    for (int color : colors) {
        slotCount = max(slotCount, color + 1);
    }

    return {colors, slotCount};
}

// Welsh-Powell: process the most connected courses first.
Schedule welshPowellColoring(const Graph& graph) {
    int n = static_cast<int>(graph.size());
    vector<int> order(n);
    for (int i = 0; i < n; ++i) {
        order[i] = i;
    }

    sort(order.begin(), order.end(), [&](int a, int b) {
        if (graph[a].size() != graph[b].size()) {
            return graph[a].size() > graph[b].size();
        }
        return a < b;
    });

    return greedyColoring(graph, order);
}

// DSATUR chooses the uncolored course with the most neighboring slot colors.
Schedule dsaturColoring(const Graph& graph) {
    int n = static_cast<int>(graph.size());
    vector<int> colors(n, -1);
    vector<bool> colored(n, false);
    int coloredCount = 0;
    int maxColor = -1;

    while (coloredCount < n) {
        int chosen = -1;
        size_t bestSaturation = 0;
        size_t bestDegree = 0;

        for (int vertex = 0; vertex < n; ++vertex) {
            if (colored[vertex]) {
                continue;
            }

            set<int> neighboringColors;
            for (int neighbor : graph[vertex]) {
                if (colors[neighbor] != -1) {
                    neighboringColors.insert(colors[neighbor]);
                }
            }

            size_t saturation = neighboringColors.size();
            size_t degree = graph[vertex].size();

            if (chosen == -1 || saturation > bestSaturation ||
                (saturation == bestSaturation && degree > bestDegree)) {
                chosen = vertex;
                bestSaturation = saturation;
                bestDegree = degree;
            }
        }

        colors[chosen] = smallestAvailableColor(chosen, graph, colors);
        colored[chosen] = true;
        ++coloredCount;
        maxColor = max(maxColor, colors[chosen]);
    }

    return {colors, maxColor + 1};
}

// Assign a room large enough for each exam, without reusing rooms in a slot.
bool assignRooms(const vector<Course>& courses, const vector<Room>& rooms,
                 const vector<int>& colors, vector<string>& assignedRooms) {
    int slotCount = 0;
    for (int color : colors) {
        slotCount = max(slotCount, color + 1);
    }

    assignedRooms.assign(courses.size(), "");
    vector<bool> roomUsed(rooms.size(), false);

    for (int slot = 0; slot < slotCount; ++slot) {
        fill(roomUsed.begin(), roomUsed.end(), false);

        vector<int> exams;
        for (int course = 0; course < static_cast<int>(courses.size()); ++course) {
            if (colors[course] == slot) {
                exams.push_back(course);
            }
        }

        sort(exams.begin(), exams.end(), [&](int a, int b) {
            return courses[a].students > courses[b].students;
        });

        for (int course : exams) {
            int bestRoom = -1;

            for (int room = 0; room < static_cast<int>(rooms.size()); ++room) {
                if (roomUsed[room] ||
                    rooms[room].capacity < courses[course].students) {
                    continue;
                }

                if (bestRoom == -1 ||
                    rooms[room].capacity < rooms[bestRoom].capacity) {
                    bestRoom = room;
                }
            }

            if (bestRoom == -1) {
                return false;
            }

            roomUsed[bestRoom] = true;
            assignedRooms[course] = rooms[bestRoom].name;
        }
    }

    return true;
}

void printSchedule(const string& algorithm, const Schedule& schedule,
                   const vector<Course>& courses, const vector<Room>& rooms) {
    vector<string> assignedRooms;
    bool roomsFit = assignRooms(courses, rooms, schedule.color, assignedRooms);

    cout << "\n" << algorithm << " - Exam slots: "
         << schedule.slotCount << '\n';
    cout << "Course\tSlot\tRoom\n";

    for (int i = 0; i < static_cast<int>(courses.size()); ++i) {
        cout << courses[i].name << '\t'
             << "Slot " << schedule.color[i] + 1 << '\t'
             << (roomsFit ? assignedRooms[i] : "No room available") << '\n';
    }

    if (!roomsFit) {
        cout << "Room assignment is infeasible with the available "
                "room capacities and slot allocation.\n";
    }
}

int main() {
    // Sample university courses and their enrollment counts.
    vector<Course> courses = {
        {"DAA", 90},
        {"DBMS", 75},
        {"OS", 45},
        {"CN", 40}
    };

    // Example student enrollments create conflicts between DAA, DBMS and OS.
    vector<vector<int>> studentCourses = {
        {0, 1},
        {1, 2},
        {0, 2},
        {3}
    };

    vector<Room> rooms = {
        {"Room-A", 100},
        {"Room-B", 80},
        {"Room-C", 50}
    };

    Graph graph = buildConflictGraph(
        static_cast<int>(courses.size()), studentCourses
    );

    Schedule greedy = greedyColoring(graph, {0, 1, 2, 3});
    Schedule dsatur = dsaturColoring(graph);
    Schedule welshPowell = welshPowellColoring(graph);

    cout << "UNIVERSITY EXAM TIMETABLE\n";
    cout << "Graph Coloring with Room Allocation\n";
    printSchedule("Greedy Coloring", greedy, courses, rooms);
    printSchedule("DSATUR", dsatur, courses, rooms);
    printSchedule("Welsh-Powell", welshPowell, courses, rooms);

    return 0;
}
