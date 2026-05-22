import React, { useState, useEffect } from 'react';
import { RailwaySystem } from './railway-system';
import './App.css';

export default function App() {
  const [railway, setRailway] = useState(null);
  const [activeTab, setActiveTab] = useState('overview');
  const [bookings, setBookings] = useState([]);
  const [routes, setRoutes] = useState([]);
  const [waitlist, setWaitlist] = useState([]);
  const [refunds, setRefunds] = useState([]);
  const [shortestPath, setShortestPath] = useState(null);
  const [networkCost, setNetworkCost] = useState(null);
  const [sortedBookings, setSortedBookings] = useState([]);
  const [message, setMessage] = useState('');

  // Form states
  const [bookingForm, setBookingForm] = useState({
    passenger: '',
    source: '',
    dest: '',
    date: '',
    isVIP: false,
  });

  const [cancelForm, setCancelForm] = useState({
    pnr: '',
  });

  const [cascadeForm, setCascadeForm] = useState({
    source: '',
    dest: '',
  });

  const [pathForm, setPathForm] = useState({
    source: '',
    dest: '',
  });

  // Initialize railway system
  useEffect(() => {
    const system = new RailwaySystem();
    system.loadSampleData();
    setRailway(system);
    updateAllData(system);
  }, []);

  const updateAllData = (system) => {
    setBookings(system.getActiveBookings());
    setRoutes(system.getRouteStatus());
    setWaitlist(system.getWaitlist());
    setRefunds(system.getRefundHistory());
    setSortedBookings(system.getSortedBookings());
  };

  const handleBooking = (e) => {
    e.preventDefault();
    if (!railway || !bookingForm.passenger || !bookingForm.source || !bookingForm.dest || !bookingForm.date) {
      setMessage('❌ Please fill all fields');
      return;
    }

    const result = railway.bookSeat(
      bookingForm.passenger,
      parseInt(bookingForm.source),
      parseInt(bookingForm.dest),
      bookingForm.isVIP,
      bookingForm.date
    );

    setMessage(result.message);
    setBookingForm({ passenger: '', source: '', dest: '', date: '', isVIP: false });
    updateAllData(railway);
  };

  const handleCancel = (e) => {
    e.preventDefault();
    if (!railway || !cancelForm.pnr) {
      setMessage('❌ Please enter PNR');
      return;
    }

    const result = railway.cancelBooking(parseInt(cancelForm.pnr));
    setMessage(result.message);
    setCancelForm({ pnr: '' });
    updateAllData(railway);
  };

  const handleCascadeCancel = (e) => {
    e.preventDefault();
    if (!railway || !cascadeForm.source || !cascadeForm.dest) {
      setMessage('❌ Please select both stations');
      return;
    }

    const result = railway.cascadeCancel(parseInt(cascadeForm.source), parseInt(cascadeForm.dest));
    setMessage(result.message);
    setCascadeForm({ source: '', dest: '' });
    updateAllData(railway);
  };

  const handleShortestPath = (e) => {
    e.preventDefault();
    if (!railway || !pathForm.source || !pathForm.dest) {
      setMessage('❌ Please select both stations');
      return;
    }

    const result = railway.getShortestPath(parseInt(pathForm.source), parseInt(pathForm.dest));
    setShortestPath(result);
    setActiveTab('shortest-path');
  };

  const handleNetworkCost = () => {
    if (!railway) return;
    const result = railway.getNetworkCost();
    setNetworkCost(result);
    setActiveTab('network-cost');
  };

  if (!railway) return <div className="loading">Loading...</div>;

  const stations = railway.stations;

  return (
    <div className="app">
      <header className="header">
        <h1>🚂 Railway Network Management System</h1>
        <p>Interactive React Application with Full DSA Implementation</p>
      </header>

      {message && (
        <div className={`message ${message.includes('❌') ? 'error' : message.includes('⏳') ? 'warning' : 'success'}`}>
          {message}
          <button onClick={() => setMessage('')}>×</button>
        </div>
      )}

      <div className="tabs">
        <button className={`tab-btn ${activeTab === 'overview' ? 'active' : ''}`} onClick={() => setActiveTab('overview')}>
          📊 Overview
        </button>
        <button className={`tab-btn ${activeTab === 'booking' ? 'active' : ''}`} onClick={() => setActiveTab('booking')}>
          🎫 Book Seat
        </button>
        <button className={`tab-btn ${activeTab === 'cancel' ? 'active' : ''}`} onClick={() => setActiveTab('cancel')}>
          ❌ Cancel
        </button>
        <button className={`tab-btn ${activeTab === 'routes' ? 'active' : ''}`} onClick={() => setActiveTab('routes')}>
          📍 Routes
        </button>
        <button className={`tab-btn ${activeTab === 'bookings' ? 'active' : ''}`} onClick={() => setActiveTab('bookings')}>
          📋 Bookings
        </button>
        <button className={`tab-btn ${activeTab === 'waitlist' ? 'active' : ''}`} onClick={() => setActiveTab('waitlist')}>
          ⏳ Waitlist
        </button>
        <button className={`tab-btn ${activeTab === 'refunds' ? 'active' : ''}`} onClick={() => setActiveTab('refunds')}>
          💵 Refunds
        </button>
        <button className={`tab-btn ${activeTab === 'shortest-path' ? 'active' : ''}`} onClick={() => setActiveTab('shortest-path')}>
          🛣️ Dijkstra
        </button>
        <button className={`tab-btn ${activeTab === 'network-cost' ? 'active' : ''}`} onClick={() => setActiveTab('network-cost')}>
          🌐 MST
        </button>
      </div>

      <div className="content">
        {/* Overview Tab */}
        {activeTab === 'overview' && (
          <div className="tab-content">
            <h2>🚂 System Overview</h2>
            <div className="overview-grid">
              <div className="stat-card">
                <h3>Total Stations</h3>
                <p className="stat-value">{stations.length}</p>
              </div>
              <div className="stat-card">
                <h3>Active Bookings</h3>
                <p className="stat-value">{bookings.length}</p>
              </div>
              <div className="stat-card">
                <h3>Waitlisted</h3>
                <p className="stat-value">{waitlist.length}</p>
              </div>
              <div className="stat-card">
                <h3>Total Refunds</h3>
                <p className="stat-value">{refunds.length}</p>
              </div>
            </div>

            <h3 style={{ marginTop: '30px' }}>Available Stations</h3>
            <div className="stations-grid">
              {stations.map((s) => (
                <div key={s.id} className="station-badge">
                  {s.id}: {s.name}
                </div>
              ))}
            </div>

            <h3 style={{ marginTop: '30px' }}>System Features</h3>
            <div className="features-grid">
              <div className="feature-card">
                <h4>✓ Dijkstra's Algorithm</h4>
                <p>Find shortest paths O(E log V)</p>
              </div>
              <div className="feature-card">
                <h4>✓ Prim's MST</h4>
                <p>Minimum spanning tree O(E log V)</p>
              </div>
              <div className="feature-card">
                <h4>✓ Priority Queue</h4>
                <p>VIP waitlist management</p>
              </div>
              <div className="feature-card">
                <h4>✓ Dynamic Pricing</h4>
                <p>50% surge at 80% occupancy</p>
              </div>
              <div className="feature-card">
                <h4>✓ Cascading Cancel</h4>
                <p>Batch cancellations with auto-promote</p>
              </div>
              <div className="feature-card">
                <h4>✓ Stack Refunds</h4>
                <p>LIFO refund tracking</p>
              </div>
            </div>
          </div>
        )}

        {/* Booking Tab */}
        {activeTab === 'booking' && (
          <div className="tab-content">
            <h2>🎫 Book a Seat</h2>
            <form onSubmit={handleBooking} className="form">
              <div className="form-group">
                <label>Passenger Name</label>
                <input
                  type="text"
                  value={bookingForm.passenger}
                  onChange={(e) => setBookingForm({ ...bookingForm, passenger: e.target.value })}
                  placeholder="Enter passenger name"
                />
              </div>

              <div className="form-row">
                <div className="form-group">
                  <label>From Station</label>
                  <select
                    value={bookingForm.source}
                    onChange={(e) => setBookingForm({ ...bookingForm, source: e.target.value })}
                  >
                    <option value="">Select station</option>
                    {stations.map((s) => (
                      <option key={s.id} value={s.id}>
                        {s.id}: {s.name}
                      </option>
                    ))}
                  </select>
                </div>

                <div className="form-group">
                  <label>To Station</label>
                  <select
                    value={bookingForm.dest}
                    onChange={(e) => setBookingForm({ ...bookingForm, dest: e.target.value })}
                  >
                    <option value="">Select station</option>
                    {stations.map((s) => (
                      <option key={s.id} value={s.id}>
                        {s.id}: {s.name}
                      </option>
                    ))}
                  </select>
                </div>
              </div>

              <div className="form-group">
                <label>Travel Date</label>
                <input
                  type="date"
                  value={bookingForm.date}
                  onChange={(e) => setBookingForm({ ...bookingForm, date: e.target.value })}
                />
              </div>

              <div className="form-group checkbox">
                <label>
                  <input
                    type="checkbox"
                    checked={bookingForm.isVIP}
                    onChange={(e) => setBookingForm({ ...bookingForm, isVIP: e.target.checked })}
                  />
                  VIP Passenger (Priority in waitlist)
                </label>
              </div>

              <button type="submit" className="btn-primary">
                Book Seat
              </button>
            </form>
          </div>
        )}

        {/* Cancel Tab */}
        {activeTab === 'cancel' && (
          <div className="tab-content">
            <h2>❌ Manage Bookings</h2>

            <div className="cancel-section">
              <h3>Cancel Specific Booking</h3>
              <form onSubmit={handleCancel} className="form">
                <div className="form-group">
                  <label>PNR Number</label>
                  <input
                    type="number"
                    value={cancelForm.pnr}
                    onChange={(e) => setCancelForm({ pnr: e.target.value })}
                    placeholder="Enter PNR (e.g., 1001)"
                  />
                </div>
                <button type="submit" className="btn-danger">
                  Cancel Booking
                </button>
              </form>
            </div>

            <div className="cancel-section">
              <h3>Cascade Cancel (Route)</h3>
              <form onSubmit={handleCascadeCancel} className="form">
                <div className="form-row">
                  <div className="form-group">
                    <label>From Station</label>
                    <select
                      value={cascadeForm.source}
                      onChange={(e) => setCascadeForm({ ...cascadeForm, source: e.target.value })}
                    >
                      <option value="">Select station</option>
                      {stations.map((s) => (
                        <option key={s.id} value={s.id}>
                          {s.id}: {s.name}
                        </option>
                      ))}
                    </select>
                  </div>

                  <div className="form-group">
                    <label>To Station</label>
                    <select
                      value={cascadeForm.dest}
                      onChange={(e) => setCascadeForm({ ...cascadeForm, dest: e.target.value })}
                    >
                      <option value="">Select station</option>
                      {stations.map((s) => (
                        <option key={s.id} value={s.id}>
                          {s.id}: {s.name}
                        </option>
                      ))}
                    </select>
                  </div>
                </div>
                <button type="submit" className="btn-warning">
                  Cancel All on Route
                </button>
              </form>
            </div>
          </div>
        )}

        {/* Routes Tab */}
        {activeTab === 'routes' && (
          <div className="tab-content">
            <h2>📍 Route Status & Occupancy</h2>
            <div className="table-responsive">
              <table className="table">
                <thead>
                  <tr>
                    <th>Route</th>
                    <th>Distance</th>
                    <th>Booked</th>
                    <th>Occupancy</th>
                    <th>Base Fare</th>
                    <th>Current Fare</th>
                    <th>Status</th>
                  </tr>
                </thead>
                <tbody>
                  {routes.map((route, idx) => (
                    <tr key={idx}>
                      <td>{route.from} ↔ {route.to}</td>
                      <td>{route.distance} km</td>
                      <td>{route.booked}/{route.capacity}</td>
                      <td>
                        <div className="progress">
                          <div
                            className="progress-bar"
                            style={{
                              width: `${route.occupancy}%`,
                              background: route.occupancy > 80 ? '#ef4444' : '#3b82f6',
                            }}
                          />
                        </div>
                        {route.occupancy}%
                      </td>
                      <td>₹{route.baseFare}</td>
                      <td className={route.isSurge ? 'surge' : ''}>
                        ₹{route.fare} {route.isSurge && '📈'}
                      </td>
                      <td>
                        {route.occupancy > 80 ? (
                          <span className="badge-danger">Full</span>
                        ) : (
                          <span className="badge-success">Available</span>
                        )}
                      </td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </div>
        )}

        {/* Bookings Tab */}
        {activeTab === 'bookings' && (
          <div className="tab-content">
            <h2>📋 Active Bookings ({bookings.length})</h2>
            {bookings.length === 0 ? (
              <p className="no-data">No active bookings</p>
            ) : (
              <div className="table-responsive">
                <table className="table">
                  <thead>
                    <tr>
                      <th>PNR</th>
                      <th>Passenger</th>
                      <th>Route</th>
                      <th>Date</th>
                      <th>Fare</th>
                      <th>Type</th>
                    </tr>
                  </thead>
                  <tbody>
                    {bookings.map((bk) => (
                      <tr key={bk.pnr}>
                        <td className="pnr">{bk.pnr}</td>
                        <td>{bk.passenger}</td>
                        <td>{bk.route}</td>
                        <td>{bk.date}</td>
                        <td className="fare">₹{bk.fare}</td>
                        <td>
                          <span className={bk.type === 'VIP' ? 'badge-vip' : 'badge-regular'}>
                            {bk.type}
                          </span>
                        </td>
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            )}
          </div>
        )}

        {/* Waitlist Tab */}
        {activeTab === 'waitlist' && (
          <div className="tab-content">
            <h2>⏳ Waitlist ({waitlist.length})</h2>
            {waitlist.length === 0 ? (
              <p className="no-data">No passengers on waitlist</p>
            ) : (
              <div className="table-responsive">
                <table className="table">
                  <thead>
                    <tr>
                      <th>#</th>
                      <th>Passenger</th>
                      <th>Route</th>
                      <th>Date</th>
                      <th>Type</th>
                    </tr>
                  </thead>
                  <tbody>
                    {waitlist.map((entry) => (
                      <tr key={entry.idx}>
                        <td>{entry.idx + 1}</td>
                        <td>{entry.passenger}</td>
                        <td>{entry.route}</td>
                        <td>{entry.date}</td>
                        <td>
                          <span className={entry.type === 'VIP' ? 'badge-vip' : 'badge-regular'}>
                            {entry.type}
                          </span>
                        </td>
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            )}
          </div>
        )}

        {/* Refunds Tab */}
        {activeTab === 'refunds' && (
          <div className="tab-content">
            <h2>💵 Refund History - LIFO Stack ({refunds.length})</h2>
            {refunds.length === 0 ? (
              <p className="no-data">No refunds processed</p>
            ) : (
              <div className="table-responsive">
                <table className="table">
                  <thead>
                    <tr>
                      <th>Order</th>
                      <th>PNR</th>
                      <th>Passenger</th>
                      <th>Refund Amount</th>
                    </tr>
                  </thead>
                  <tbody>
                    {refunds.map((refund) => (
                      <tr key={refund.idx}>
                        <td className="order">{refunds.length - refund.idx}</td>
                        <td className="pnr">{refund.pnr}</td>
                        <td>{refund.passenger}</td>
                        <td className="refund">₹{refund.amount}</td>
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            )}
          </div>
        )}

        {/* Dijkstra Tab */}
        {activeTab === 'shortest-path' && (
          <div className="tab-content">
            <h2>🛣️ Shortest Path (Dijkstra's Algorithm)</h2>

            <form onSubmit={handleShortestPath} className="form">
              <div className="form-row">
                <div className="form-group">
                  <label>From Station</label>
                  <select
                    value={pathForm.source}
                    onChange={(e) => setPathForm({ ...pathForm, source: e.target.value })}
                  >
                    <option value="">Select station</option>
                    {stations.map((s) => (
                      <option key={s.id} value={s.id}>
                        {s.id}: {s.name}
                      </option>
                    ))}
                  </select>
                </div>

                <div className="form-group">
                  <label>To Station</label>
                  <select
                    value={pathForm.dest}
                    onChange={(e) => setPathForm({ ...pathForm, dest: e.target.value })}
                  >
                    <option value="">Select station</option>
                    {stations.map((s) => (
                      <option key={s.id} value={s.id}>
                        {s.id}: {s.name}
                      </option>
                    ))}
                  </select>
                </div>
              </div>
              <button type="submit" className="btn-primary">
                Calculate Path
              </button>
            </form>

            {shortestPath && (
              <div className="result-section">
                <h3>Results from {shortestPath.from}</h3>
                <div className="alert-info">
                  <strong>Optimal Route: </strong>{shortestPath.path}
                  <strong style={{ marginLeft: '20px' }}>Distance: </strong>{shortestPath.targetDistance} km
                </div>

                <h4>Distances to All Stations</h4>
                <div className="distances-grid">
                  {shortestPath.distances.map((d, idx) => (
                    <div key={idx} className={`distance-card ${d.isTarget ? 'target' : ''}`}>
                      <p className="station-name">{d.station}</p>
                      <p className="distance">{d.distance} km</p>
                    </div>
                  ))}
                </div>
              </div>
            )}
          </div>
        )}

        {/* Network Cost Tab */}
        {activeTab === 'network-cost' && (
          <div className="tab-content">
            <h2>🌐 Network Cost Analysis (Prim's MST)</h2>
            <button onClick={handleNetworkCost} className="btn-primary">
              Calculate MST Cost
            </button>

            {networkCost && (
              <div className="result-section">
                <div className="alert-success">
                  <h3>{networkCost.message}</h3>
                  <p className="cost">₹{networkCost.cost}</p>
                </div>
                <p className="description">{networkCost.description}</p>
                <p className="algorithm-info">
                  <strong>Algorithm:</strong> Prim's Minimum Spanning Tree - O(E log V) complexity
                </p>
              </div>
            )}
          </div>
        )}
      </div>

      <footer className="footer">
        <p>🚀 Railway Network Management System | React + Advanced Data Structures & Algorithms</p>
        <p>Dijkstra O(E log V) | Prim's O(E log V) | Booking O(1) | Cancellation O(n)</p>
      </footer>
    </div>
  );
}
