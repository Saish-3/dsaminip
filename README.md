# Railway Network Management System

A C++ implementation of a railway network management system with graph optimization, dynamic pricing, and booking management.

## Features Implemented

### 1. **Graph Representation** 
- Bidirectional routes between stations
- Each route stores: distance, capacity, base fare, and current occupancy
- Adjacency list representation for efficient traversal

### 2. **Dijkstra's Algorithm** (Shortest Path)
- Finds minimum distance between any two stations
- Used for route optimization and network analysis
- Time Complexity: O((V + E) log V) with priority queue

### 3. **Prim's Algorithm** (Minimum Spanning Tree)
- Calculates minimum cost to connect all stations
- Used for cost-optimized network planning
- Reports total network cost for infrastructure planning

### 4. **Booking System**
- PNR-based booking management
- Seat capacity tracking
- VIP and Regular passenger differentiation
- Active/inactive booking states

### 5. **Dynamic Pricing** (Greedy Algorithm)
- Base fare + 50% surge when occupancy > 80%
- Automatically applied during booking
- Incentivizes early bookings and load balancing

### 6. **Waitlist Management** (Priority Queue)
- VIP passengers prioritized over regular passengers
- FIFO (First In First Out) within same priority level
- Manual priority queue implementation

### 7. **Cancellation & Refunds** (Stack - LIFO)
- Refund stack stores cancellations in order
- Last-cancelled refund processed first
- Auto-promotion: Next waitlisted passenger automatically booked

### 8. **Cascade Cancellation**
- Cancel all bookings on a specific route
- Clear seats and refund all passengers
- Useful for maintenance or emergency scenarios

### 9. **Multi-Level Sorting** 
- Bookings sorted by: Route → Date → Fare
- Provides revenue insights and booking patterns

### 10. **Binary Search**
- Search sorted fare ranges
- Find booking positions by fare amount
- Efficient O(log N) lookup

## Data Structures Used

| Feature | Data Structure |
|---------|-----------------|
| Stations | Vector |
| Routes | Adjacency List (Vector of Vectors) |
| Bookings | Vector |
| Waitlist | Priority Queue |
| Refunds | Stack |
| Sorting | Vector + std::sort |
| Search | Binary Search (std::lower_bound) |

## Algorithms Used

| Algorithm | Purpose | Complexity |
|-----------|---------|-----------|
| **Dijkstra's** | Shortest path between stations | O((V+E) log V) |
| **Prim's MST** | Cost-optimized network | O(V² ) or O((V+E) log V) |
| **Greedy** | Dynamic pricing surge | O(1) |
| **Merge Sort** (via std::sort) | Multi-level sorting | O(N log N) |
| **Binary Search** | Fare lookup | O(log N) |

## Project Structure

```
minip/
├── railway_system.cpp      # Main program (self-contained)
├── Makefile                # Build configuration
└── README.md               # This file
```

## Compilation & Execution

### Build:
```bash
make
```

### Run:
```bash
make run
# or
./railway
```

### Clean:
```bash
make clean
```

## Menu Options

```
1. Book a seat              - Add new booking with VIP option
2. Cancel booking           - Refund and auto-promote waitlist
3. Cascade cancel           - Cancel all on a route
4. View active bookings     - List all active bookings
5. View route status        - Show occupancy & surge pricing
6. View refund history      - LIFO stack of refunds
7. Shortest path analysis   - Dijkstra's distances from station
8. Network cost (MST)       - Prim's minimum spanning tree
9. Sorted bookings report   - Multi-level sorted bookings
0. Exit                     - Close program
```

## Sample Execution

The program pre-loads:
- **6 Stations**: Mumbai, Delhi, Bangalore, Chennai, Kolkata, Hyderabad
- **10 Routes**: Bidirectional connections with realistic distances/fares
- **5 Sample Bookings**: Various passengers with different travel dates

### Example Output:

```
═══ Active Bookings ═══
PNR: 1001 | Rahul Sharma | Mumbai → Delhi | Date: 2026-06-15 | Fare: ₹1500.00 | Regular
PNR: 1002 | Priya Patel | Mumbai → Delhi | Date: 2026-06-15 | Fare: ₹1500.00 | VIP
...

═══ Route Status & Occupancy ═══
Mumbai ↔ Delhi | Distance: 1400km | Occupancy: 50.0% (50/100) | Fare: ₹1500.00
Mumbai ↔ Bangalore | Distance: 980km | Occupancy: 0.0% (0/90) | Fare: ₹1100.00
```

## Key Features Highlighted

### Custom Features (Group Requirements):

1. **Dynamic Pricing System**
   - Implements greedy surge pricing
   - Adapts fares based on real-time occupancy
   - Incentivizes bookings and revenue optimization

2. **Advanced Waitlist Management**
   - Priority queue with VIP support
   - Automatic promotion on cancellation
   - Timestamp-based FIFO within priority levels

3. **Network Optimization**
   - Dijkstra's for routing optimization
   - Prim's MST for infrastructure planning
   - Multi-level sorting for business intelligence

4. **Cascading Refunds**
   - Stack-based LIFO refund tracking
   - Emergency route cancellations
   - Complete booking rollback capability

## Compilation Requirements

- **C++ Standard**: C++17 or later
- **Compiler**: g++, clang++, or equivalent
- **Platform**: Linux, macOS, Windows (with appropriate compiler)

### Compile with clang:
```bash
clang++ -std=c++17 -o railway railway_system.cpp
```

## Time Complexities

| Operation | Complexity | Notes |
|-----------|-----------|-------|
| Book Seat | O(log W) | W = waitlist size |
| Cancel Booking | O(N) | N = total bookings |
| Dijkstra's | O((V+E) log V) | V = stations, E = routes |
| Prim's MST | O(V² ) or O((V+E) log V) | Depending on implementation |
| Sort Bookings | O(N log N) | N = bookings |
| Binary Search Fare | O(log N) | N = sorted fares |

## Space Complexities

| Component | Complexity |
|-----------|-----------|
| Stations | O(V) |
| Routes | O(E) |
| Bookings | O(N) |
| Waitlist | O(W) |
| Refunds | O(R) |

## Future Enhancements

- [ ] Persistent storage (file I/O)
- [ ] Database integration
- [ ] Advanced pricing models
- [ ] Multi-stop journeys
- [ ] Train schedule management
- [ ] Real-time tracking
- [ ] Notification system
- [ ] Analytics dashboard

## Notes

- All prices are in Indian Rupees (₹)
- Distances are in kilometers
- VIP passengers get priority in waitlist
- Occupancy tracking affects dynamic pricing
- Auto-promotion processes immediately on cancellation

---

**Author**: DSA Project  
**Date**: May 2026  
**Language**: C++ (C++17)
