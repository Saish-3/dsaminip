#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <limits>

using namespace std;

// ═══════════════════════════════════════════════════════════════
// Railway Network Management System - Pure C++
// Features: Dijkstra's, Prim's MST, Priority Queue Waitlist,
//           Dynamic Pricing, Cascading Cancellation, Multi-level Sorting
// ═══════════════════════════════════════════════════════════════

const int MAXN = 20, MAXB = 500;
const double INF = 1e9;

// ──────────────────────────────────────
// Data Structures
// ──────────────────────────────────────

struct Station {
    int id;
    string name;
};

struct Route {
    int from, to;
    int distance, capacity, booked;
    double baseFare;
    
    double getCurrentFare() const {
        if (capacity == 0) return baseFare;
        double occupancy = (double)booked / capacity;
        if (occupancy > 0.8) {
            return baseFare * 1.5;  // 50% surge when >80% full
        }
        return baseFare;
    }
};

struct Booking {
    int pnr;
    string passenger;
    int source, dest;
    string date;
    bool isVIP;
    double fare;
    bool active;
};

struct WaitlistEntry {
    string passenger;
    int source, dest;
    string date;
    bool isVIP;
    int timestamp;
};

struct Refund {
    int pnr;
    string passenger;
    double amount;
};

// ──────────────────────────────────────
// Global Data
// ──────────────────────────────────────

vector<Station> stations;
vector<vector<Route>> adjList;
vector<Booking> bookings;
priority_queue<WaitlistEntry, vector<WaitlistEntry>, 
               function<bool(const WaitlistEntry&, const WaitlistEntry&)>> waitlist(
    [](const WaitlistEntry& a, const WaitlistEntry& b) {
        if (a.isVIP != b.isVIP) return !a.isVIP;  // VIP first
        return a.timestamp > b.timestamp;           // FIFO for same priority
    });

stack<Refund> refundStack;
int nextPNR = 1001;
int globalTimestamp = 0;

// ──────────────────────────────────────
// Station Management
// ──────────────────────────────────────

int addStation(const string& name) {
    int id = stations.size();
    stations.push_back({id, name});
    adjList.push_back(vector<Route>());
    return id;
}

int getStationId(const string& name) {
    for (const auto& s : stations) {
        if (s.name == name) return s.id;
    }
    return -1;
}

string getStationName(int id) {
    if (id >= 0 && id < (int)stations.size()) {
        return stations[id].name;
    }
    return "Unknown";
}

// ──────────────────────────────────────
// Route Management (Graph)
// ──────────────────────────────────────

void addRoute(int from, int to, int distance, double fare, int capacity) {
    if (from >= (int)adjList.size() || to >= (int)adjList.size()) return;
    
    adjList[from].push_back({from, to, distance, capacity, 0, fare});
    adjList[to].push_back({to, from, distance, capacity, 0, fare});  // Bidirectional
}

Route* findRoute(int from, int to) {
    if (from >= (int)adjList.size() || to >= (int)adjList.size()) return nullptr;
    for (auto& r : adjList[from]) {
        if (r.to == to) return &r;
    }
    return nullptr;
}

// ──────────────────────────────────────
// Dijkstra's Algorithm (Shortest Path)
// ──────────────────────────────────────

vector<int> dijkstra(int start) {
    vector<int> dist(stations.size(), (int)INF);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    
    dist[start] = 0;
    pq.push({0, start});
    
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        
        if (d > dist[u]) continue;
        
        for (const auto& edge : adjList[u]) {
            int v = edge.to;
            int newDist = dist[u] + edge.distance;
            
            if (newDist < dist[v]) {
                dist[v] = newDist;
                pq.push({newDist, v});
            }
        }
    }
    
    return dist;
}

// ──────────────────────────────────────
// Prim's Algorithm (Minimum Spanning Tree)
// ──────────────────────────────────────

