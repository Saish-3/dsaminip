#pragma once
#include "defs.h"

// ══════════════════════════════════════
// WAITLIST - Priority Queue Manual (P2)
// ══════════════════════════════════════
bool wPri(WaitEntry& a,WaitEntry& b){
    if(a.vip!=b.vip)return a.vip;
    return a.ts<b.ts;
}
void wqInsert(WaitEntry e){
    wq[wqSz]=e; int i=wqSz; wqSz++;
    while(i>0){int p=(i-1)/2;if(wPri(wq[i],wq[p])){swap(wq[i],wq[p]);i=p;}else break;}
}
WaitEntry wqExtract(){
    WaitEntry top=wq[0]; wqSz--; wq[0]=wq[wqSz]; int i=0;
    while(true){
        int l=2*i+1,r=2*i+2,b=i;
        if(l<wqSz&&wPri(wq[l],wq[b]))b=l;
        if(r<wqSz&&wPri(wq[r],wq[b]))b=r;
        if(b!=i){swap(wq[i],wq[b]);i=b;}else break;
    }
    return top;
}
WaitEntry* wqFindRoute(int s,int d){
    for(int i=0;i<wqSz;i++)if(wq[i].src==s&&wq[i].dest==d)return &wq[i];
    return nullptr;
}

// ══════════════════════════════════════
// REFUND STACK (P2)
// ══════════════════════════════════════
void rfPush(int pnr,string nm,double a){rfTop++;rfStk[rfTop]={pnr,nm,a};}
void showRefunds(){
    cout<<MAG<<BLD<<"\n╔══ Refund History (Stack - LIFO) ══╗"<<RST<<endl;
    if(rfTop<0){cout<<"  Empty."<<endl;return;}
    for(int i=rfTop;i>=0;i--)
        cout<<"  PNR:"<<rfStk[i].pnr<<" "<<rfStk[i].name<<" ₹"<<rfStk[i].amt<<endl;
}

// ══════════════════════════════════════
// BOOKING & CANCELLATION (P2)
// ══════════════════════════════════════
void bookSeat(string nm,int s,int d,bool vip,string dt){
    Edge* e=findEdge(s,d);
    if(!e){cout<<RED<<"  No direct route!"<<RST<<endl;return;}
    double price=dynPrice(s,d);
    if(e->booked<e->cap){
        bk[bkCnt]={nxtPNR++,s,d,nm,dt,vip,true,price};
        e->booked++;
        Edge* rev=findEdge(d,s);if(rev)rev->booked++;
        cout<<GRN<<BLD<<"  ✓ Booked! PNR: "<<bk[bkCnt].pnr<<" | Fare: ₹"<<price;
        if(price>e->fare)cout<<RED<<" (SURGE)"<<RST;
        cout<<RST<<endl; bkCnt++;
    }else{
        WaitEntry w={nm,dt,s,d,gTS++,vip};
        wqInsert(w);
        cout<<YLW<<"  ⏳ Full! Added to waitlist ("<<(vip?"VIP":"Regular")<<")"<<RST<<endl;
    }
}
void cancelBooking(int pnr){
    for(int i=0;i<bkCnt;i++){
        if(bk[i].pnr==pnr&&bk[i].active){
            bk[i].active=false;
            rfPush(pnr,bk[i].name,bk[i].fare);
            Edge* e=findEdge(bk[i].src,bk[i].dest);
            if(e)e->booked--;
            Edge* rev=findEdge(bk[i].dest,bk[i].src);if(rev)rev->booked--;
            cout<<GRN<<"  ✓ PNR "<<pnr<<" cancelled. Refund ₹"<<bk[i].fare<<" pushed to stack."<<RST<<endl;
            // Auto-promote from waitlist
            if(wqSz>0){
                // Find someone for this route
                for(int j=0;j<wqSz;j++){
                    if(wq[j].src==bk[i].src&&wq[j].dest==bk[i].dest){
                        WaitEntry w=wq[j];
                        // Remove from waitlist by shifting
                        for(int k=j;k<wqSz-1;k++)wq[k]=wq[k+1];
                        wqSz--;
                        cout<<CYN<<"  ↑ Auto-promoted "<<w.name<<" from waitlist!"<<RST<<endl;
                        bookSeat(w.name,w.src,w.dest,w.vip,w.date);
                        break;
                    }
                }
            }
            return;
        }
    }
    cout<<RED<<"  PNR not found."<<RST<<endl;
}
void cascadeCancel(int s,int d){
    cout<<RED<<BLD<<"\n  ⚠ Cancelling all bookings on "<<sName(s)<<" → "<<sName(d)<<RST<<endl;
    int cnt=0;
    for(int i=0;i<bkCnt;i++){
        if(bk[i].active&&bk[i].src==s&&bk[i].dest==d){
            bk[i].active=false;
            rfPush(bk[i].pnr,bk[i].name,bk[i].fare);
            cnt++;
        }
    }
    Edge* e=findEdge(s,d);if(e)e->booked=0;
    Edge* rev=findEdge(d,s);if(rev)rev->booked=0;
    cout<<"  Cancelled "<<cnt<<" bookings. Refunds pushed to stack."<<endl;
    // Remove waitlist entries for this route
    int j=0;
    for(int i=0;i<wqSz;i++)
        if(!(wq[i].src==s&&wq[i].dest==d))wq[j++]=wq[i];
    wqSz=j;
}
void showBookings(){
    cout<<BLU<<BLD<<"\n╔══ Active Bookings ══╗"<<RST<<endl;
    cout<<"  "<<left<<setw(6)<<"PNR"<<setw(16)<<"Name"<<setw(6)<<"VIP"
        <<setw(20)<<"Route"<<setw(12)<<"Date"<<"Fare"<<endl;
    cout<<"  "<<string(65,'-')<<endl;
    for(int i=0;i<bkCnt;i++){
        if(!bk[i].active)continue;
        cout<<"  "<<left<<setw(6)<<bk[i].pnr<<setw(16)<<bk[i].name
            <<setw(6)<<(bk[i].vip?"Y":"N")<<setw(20)
            <<(sName(bk[i].src)+"→"+sName(bk[i].dest))
            <<setw(12)<<bk[i].date<<"₹"<<bk[i].fare<<endl;
    }
}
void showWaitlist(){
    cout<<YLW<<BLD<<"\n╔══ Waitlist (Priority Queue) ══╗"<<RST<<endl;
    if(wqSz==0){cout<<"  Empty."<<endl;return;}
    for(int i=0;i<wqSz;i++)
        cout<<"  "<<wq[i].name<<" | "<<sName(wq[i].src)<<"→"<<sName(wq[i].dest)
            <<" | "<<(wq[i].vip?"VIP":"Reg")<<" | T="<<wq[i].ts<<endl;
}

