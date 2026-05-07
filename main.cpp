// ═══════════════════════════════════════════════════════════════
//  PS_02: Railway Network Management System with Graph Optimization
//  DSA Used: Graph, Linked List, Stack, Queue (Priority), Heap,
//            Dijkstra's, Prim's MST, BFS, Merge Sort,
//            Binary Search, Greedy
// ═══════════════════════════════════════════════════════════════
#include "algo.h"

void loadSampleData(){
    // 8 Indian Railway Stations (Linked List)
    addStation("Mumbai");     // 0
    addStation("Delhi");      // 1
    addStation("Chennai");    // 2
    addStation("Kolkata");    // 3
    addStation("Jaipur");     // 4
    addStation("Ahmedabad");  // 5
    addStation("Bangalore");  // 6
    addStation("Hyderabad");  // 7

    // 12 Routes (Graph edges)
    addRoute(0,1,1400,1500,100);  // Mumbai-Delhi
    addRoute(0,5,530,600,80);     // Mumbai-Ahmedabad
    addRoute(0,6,980,1100,90);    // Mumbai-Bangalore
    addRoute(1,4,280,350,70);     // Delhi-Jaipur
    addRoute(1,3,1500,1600,100);  // Delhi-Kolkata
    addRoute(1,2,2200,2200,110);  // Delhi-Chennai
    addRoute(2,6,350,400,80);     // Chennai-Bangalore
    addRoute(2,7,630,700,85);     // Chennai-Hyderabad
    addRoute(2,3,1660,1700,95);   // Chennai-Kolkata
    addRoute(3,7,1500,1550,75);   // Kolkata-Hyderabad
    addRoute(6,7,570,650,80);     // Bangalore-Hyderabad
    addRoute(4,5,680,750,65);     // Jaipur-Ahmedabad

    // Pre-load bookings for demo
    // Mumbai-Delhi: high occupancy to demo surge pricing
    Edge* md=findEdge(0,1); if(md)md->booked=82;
    Edge* dm=findEdge(1,0); if(dm)dm->booked=82;

    bookSeat("Rahul Sharma",0,1,false,"2026-06-15");
    bookSeat("Priya Patel",0,1,true,"2026-06-15");
    bookSeat("Amit Kumar",2,6,false,"2026-06-20");
    bookSeat("Sneha Reddy",6,7,false,"2026-06-18");
    bookSeat("Vikram Singh",1,4,true,"2026-06-22");
    bookSeat("Ananya Das",1,3,false,"2026-06-25");
    bookSeat("Rohan Mehta",0,6,false,"2026-06-20");
}

void printMenu(){
    cout<<"\n"<<BLU<<BLD;
    cout<<"╔═══════════════════════════════════════════════════╗\n";
    cout<<"║        🚂 RAILWAY MANAGEMENT SYSTEM 🚂           ║\n";
    cout<<"╠═══════════════════════════════════════════════════╣\n";
    cout<<"║  1.  Add Station          2.  Add Route          ║\n";
    cout<<"║  3.  View Network         4.  View Stations      ║\n";
    cout<<"║  5.  Book Ticket          6.  Cancel Ticket      ║\n";
    cout<<"║  7.  Cancel Route (Cascade)                      ║\n";
    cout<<"║  8.  View Bookings        9.  View Waitlist      ║\n";
    cout<<"║  10. View Refund Stack                           ║\n";
    cout<<"║  11. Shortest Path (Dijkstra)                    ║\n";
    cout<<"║  12. Least Crowded Route                         ║\n";
    cout<<"║  13. MST Report (Prim's)                         ║\n";
    cout<<"║  14. Check Dynamic Pricing                       ║\n";
    cout<<"║  15. Sort Bookings (Merge Sort)                  ║\n";
    cout<<"║  16. Search Fare Range (Binary Search)           ║\n";
    cout<<"║  17. Simulate Delay (BFS)                        ║\n";
    cout<<"║  18. Revenue Dashboard (Heap)                    ║\n";
    cout<<"║  0.  Exit                                        ║\n";
    cout<<"╚═══════════════════════════════════════════════════╝"<<RST<<endl;
    cout<<"  Enter choice: ";
}