int primMST() {
    vector<bool> visited(stations.size(), false);
    priority_queue<pair<int, pair<int, int>>, 
                   vector<pair<int, pair<int, int>>>,
                   greater<pair<int, pair<int, int>>>> pq;
    
    int totalCost = 0;
    visited[0] = true;
    
    for (const auto& edge : adjList[0]) {
        pq.push({edge.distance, {0, edge.to}});
    }
    
    int edgeCount = 0;
    while (!pq.empty() && edgeCount < (int)stations.size() - 1) {
        auto [cost, nodes] = pq.top();
        pq.pop();
        
        int u = nodes.first, v = nodes.second;
        if (visited[v]) continue;
        
        visited[v] = true;
        totalCost += cost;
        edgeCount++;
        
        for (const auto& edge : adjList[v]) {
            if (!visited[edge.to]) {
                pq.push({edge.distance, {v, edge.to}});
            }
        }
    }
    
    return totalCost;
}

// ──────────────────────────────────────
// Booking Management
// ──────────────────────────────────────

int bookSeat(const string& passenger, int src, int dst, bool isVIP, const string& date) {
    Route* route = findRoute(src, dst);
    if (!route) {
        cout << "❌ No direct route found!\n";
        return -1;
    }
    
    if (route->booked < route->capacity) {
        double fare = route->getCurrentFare();
        int pnr = nextPNR++;
        bookings.push_back({pnr, passenger, src, dst, date, isVIP, fare, true});
        route->booked++;
        
        // Update reverse route
        Route* revRoute = findRoute(dst, src);
        if (revRoute) revRoute->booked++;
        
        cout << "✓ Booking successful!\n"
             << "  PNR: " << pnr << "\n"
             << "  Passenger: " << passenger << "\n"
             << "  Route: " << getStationName(src) << " → " << getStationName(dst) << "\n"
             << "  Fare: ₹" << fixed << setprecision(2) << fare;
        
        if (fare > route->baseFare) {
            cout << " (SURGE PRICING)";
        }
        cout << "\n";
        return pnr;
    } else {
        WaitlistEntry entry = {passenger, src, dst, date, isVIP, globalTimestamp++};
        waitlist.push(entry);
        cout << "⏳ Seat full! Added to waitlist (" << (isVIP ? "VIP" : "Regular") << ")\n";
        return -1;
    }
}

void cancelBooking(int pnr) {
    for (auto& bk : bookings) {
        if (bk.pnr == pnr && bk.active) {
            bk.active = false;
            refundStack.push({pnr, bk.passenger, bk.fare});
            
            // Free up seat
            Route* route = findRoute(bk.source, bk.dest);
            if (route) route->booked--;
            Route* revRoute = findRoute(bk.dest, bk.source);
            if (revRoute) revRoute->booked--;
            
            cout << "✓ Booking " << pnr << " cancelled\n"
                 << "  Refund: ₹" << fixed << setprecision(2) << bk.fare << "\n";
            
            // Auto-promote from waitlist
            while (!waitlist.empty()) {
                auto entry = waitlist.top();
                waitlist.pop();
                
                if (entry.source == bk.source && entry.dest == bk.dest) {
                    cout << "↑ Auto-promoted " << entry.passenger << " from waitlist!\n";
                    bookSeat(entry.passenger, entry.source, entry.dest, 
                            entry.isVIP, entry.date);
                    return;
                }
            }
            return;
        }
    }
    cout << "❌ PNR not found\n";
}

void cascadeCancel(int src, int dst) {
    cout << "⚠ Cancelling all bookings on " << getStationName(src) 
         << " → " << getStationName(dst) << "\n";
    
    int count = 0;
    for (auto& bk : bookings) {
        if (bk.active && bk.source == src && bk.dest == dst) {
            bk.active = false;
            refundStack.push({bk.pnr, bk.passenger, bk.fare});
            count++;
        }
    }
    
    Route* route = findRoute(src, dst);
    if (route) route->booked = 0;
    
    cout << "Cancelled " << count << " bookings\n";
}

// ──────────────────────────────────────
// Multi-level Sorting & Binary Search
// ──────────────────────────────────────

struct BookingSortKey {
    int route;
    string date;
    double fare;
    
    bool operator<(const BookingSortKey& other) const {
        if (route != other.route) return route < other.route;
        if (date != other.date) return date < other.date;
        return fare < other.fare;
    }
};