// ══════════════════════════════════════
// DIJKSTRA'S ALGORITHM (P3)
// ══════════════════════════════════════
void dijkstra(int src,int dst){
    vector<int>dist(sCnt,INT_MAX),par(sCnt,-1);
    vector<bool>vis(sCnt,false);
    dist[src]=0;
    // Simple O(V^2) Dijkstra
    for(int i=0;i<sCnt;i++){
        int u=-1;
        for(int j=0;j<sCnt;j++)if(!vis[j]&&(u==-1||dist[j]<dist[u]))u=j;
        if(u==-1||dist[u]==INT_MAX)break;
        vis[u]=true;
        for(auto& e:G[u]){
            if(!vis[e.dest]&&dist[u]+e.dist<dist[e.dest]){
                dist[e.dest]=dist[u]+e.dist;
                par[e.dest]=u;
            }
        }
    }
    if(dist[dst]==INT_MAX){cout<<RED<<"  No path exists."<<RST<<endl;return;}
    // Reconstruct path
    vector<int>path; int c=dst;
    while(c!=-1){path.push_back(c);c=par[c];}
    reverse(path.begin(),path.end());
    cout<<GRN<<BLD<<"\n  Shortest Path ("<<dist[dst]<<" km):"<<RST<<endl<<"  ";
    for(int i=0;i<(int)path.size();i++){
        cout<<sName(path[i]);if(i<(int)path.size()-1)cout<<" → ";
    }
    cout<<endl;
    // Show fare
    double total=0;
    for(int i=0;i<(int)path.size()-1;i++){
        Edge* e=findEdge(path[i],path[i+1]);
        if(e)total+=dynPrice(path[i],path[i+1]);
    }
    cout<<"  Total Fare: ₹"<<total<<endl;
}