int main(){
    loadSampleData();
    cout<<GRN<<BLD<<"\n  ✓ System loaded with 8 stations, 12 routes, 7 bookings.\n"<<RST;

    int ch;
    do{
        printMenu();
        cin>>ch; cin.ignore();
        switch(ch){
        case 1:{
            string nm; cout<<"  Station name: "; getline(cin,nm);
            addStation(nm);
            cout<<GRN<<"  ✓ Added: "<<nm<<" (ID:"<<sCnt-1<<")"<<RST<<endl;
            break;
        }
        case 2:{
            showStations();
            int a,b,d,c; double f;
            cout<<"  From (ID): ";cin>>a; cout<<"  To (ID): ";cin>>b;
            cout<<"  Distance (km): ";cin>>d; cout<<"  Fare (₹): ";cin>>f;
            cout<<"  Capacity: ";cin>>c;
            addRoute(a,b,d,f,c);
            cout<<GRN<<"  ✓ Route added!"<<RST<<endl;
            break;
        }
        case 3: showNetwork(); break;
        case 4: showStations(); break;
        case 5:{
            showStations();
            string nm,dt; int s,d; char v;
            cout<<"  Name: ";cin.ignore();getline(cin,nm);
            cout<<"  From (ID): ";cin>>s; cout<<"  To (ID): ";cin>>d;
            cout<<"  Date (YYYY-MM-DD): ";cin>>dt;
            cout<<"  VIP? (y/n): ";cin>>v;
            bookSeat(nm,s,d,v=='y'||v=='Y',dt);
            break;
        }
        case 6:{
            int pnr; cout<<"  PNR to cancel: ";cin>>pnr;
            cancelBooking(pnr);
            break;
        }
        case 7:{
            showStations();
            int s,d; cout<<"  From (ID): ";cin>>s; cout<<"  To (ID): ";cin>>d;
            cascadeCancel(s,d);
            break;
        }
        case 8: showBookings(); break;
        case 9: showWaitlist(); break;
        case 10: showRefunds(); break;
        case 11:{
            showStations();
            int s,d; cout<<"  From (ID): ";cin>>s; cout<<"  To (ID): ";cin>>d;
            dijkstra(s,d);
            break;
        }
        case 12:{
            showStations();
            int s,d; cout<<"  From (ID): ";cin>>s; cout<<"  To (ID): ";cin>>d;
            leastCrowded(s,d);
            break;
        }
        case 13: primsMST(); break;
        case 14:{
            showStations();
            int s,d; cout<<"  From (ID): ";cin>>s; cout<<"  To (ID): ";cin>>d;
            Edge* e=findEdge(s,d);
            if(!e){cout<<RED<<"  No route."<<RST<<endl;break;}
            double base=e->fare, dyn=dynPrice(s,d);
            double occ=(double)e->booked/e->cap*100;
            cout<<CYN<<"\n  Route: "<<sName(s)<<" → "<<sName(d)<<RST<<endl;
            cout<<"  Occupancy: "<<e->booked<<"/"<<e->cap<<" ("<<fixed<<setprecision(1)<<occ<<"%)"<<endl;
            cout<<"  Base Fare: ₹"<<base<<endl;
            cout<<"  Dynamic Fare: ₹"<<dyn;
            if(dyn>base)cout<<RED<<" (SURGE "<<(dyn/base)<<"x)"<<RST;
            else cout<<GRN<<" (Normal)"<<RST;
            cout<<endl;
            break;
        }
        case 15: sortAndShow(); break;
        case 16:{
            double lo,hi;
            cout<<"  Min fare (₹): ";cin>>lo; cout<<"  Max fare (₹): ";cin>>hi;
            searchFareRange(lo,hi);
            break;
        }
        case 17:{
            showStations();
            int s,d; cout<<"  Station (ID): ";cin>>s;
            cout<<"  Delay (minutes): ";cin>>d;
            bfsDelay(s,d);
            break;
        }
        case 18:{
            int k; cout<<"  Top-K routes: ";cin>>k;
            revDashboard(k);
            break;
        }
        case 0: cout<<GRN<<BLD<<"\n  Goodbye! 🚂\n"<<RST; break;
        default: cout<<RED<<"  Invalid choice."<<RST<<endl;
        }
    }while(ch!=0);
    return 0;
}