void generateSortedReport() {
    vector<BookingSortKey> sortedBookings;
    
    for (const auto& bk : bookings) {
        if (bk.active) {
            sortedBookings.push_back({
                (int)(bk.source * stations.size() + bk.dest),
                bk.date,
                bk.fare
            });
        }
    }
    
    sort(sortedBookings.begin(), sortedBookings.end());
    
    cout << "\n═══ Sorted Bookings (Route → Date → Fare) ═══\n";
    for (const auto& sb : sortedBookings) {
        cout << "Route " << sb.route << " | Date: " << sb.date 
             << " | Fare: ₹" << fixed << setprecision(2) << sb.fare << "\n";
    }
}

// Binary search on sorted fare ranges
int binarySearchFare(double targetFare) {
    vector<double> fares;
    for (const auto& bk : bookings) {
        if (bk.active) fares.push_back(bk.fare);
    }
    sort(fares.begin(), fares.end());
    
    auto it = lower_bound(fares.begin(), fares.end(), targetFare);
    if (it != fares.end()) {
        return it - fares.begin();
    }
    return -1;
}

// ──────────────────────────────────────
// Reports & Display
// ──────────────────────────────────────

void displayActiveBookings() {
    cout << "\n═══ Active Bookings ═══\n";
    if (bookings.empty()) {
        cout << "No bookings yet\n";
        return;
    }
    
    int count = 0;
    for (const auto& bk : bookings) {
        if (bk.active) {
            cout << "PNR: " << bk.pnr << " | " << bk.passenger << " | "
                 << getStationName(bk.source) << " → " << getStationName(bk.dest)
                 << " | Date: " << bk.date << " | Fare: ₹" << fixed << setprecision(2) 
                 << bk.fare << " | " << (bk.isVIP ? "VIP" : "Regular") << "\n";
            count++;
        }
    }
    cout << "Total: " << count << " active bookings\n";
}

void displayRouteStatus() {
    cout << "\n═══ Route Status & Occupancy ═══\n";
    for (int i = 0; i < (int)stations.size(); i++) {
        for (const auto& route : adjList[i]) {
            if (i < route.to) {  // Avoid printing each route twice
                double occupancy = (double)route.booked / route.capacity * 100;
                cout << getStationName(route.from) << " ↔ " << getStationName(route.to)
                     << " | Distance: " << route.distance << "km"
                     << " | Occupancy: " << fixed << setprecision(1) << occupancy << "%"
                     << " (" << route.booked << "/" << route.capacity << ")"
                     << " | Fare: ₹" << setprecision(2) << route.getCurrentFare() << "\n";
            }
        }
    }
}

void displayRefundHistory() {
    cout << "\n═══ Refund History (Stack - LIFO) ═══\n";
    if (refundStack.empty()) {
        cout << "No refunds\n";
        return;
    }
    
    stack<Refund> temp = refundStack;
    while (!temp.empty()) {
        auto refund = temp.top();
        temp.pop();
        cout << "PNR: " << refund.pnr << " | " << refund.passenger 
             << " | Refund: ₹" << fixed << setprecision(2) << refund.amount << "\n";
    }
}

void displayNetworkCost() {
    int cost = primMST();
    cout << "\n═══ Network Cost Analysis (Prim's MST) ═══\n"
         << "Minimum Spanning Tree Cost: ₹" << cost << "\n"
         << "This represents the minimum cost to connect all stations\n";
}

void displayShortestPath(int src, int dst) {
    vector<int> distances = dijkstra(src);
    
    cout << "\n═══ Shortest Path Analysis (Dijkstra's) ═══\n"
         << "From: " << getStationName(src) << "\n"
         << "Shortest distances:\n";
    
    for (int i = 0; i < (int)stations.size(); i++) {
        if (distances[i] < (int)INF) {
            cout << "  → " << getStationName(i) << ": " << distances[i] << "km";
            if (i == dst) cout << " [TARGET]";
            cout << "\n";
        }
    }
}

// ──────────────────────────────────────
// Demo & Menu
// ──────────────────────────────────────