// ── Least Crowded Route (Modified Dijkstra) (P3) ──
void leastCrowded(int src,int dst){
    vector<double>cost(sCnt,1e18);
    vector<int>par(sCnt,-1);
    vector<bool>vis(sCnt,false);
    cost[src]=0;
    for(int i=0;i<sCnt;i++){
        int u=-1;
        for(int j=0;j<sCnt;j++)if(!vis[j]&&(u==-1||cost[j]<cost[u]))u=j;
        if(u==-1||cost[u]>=1e18)break;
        vis[u]=true;
        for(auto& e:G[u]){
            double w=(e.cap>0)?(double)e.booked/e.cap:1.0;
            if(!vis[e.dest]&&cost[u]+w<cost[e.dest]){
                cost[e.dest]=cost[u]+w;
                par[e.dest]=u;
            }
        }
    }
    if(cost[dst]>=1e18){cout<<RED<<"  No path exists."<<RST<<endl;return;}
    vector<int>path;int c=dst;
    while(c!=-1){path.push_back(c);c=par[c];}
    reverse(path.begin(),path.end());
    cout<<CYN<<BLD<<"\n  Least Crowded Route:"<<RST<<endl<<"  ";
    for(int i=0;i<(int)path.size();i++){
        cout<<sName(path[i]);
        if(i<(int)path.size()-1){
            Edge* e=findEdge(path[i],path[i+1]);
            cout<<"("<<e->booked<<"/"<<e->cap<<") → ";
        }
    }
    cout<<endl;
}

// ══════════════════════════════════════
// PRIM'S MST (P3)
// ══════════════════════════════════════
void primsMST(){
    vector<int>key(sCnt,INT_MAX),par(sCnt,-1);
    vector<bool>inMST(sCnt,false);
    key[0]=0;
    cout<<MAG<<BLD<<"\n╔══ Minimum Spanning Tree (Prim's) ══╗"<<RST<<endl;
    int totalCost=0;
    for(int i=0;i<sCnt;i++){
        int u=-1;
        for(int j=0;j<sCnt;j++)if(!inMST[j]&&(u==-1||key[j]<key[u]))u=j;
        if(u==-1||key[u]==INT_MAX)break;
        inMST[u]=true; totalCost+=key[u];
        for(auto& e:G[u]){
            if(!inMST[e.dest]&&e.dist<key[e.dest]){
                key[e.dest]=e.dist;
                par[e.dest]=u;
            }
        }
    }
    cout<<"  Edges in MST:"<<endl;
    for(int i=1;i<sCnt;i++){
        if(par[i]!=-1){
            Edge* e=findEdge(par[i],i);
            cout<<"  "<<sName(par[i])<<" ↔ "<<sName(i)<<" : "<<e->dist<<"km, ₹"<<e->fare<<endl;
        }
    }
    cout<<GRN<<"  Total MST Cost: "<<totalCost<<" km"<<RST<<endl;
}

// ══════════════════════════════════════
// MERGE SORT - Multi-level (P4)
// ══════════════════════════════════════
bool bkCmp(Booking& a,Booking& b){
    if(a.src!=b.src)return a.src<b.src;
    if(a.date!=b.date)return a.date<b.date;
    return a.fare<b.fare;
}
void mgMerge(Booking a[],int l,int m,int r){
    int n1=m-l+1,n2=r-m;
    vector<Booking>L(n1),R(n2);
    for(int i=0;i<n1;i++)L[i]=a[l+i];
    for(int i=0;i<n2;i++)R[i]=a[m+1+i];
    int i=0,j=0,k=l;
    while(i<n1&&j<n2){if(bkCmp(L[i],R[j]))a[k++]=L[i++];else a[k++]=R[j++];}
    while(i<n1)a[k++]=L[i++];
    while(j<n2)a[k++]=R[j++];
}
void mergeSort(Booking a[],int l,int r){
    if(l<r){int m=l+(r-l)/2;mergeSort(a,l,m);mergeSort(a,m+1,r);mgMerge(a,l,m,r);}
}
void sortAndShow(){
    // Copy active bookings
    vector<Booking>act;
    for(int i=0;i<bkCnt;i++)if(bk[i].active)act.push_back(bk[i]);
    if(act.empty()){cout<<YLW<<"  No bookings."<<RST<<endl;return;}
    Booking* arr=act.data();
    mergeSort(arr,0,act.size()-1);
    cout<<BLU<<BLD<<"\n╔══ Sorted Bookings (Route→Date→Fare) ══╗"<<RST<<endl;
    cout<<"  "<<left<<setw(6)<<"PNR"<<setw(16)<<"Name"<<setw(20)<<"Route"<<setw(12)<<"Date"<<"Fare"<<endl;
    cout<<"  "<<string(58,'-')<<endl;
    for(auto& b:act)
        cout<<"  "<<left<<setw(6)<<b.pnr<<setw(16)<<b.name<<setw(20)
            <<(sName(b.src)+"→"+sName(b.dest))<<setw(12)<<b.date<<"₹"<<b.fare<<endl;
}

