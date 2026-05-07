#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <climits>
#include <iomanip>
#include <queue>
#include <algorithm>
using namespace std;

// ANSI Colors
const string RST="\033[0m", BLD="\033[1m", RED="\033[31m", GRN="\033[32m";
const string YLW="\033[33m", BLU="\033[34m", MAG="\033[35m", CYN="\033[36m";

// ── Structs ──
struct StationNode { int id; string name; StationNode* next; };
struct Edge { int dest, dist, cap, booked; double fare; };
struct Booking { int pnr, src, dest; string name, date; bool vip, active; double fare; };
struct WaitEntry { string name, date; int src, dest, ts; bool vip; };
struct Refund { int pnr; string name; double amt; };
struct RevEntry { int src, dest, cnt; double rev; };

// ── Globals ──
const int MX=50, MXB=500, MXW=100, MXR=200;
StationNode* sHead=nullptr;
int sCnt=0;
vector<vector<Edge>> G;
Booking bk[MXB]; int bkCnt=0, nxtPNR=1001;
WaitEntry wq[MXW]; int wqSz=0;
Refund rfStk[MXR]; int rfTop=-1;
RevEntry rvH[MX*MX]; int rvSz=0;
int gTS=0;

// ── Station Linked List (P1) ──
void addStation(string nm){
    StationNode* n=new StationNode{sCnt,nm,nullptr};
    if(!sHead){sHead=n;}
    else{StationNode* t=sHead;while(t->next)t=t->next;t->next=n;}
    G.push_back(vector<Edge>());
    sCnt++;
}
string sName(int id){
    StationNode* t=sHead;
    while(t){if(t->id==id)return t->name;t=t->next;}
    return "?";
}
int sId(string nm){
    StationNode* t=sHead;
    while(t){if(t->name==nm)return t->id;t=t->next;}
    return -1;
}
void showStations(){
    cout<<CYN<<BLD<<"\n╔══ Stations ══╗"<<RST<<endl;
    StationNode* t=sHead;
    while(t){cout<<"  "<<t->id<<". "<<t->name<<endl;t=t->next;}
}

// ── Graph / Routes (P1+P4) ──
void addRoute(int a,int b,int d,double f,int c){
    G[a].push_back({b,d,c,0,f});
    G[b].push_back({a,d,c,0,f});
}
Edge* findEdge(int a,int b){
    for(auto& e:G[a])if(e.dest==b)return &e;
    return nullptr;
}
void showNetwork(){
    cout<<CYN<<BLD<<"\n╔══ Railway Network ══╗"<<RST<<endl;
    for(int i=0;i<sCnt;i++){
        cout<<GRN<<"  "<<sName(i)<<RST<<endl;
        for(auto& e:G[i])
            cout<<"    → "<<sName(e.dest)<<" | "<<e.dist<<"km | ₹"
                <<e.fare<<" | "<<e.booked<<"/"<<e.cap<<endl;
    }
}

// ── Dynamic Pricing - Greedy (P3) ──
double dynPrice(int a,int b){
    Edge* e=findEdge(a,b);
    if(!e)return 0;
    double occ=(double)e->booked/e->cap;
    if(occ>0.9)return e->fare*2.0;
    if(occ>0.8)return e->fare*1.5;
    return e->fare;
}

// ── Binary Search on Fares (P1) ──
void searchFareRange(double lo,double hi){
    vector<pair<double,pair<int,int>>> fares;
    for(int i=0;i<sCnt;i++)
        for(auto& e:G[i])
            if(e.dest>i) fares.push_back({e.fare,{i,e.dest}});
    sort(fares.begin(),fares.end());
    // binary search for lower bound
    int l=0,r=fares.size()-1,lb=fares.size();
    while(l<=r){int m=(l+r)/2;if(fares[m].first>=lo){lb=m;r=m-1;}else l=m+1;}
    cout<<YLW<<BLD<<"\n Routes with fare ₹"<<lo<<" - ₹"<<hi<<":"<<RST<<endl;
    bool found=false;
    for(int i=lb;i<(int)fares.size()&&fares[i].first<=hi;i++){
        cout<<"  "<<sName(fares[i].second.first)<<" ↔ "
            <<sName(fares[i].second.second)<<" : ₹"<<fares[i].first<<endl;
        found=true;
    }
    if(!found)cout<<RED<<"  No routes found."<<RST<<endl;
}