void loadSampleData() {
    cout << "Loading sample data...\n";
    
    // Add 6 stations
    addStation("Mumbai");
    addStation("Delhi");
    addStation("Bangalore");
    addStation("Chennai");
    addStation("Kolkata");
    addStation("Hyderabad");
    
    // Add routes: from, to, distance, fare, capacity
    addRoute(0, 1, 1400, 1500, 100);
    addRoute(0, 2, 980, 1100, 90);
    addRoute(0, 5, 750, 900, 85);
    addRoute(1, 3, 2200, 2200, 110);
    addRoute(1, 4, 1500, 1600, 100);
    addRoute(2, 3, 350, 400, 80);
    addRoute(2, 5, 570, 650, 80);
    addRoute(3, 4, 1660, 1700, 95);
    addRoute(3, 5, 630, 700, 85);
    addRoute(4, 5, 1500, 1550, 75);
    
    cout << "✓ Sample data loaded\n";
}

void printMenu() {
    cout << "\n╔════════════════════════════════════════╗\n"
         << "║  Railway Network Management System     ║\n"
         << "╚════════════════════════════════════════╝\n"
         << "1. Book a seat\n"
         << "2. Cancel booking\n"
         << "3. Cascade cancel (route)\n"
         << "4. View active bookings\n"
         << "5. View route status\n"
         << "6. View refund history\n"
         << "7. Shortest path analysis\n"
         << "8. Network cost (MST)\n"
         << "9. Sorted bookings report\n"
         << "0. Exit\n"
         << "─────────────────────────────────────────\n"
         << "Choose option: ";
}

int main() {
    loadSampleData();
    
    // Pre-load some bookings
    bookSeat("Rahul Sharma", 0, 1, false, "2026-06-15");
    bookSeat("Priya Patel", 0, 1, true, "2026-06-15");
    bookSeat("Amit Kumar", 2, 3, false, "2026-06-20");
    bookSeat("Sneha Reddy", 2, 5, false, "2026-06-18");
    bookSeat("Vikram Singh", 1, 2, true, "2026-06-22");
    
    int choice;
    while (true) {
        printMenu();
        cin >> choice;
        cin.ignore();
        
        if (choice == 0) break;
        
        switch (choice) {
            case 1: {
                cout << "\n─ Book a Seat ─\n";
                cout << "Available stations:\n";
                for (const auto& s : stations) {
                    cout << "  " << s.id << ": " << s.name << "\n";
                }
                
                cout << "Enter source station ID: ";
                int src;
                cin >> src;
                
                cout << "Enter destination station ID: ";
                int dst;
                cin >> dst;
                cin.ignore();
                
                cout << "Enter passenger name: ";
                string name;
                getline(cin, name);
                
                cout << "Enter travel date (YYYY-MM-DD): ";
                string date;
                getline(cin, date);
                
                cout << "Is VIP? (y/n): ";
                char vipChar;
                cin >> vipChar;
                
                bookSeat(name, src, dst, vipChar == 'y', date);
                break;
            }
            
            case 2: {
                cout << "Enter PNR to cancel: ";
                int pnr;
                cin >> pnr;
                cancelBooking(pnr);
                break;
            }
            
            case 3: {
                cout << "\n─ Cascade Cancel ─\n";
                cout << "Enter source station ID: ";
                int src;
                cin >> src;
                cout << "Enter destination station ID: ";
                int dst;
                cin >> dst;
                cascadeCancel(src, dst);
                break;
            }
            
            case 4:
                displayActiveBookings();
                break;
            
            case 5:
                displayRouteStatus();
                break;
            
            case 6:
                displayRefundHistory();
                break;
            
            case 7: {
                cout << "Enter source station ID: ";
                int src;
                cin >> src;
                cout << "Enter destination station ID: ";
                int dst;
                cin >> dst;
                displayShortestPath(src, dst);
                break;
            }
            
            case 8:
                displayNetworkCost();
                break;
            
            case 9:
                generateSortedReport();
                break;
            
            default:
                cout << "Invalid option\n";
        }
    }
    
    cout << "Goodbye!\n";
    return 0;
}