// ══════════════════════════════════════
// BFS DELAY PROPAGATION - Custom #1 (P4)
// ══════════════════════════════════════
void bfsDelay(int station,int delayMin){
    vector<int>delay(sCnt,-1);
    vector<bool>vis(sCnt,false);
    queue<int>q;
    q.push(station); vis[station]=true; delay[station]=delayMin;
    cout<<RED<<BLD<<"\n╔══ Delay Propagation from "<<sName(station)<<" ("<<delayMin<<" min) ══╗"<<RST<<endl;
    int level=0;
    while(!q.empty()){
        int sz=q.size();
        for(int i=0;i<sz;i++){
            int u=q.front();q.pop();
            double factor=(level==0)?1.0:(level==1)?0.7:(level==2)?0.5:0.3;
            delay[u]=(int)(delayMin*factor);
            cout<<"  "<<sName(u)<<" : "<<delay[u]<<" min delay"<<endl;
            for(auto& e:G[u]){
                if(!vis[e.dest]){vis[e.dest]=true;q.push(e.dest);}
            }
        }
        level++;
    }
    // Show affected bookings
    cout<<YLW<<"\n  Affected Bookings:"<<RST<<endl;
    for(int i=0;i<bkCnt;i++){
        if(bk[i].active&&delay[bk[i].src]>0)
            cout<<"    PNR "<<bk[i].pnr<<" ("<<bk[i].name<<") delayed by "<<delay[bk[i].src]<<" min"<<endl;
    }
}

// ══════════════════════════════════════
// REVENUE ANALYTICS - Max Heap Custom #2 (P4)
// ══════════════════════════════════════
void rvInsert(RevEntry e){
    rvH[rvSz]=e;int i=rvSz;rvSz++;
    while(i>0){int p=(i-1)/2;if(rvH[i].rev>rvH[p].rev){swap(rvH[i],rvH[p]);i=p;}else break;}
}
RevEntry rvExtract(){
    RevEntry top=rvH[0];rvSz--;rvH[0]=rvH[rvSz];int i=0;
    while(true){
        int l=2*i+1,r=2*i+2,b=i;
        if(l<rvSz&&rvH[l].rev>rvH[b].rev)b=l;
        if(r<rvSz&&rvH[r].rev>rvH[b].rev)b=r;
        if(b!=i){swap(rvH[i],rvH[b]);i=b;}else break;
    }
    return top;
}
void buildRevHeap(){
    rvSz=0;
    double rev[MX][MX]={};int cnt[MX][MX]={};
    for(int i=0;i<bkCnt;i++){
        if(bk[i].active){rev[bk[i].src][bk[i].dest]+=bk[i].fare;cnt[bk[i].src][bk[i].dest]++;}
    }
    for(int i=0;i<sCnt;i++)for(int j=0;j<sCnt;j++)
        if(rev[i][j]>0)rvInsert({i,j,cnt[i][j],rev[i][j]});
}
void revDashboard(int k){
    buildRevHeap();
    cout<<GRN<<BLD<<"\n╔══ Revenue Dashboard (Max-Heap) ══╗"<<RST<<endl;
    if(rvSz==0){cout<<"  No revenue data."<<endl;return;}
    cout<<YLW<<"  Top "<<min(k,rvSz)<<" Routes by Revenue:"<<RST<<endl;
    vector<RevEntry>tmp;
    int show=min(k,rvSz);
    for(int i=0;i<show;i++){
        RevEntry e=rvExtract();tmp.push_back(e);
        cout<<"  "<<(i+1)<<". "<<sName(e.src)<<"→"<<sName(e.dest)
            <<" | ₹"<<e.rev<<" | "<<e.cnt<<" bookings"<<endl;
    }
    // Show bottom-K (remaining)
    cout<<RED<<"\n  Underperforming Routes:"<<RST<<endl;
    vector<RevEntry>bottom;
    while(rvSz>0)bottom.push_back(rvExtract());
    reverse(bottom.begin(),bottom.end());
    for(int i=0;i<min(k,(int)bottom.size());i++)
        cout<<"  "<<(i+1)<<". "<<sName(bottom[i].src)<<"→"<<sName(bottom[i].dest)
            <<" | ₹"<<bottom[i].rev<<endl;
    if(bottom.empty())cout<<"  All routes are top performers!"<<endl;
}
