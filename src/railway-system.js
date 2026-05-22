// Railway Network Management System - JavaScript Implementation

class Station {
  constructor(id, name) {
    this.id = id;
    this.name = name;
  }
}

class Route {
  constructor(from, to, distance, capacity, baseFare) {
    this.from = from;
    this.to = to;
    this.distance = distance;
    this.capacity = capacity;
    this.booked = 0;
    this.baseFare = baseFare;
  }

  getCurrentFare() {
    if (this.capacity === 0) return this.baseFare;
    const occupancy = this.booked / this.capacity;
    if (occupancy > 0.8) {
      return this.baseFare * 1.5; // 50% surge when >80% full
    }
    return this.baseFare;
  }

  getOccupancy() {
    return (this.booked / this.capacity) * 100;
  }
}

class Booking {
  constructor(pnr, passenger, source, dest, date, isVIP, fare) {
    this.pnr = pnr;
    this.passenger = passenger;
    this.source = source;
    this.dest = dest;
    this.date = date;
    this.isVIP = isVIP;
    this.fare = fare;
    this.active = true;
  }
}

class WaitlistEntry {
  constructor(passenger, source, dest, date, isVIP, timestamp) {
    this.passenger = passenger;
    this.source = source;
    this.dest = dest;
    this.date = date;
    this.isVIP = isVIP;
    this.timestamp = timestamp;
  }
}

export class RailwaySystem {
  constructor() {
    this.stations = [];
    this.adjList = [];
    this.bookings = [];
    this.waitlist = [];
    this.refunds = [];
    this.nextPNR = 1001;
    this.globalTimestamp = 0;
  }

  addStation(name) {
    const id = this.stations.length;
    this.stations.push(new Station(id, name));
    this.adjList.push([]);
    return id;
  }

  getStationId(name) {
    const station = this.stations.find((s) => s.name === name);
    return station ? station.id : -1;
  }

  getStationName(id) {
    const station = this.stations[id];
    return station ? station.name : "Unknown";
  }

  addRoute(from, to, distance, fare, capacity) {
    if (from >= this.adjList.length || to >= this.adjList.length) return;

    const route1 = new Route(from, to, distance, capacity, fare);
    const route2 = new Route(to, from, distance, capacity, fare);

    this.adjList[from].push(route1);
    this.adjList[to].push(route2);
  }

  findRoute(from, to) {
    if (from >= this.adjList.length || to >= this.adjList.length) return null;
    return this.adjList[from].find((r) => r.to === to) || null;
  }

  // Dijkstra's Algorithm
  dijkstra(start) {
    const dist = Array(this.stations.length).fill(Infinity);
    const pq = [];

    dist[start] = 0;
    pq.push([0, start]);

    while (pq.length > 0) {
      pq.sort((a, b) => a[0] - b[0]);
      const [d, u] = pq.shift();

      if (d > dist[u]) continue;

      for (const edge of this.adjList[u]) {
        const v = edge.to;
        const newDist = dist[u] + edge.distance;

        if (newDist < dist[v]) {
          dist[v] = newDist;
          pq.push([newDist, v]);
        }
      }
    }

    return dist;
  }

  // Prim's Algorithm for MST
  primMST() {
    const visited = Array(this.stations.length).fill(false);
    const pq = [];
    let totalCost = 0;

    visited[0] = true;

    for (const edge of this.adjList[0]) {
      pq.push([edge.distance, 0, edge.to]);
    }

    let edgeCount = 0;
    while (pq.length > 0 && edgeCount < this.stations.length - 1) {
      pq.sort((a, b) => a[0] - b[0]);
      const [cost, u, v] = pq.shift();

      if (visited[v]) continue;

      visited[v] = true;
      totalCost += cost;
      edgeCount++;

      for (const edge of this.adjList[v]) {
        if (!visited[edge.to]) {
          pq.push([edge.distance, v, edge.to]);
        }
      }
    }

    return totalCost;
  }

  // Booking with dynamic pricing
  bookSeat(passenger, src, dst, isVIP, date) {
    const route = this.findRoute(src, dst);
    if (!route) {
      return { success: false, message: "❌ No direct route found!" };
    }

    if (route.booked < route.capacity) {
      const fare = route.getCurrentFare();
      const pnr = this.nextPNR++;
      const booking = new Booking(pnr, passenger, src, dst, date, isVIP, fare);
      this.bookings.push(booking);
      route.booked++;

      // Update reverse route
      const revRoute = this.findRoute(dst, src);
      if (revRoute) revRoute.booked++;

      return {
        success: true,
        message: `✓ Booking successful!`,
        booking: {
          pnr,
          passenger,
          route: `${this.getStationName(src)} → ${this.getStationName(dst)}`,
          fare,
          isSurge: fare > route.baseFare,
        },
      };
    } else {
      const entry = new WaitlistEntry(
        passenger,
        src,
        dst,
        date,
        isVIP,
        this.globalTimestamp++
      );
      this.waitlist.push(entry);

      // Sort waitlist (VIP first, then FIFO)
      this.waitlist.sort((a, b) => {
        if (a.isVIP !== b.isVIP) return b.isVIP - a.isVIP;
        return a.timestamp - b.timestamp;
      });

      return {
        success: false,
        message: `⏳ Seat full! Added to waitlist (${isVIP ? "VIP" : "Regular"})`,
        waitlisted: true,
      };
    }
  }

  // Cancel a booking
  cancelBooking(pnr) {
    const booking = this.bookings.find((bk) => bk.pnr === pnr && bk.active);
    if (!booking) {
      return { success: false, message: "❌ PNR not found" };
    }

    booking.active = false;
    this.refunds.unshift({ pnr, passenger: booking.passenger, amount: booking.fare });

    // Free up seat
    const route = this.findRoute(booking.source, booking.dest);
    if (route) route.booked--;

    const revRoute = this.findRoute(booking.dest, booking.source);
    if (revRoute) revRoute.booked--;

    let autoPromoteMsg = "";

    // Auto-promote from waitlist
    if (this.waitlist.length > 0) {
      for (let i = 0; i < this.waitlist.length; i++) {
        const entry = this.waitlist[i];
        if (entry.source === booking.source && entry.dest === booking.dest) {
          this.waitlist.splice(i, 1);
          const result = this.bookSeat(
            entry.passenger,
            entry.source,
            entry.dest,
            entry.isVIP,
            entry.date
          );
          if (result.success) {
            autoPromoteMsg = `\n↑ Auto-promoted ${entry.passenger} from waitlist!`;
          }
          break;
        }
      }
    }

    return {
      success: true,
      message: `✓ Booking ${pnr} cancelled\n✓ Refund: ₹${booking.fare.toFixed(2)}${autoPromoteMsg}`,
      refund: { pnr, passenger: booking.passenger, amount: booking.fare },
    };
  }

  // Cascade cancel all bookings on a route
  cascadeCancel(src, dst) {
    let count = 0;
    const cancelled = [];

    for (const bk of this.bookings) {
      if (bk.active && bk.source === src && bk.dest === dst) {
        bk.active = false;
        this.refunds.unshift({
          pnr: bk.pnr,
          passenger: bk.passenger,
          amount: bk.fare,
        });
        cancelled.push({ pnr: bk.pnr, passenger: bk.passenger, fare: bk.fare });
        count++;
      }
    }

    const route = this.findRoute(src, dst);
    if (route) route.booked = 0;

    return {
      success: true,
      message: `⚠ Cancelled ${count} bookings on ${this.getStationName(src)} → ${this.getStationName(dst)}`,
      cancelled,
    };
  }

  // Get all active bookings
  getActiveBookings() {
    return this.bookings.filter((bk) => bk.active).map((bk) => ({
      pnr: bk.pnr,
      passenger: bk.passenger,
      route: `${this.getStationName(bk.source)} → ${this.getStationName(bk.dest)}`,
      date: bk.date,
      fare: bk.fare.toFixed(2),
      type: bk.isVIP ? "VIP" : "Regular",
    }));
  }

  // Get route status
  getRouteStatus() {
    const routes = [];
    const seen = new Set();

    for (let i = 0; i < this.stations.length; i++) {
      for (const route of this.adjList[i]) {
        const key = [Math.min(i, route.to), Math.max(i, route.to)].join("-");
        if (!seen.has(key)) {
          seen.add(key);
          routes.push({
            from: this.getStationName(route.from),
            to: this.getStationName(route.to),
            distance: route.distance,
            occupancy: route.getOccupancy().toFixed(1),
            booked: route.booked,
            capacity: route.capacity,
            fare: route.getCurrentFare().toFixed(2),
            baseFare: route.baseFare.toFixed(2),
            isSurge: route.getCurrentFare() > route.baseFare,
          });
        }
      }
    }

    return routes;
  }

  // Get refund history (stack - LIFO)
  getRefundHistory() {
    return this.refunds.map((refund, idx) => ({
      idx,
      pnr: refund.pnr,
      passenger: refund.passenger,
      amount: refund.amount.toFixed(2),
    }));
  }

  // Get waitlist
  getWaitlist() {
    return this.waitlist.map((entry, idx) => ({
      idx,
      passenger: entry.passenger,
      route: `${this.getStationName(entry.source)} → ${this.getStationName(entry.dest)}`,
      type: entry.isVIP ? "VIP" : "Regular",
      date: entry.date,
    }));
  }

  // Shortest path with actual path
  getShortestPath(src, dst) {
    const distances = this.dijkstra(src);
    const paths = Array(this.stations.length).fill(null);
    paths[src] = [src];

    const visited = Array(this.stations.length).fill(false);
    const pq = [[0, src]];

    while (pq.length > 0) {
      pq.sort((a, b) => a[0] - b[0]);
      const [d, u] = pq.shift();

      if (visited[u]) continue;
      visited[u] = true;

      for (const edge of this.adjList[u]) {
        const v = edge.to;
        if (distances[u] + edge.distance < distances[v]) {
          distances[v] = distances[u] + edge.distance;
          paths[v] = [...paths[u], v];
          pq.push([distances[v], v]);
        }
      }
    }

    return {
      from: this.getStationName(src),
      distances: this.stations.map((s, i) => ({
        station: s.name,
        distance: distances[i] === Infinity ? "∞" : distances[i],
        isTarget: i === dst,
      })),
      targetDistance: distances[dst] === Infinity ? "No route" : distances[dst],
      path: paths[dst]
        ? paths[dst].map((id) => this.getStationName(id)).join(" → ")
        : "No route",
    };
  }

  // Network cost using MST
  getNetworkCost() {
    const cost = this.primMST();
    return {
      message: "Minimum Spanning Tree Cost (Prim's Algorithm)",
      cost,
      description: "Minimum cost to connect all stations in the network",
    };
  }

  // Get sorted bookings
  getSortedBookings() {
    const sorted = this.bookings
      .filter((bk) => bk.active)
      .map((bk) => ({
        route: bk.source * this.stations.length + bk.dest,
        routeName: `${this.getStationName(bk.source)} → ${this.getStationName(bk.dest)}`,
        date: bk.date,
        fare: bk.fare.toFixed(2),
        passenger: bk.passenger,
        pnr: bk.pnr,
      }))
      .sort((a, b) => {
        if (a.route !== b.route) return a.route - b.route;
        if (a.date !== b.date) return a.date.localeCompare(b.date);
        return parseFloat(a.fare) - parseFloat(b.fare);
      });

    return sorted;
  }

  // Load sample data
  loadSampleData() {
    this.addStation("Mumbai");
    this.addStation("Delhi");
    this.addStation("Bangalore");
    this.addStation("Chennai");
    this.addStation("Kolkata");
    this.addStation("Hyderabad");

    this.addRoute(0, 1, 1400, 1500, 100);
    this.addRoute(0, 2, 980, 1100, 90);
    this.addRoute(0, 5, 750, 900, 85);
    this.addRoute(1, 3, 2200, 2200, 110);
    this.addRoute(1, 4, 1500, 1600, 100);
    this.addRoute(2, 3, 350, 400, 80);
    this.addRoute(2, 5, 570, 650, 80);
    this.addRoute(3, 4, 1660, 1700, 95);
    this.addRoute(3, 5, 630, 700, 85);
    this.addRoute(4, 5, 1500, 1550, 75);

    // Pre-load some bookings
    this.bookSeat("Rahul Sharma", 0, 1, false, "2026-06-15");
    this.bookSeat("Priya Patel", 0, 1, true, "2026-06-15");
    this.bookSeat("Amit Kumar", 2, 3, false, "2026-06-20");
    this.bookSeat("Sneha Reddy", 2, 5, false, "2026-06-18");
    this.bookSeat("Vikram Singh", 1, 2, true, "2026-06-22");
  }
}
