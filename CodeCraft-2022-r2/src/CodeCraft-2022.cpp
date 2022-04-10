#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <cstring>
#include <algorithm>
#include <cassert>
#include "config.h"
#include "work.h"
//#define DEBUG
using namespace std;
const double eps = 1e-8;
#ifdef DEBUG
const string PATH = "data";
#else
const string PATH = "/data";
#endif
void getlines(vector<string> &lines, string Path){
    lines.clear();
    ifstream inFile(Path, ios::in);
    string line;
    while (getline(inFile, line)) {
        lines.push_back(line.substr(0, line.length() - 1));
    }
    inFile.close();
}
void output(const vector<vector<int>>& demand, const vector<int>& bandwidth, 
            const vector<string> &mtime, vector<vector<pair<int, int> >> & distb, 
            const vector<string> &clientid, const vector<string> &serverid, const vector<string> &streamid, 
            const int n, const int m, const int T, const int realT, const int base_cost) {
    vector<vector<int>> stmlist(n);
    vector<pair<int, int> >::iterator ps[m];
    vector<int> out(n);
    vector<vector<int> > K(n);
    ofstream fout("/output/solution.txt"); 
    for (int i = 0; i < m; i++) ps[i] = distb[i].begin();
    vector<int> p(n);
    for (int i = 0; i < n; i++) p[i] = i;
    random_shuffle(p.begin(), p.end());
    for (int i = 0; i < 10; i++) fout << serverid[p[i]] << ",\n"[i == 9];
    #ifdef DEBUG
        vector<int> ck(m);
        for (int t = 0; t < T; t++) 
            for (int i = 0; i < m; i++) if (demand[t][i] > 0) ck[i]++;
        for (int i = 0; i < m; i++) assert(ck[i] == (int)distb[i].size());
    #endif
    
    for (int t = 0; t < T; t++) {
        while (t + 1 < T && mtime[t + 1] == mtime[t]) t++;
        for (int i = 0; i < n; i++) out[i] = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) stmlist[j].clear();
            while (ps[i] != distb[i].end() && ps[i]->first <= t) {
                stmlist[ps[i]->second].push_back(ps[i]->first);
                out[ps[i]->second] += demand[ps[i]->first][i];
                ++ps[i];
            }
            fout << clientid[i] << ":";
            int cnt = 0;
            for (int j = 0; j < n; j++) {
                if (!stmlist[j].empty()) {
                    if (cnt > 0) fout << ',';
                    fout << '<' << serverid[j];
                    for (int stmid : stmlist[j]) fout << ',' << streamid[stmid];
                    fout << '>';
                    cnt++;
                }
            }
            if (!(t == T - 1 && i == m - 1)) fout << '\n';
        }
        for (int i = 0; i < n; i++) K[i].push_back(out[i]);
    }
    fout.close();

    #ifdef DEBUG
        double ans = 0;
        int w95 = (realT * 95 + 99) / 100;
        for (int i = 0; i < n; i++){
            sort(K[i].begin(), K[i].end());
            long long sum = 0;
            for (int x : K[i]) sum += x;
            if (sum > 0) {
                int W = K[i][w95 - 1];
                ans += (W <= base_cost) ? base_cost : (1.0 * (W - base_cost) * (W - base_cost) / bandwidth[i] + W);
            }
        }
        cout << ans << endl;
    #endif
}
unordered_map<int, int> vis[900000];
int main(){
    ios::sync_with_stdio(false);
    string str;
    vector<string> lines;
    getlines(lines, PATH + "/demand.csv");
    const int T = lines.size() - 1;
    //cout << T << endl;
    vector<string> clientid;
    vector<vector<int>> demand(T);
    vector<string> mtime(T), streamid(T);

    for (int i = 0; i <= T; i++) {
        istringstream sin(lines[i]);
        int id = 0;
        if (i == 0) {
            while (getline(sin, str, ',')) {
                if (id > 1) clientid.push_back(str);
                id++;
            }
        }else{
            while (getline(sin, str, ',')) {
                if (id < 1) mtime[i - 1] = str;
                else if (id == 1) streamid[i - 1] = str;
                else if (id > 1) demand[i - 1].push_back(atoi(str.c_str()));
                id++;
            }
        }
    }
    const int m = clientid.size();
    map<string, int>clientidmap;
    for (int i = 0; i < m; i++) clientidmap[clientid[i]] = i;

    getlines(lines, PATH + "/qos.csv");
    const int n = lines.size() - 1;
    vector<string> serverid;
    vector<vector<int>> qos(n);
    vector<int> place(m);
    for (int i = 0; i <= n; i++) {
        istringstream sin(lines[i]);
        int id = 0;
        if (i == 0){
            while (getline(sin, str, ',')) {
                if (id > 0) assert(clientidmap.count(str)), place[id - 1] = clientidmap[str];
                id++;
            }
        }else{
            qos[i - 1].resize(m);
            while (getline(sin, str, ',')) {
                if (id == 0) serverid.push_back(str);
                else qos[i - 1][place[id - 1]] = atoi(str.c_str());
                id++;
            }
        }
    }
    map<string, int>serveridmap;
    for (int i = 0; i < n; i++) serveridmap[serverid[i]] = i;

    getlines(lines, PATH + "/site_bandwidth.csv");
    vector<int> bandwidth(n);
    for (int i = 1; i < (int)lines.size(); i++) {
        istringstream sin(lines[i]);
        int id = 0, pos = 0;
        while (getline(sin, str, ',')) {
            if (id == 0) assert(serveridmap.count(str)), pos = serveridmap[str];
            else bandwidth[pos] = atoi(str.c_str());
            id++;
        }
    }
    Config config(PATH + "/config.ini");
    int qos_constraint = config.Read("qos_constraint", 400);
    int base_cost = config.Read("base_cost", 0);
    vector<pair<int, int>> intval;
    for (int t = 0; t < T; t++) {
        int l = t;
        while (t + 1 < T && mtime[t + 1] == mtime[t]) t++;
        intval.emplace_back(l, t);
    }
    const int realT = intval.size(), Limit = realT - (realT * 95 + 99) / 100;

    vector<vector<int>>g(n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            if (qos[i][j] < qos_constraint) g[i].push_back(j);
    vector<int> deg(m);
    vector<double> sdemand(m), esb(n);
    for (int t = 0; t < T; t++)
        for (int i = 0; i < m; i++)
            sdemand[i] += demand[t][i];
    for (int i = 0; i < m; i++) sdemand[i] /= realT;
    for (int i = 0; i < n; i++) for (int j : g[i]) deg[j]++;
    for (int i = 0; i < n; i++) for (int j : g[i]) esb[i] += sdemand[j] / deg[j];

    vector<int> p(n);
    for (int i = 0; i < n; i++) p[i] = i;
    random_shuffle(p.begin(), p.end());
    vector<vector<int> >carry(realT);
    for (auto it = p.begin(); it != p.end(); ) {
        int s = *it;
        vector<pair<double, int>> bests;
        for (int k = 0; k < realT; k++) {
            double ws = 0;
            for (int c : g[s])
                for (int t = intval[k].first; t <= intval[k].second; t++) 
                    if (!vis[t].count(c))
                        ws += 1.0 * demand[t][c] / deg[c];
            bests.push_back(make_pair(ws, k));
        }
        sort(bests.rbegin(), bests.rend());
        if (bests[0].first < base_cost * 0.0 - eps) p.erase(it); // here you can delete those servers which are ignorable
        else{
            for (int i = 0; i < Limit; i++) if (bests[i].first > eps){
                int k = bests[i].second;
                vector<pair<int, pair<int, int>> > can;
                for (int c : g[s])
                    for (int t = intval[k].first; t <= intval[k].second; t++) 
                        if (!vis[t].count(c)) can.push_back(make_pair(demand[t][c], make_pair(t, c)));
                sort(can.rbegin(), can.rend());
                int tot = 0;
                for (auto x : can){
                    int wt = x.first;
                    auto pr = x.second;
                    if (tot + wt <= bandwidth[s]) tot += wt, vis[pr.first][pr.second] = true;
                }
                carry[bests[i].second].push_back(s);
            }
            it++;
        }
    }
    vector<int> cap(n);
    vector<long long> tot(n);
    vector<vector<pair<int, int> > > distb(m);
    vector<vector<int>> load(n);
    for (int i = 0; i < n; i++) load[i].resize(realT);
    bool okay = false;
    for (int i = 0; i < n; i++) cap[i] = base_cost;
    for (int iteration = 0; ; iteration++){
        okay = true;
        for (int i = 0; i < n; i++) tot[i] = 0;
    
        #ifdef DEBUG
            for (int i = 0; i < n; i++) cout << cap[i] << ' ';
            cout << endl;
        #endif

        for (int i = 0; i < m; i++) distb[i].clear();
        for (int t = 0; t < realT; t++) {
            int re = work::solve(demand, bandwidth, qos, carry[t], distb, cap, p, tot, n, m, intval[t].first, intval[t].second, Limit, qos_constraint, base_cost); 
            if (re == -1) okay = false;
        }
        if (okay && iteration >= 5) break;

        //clear carry and solve
        for (int i = 0; i < n; i++) cap[i] = cap[i] * 0.8;
        for (int t = 0; t < realT; t++) {
            carry[t].clear();
            work::solve_record_load(demand, bandwidth, qos, carry[t], cap, p, load, n, m, t, intval[t].first, intval[t].second, Limit, qos_constraint, base_cost);
        }

        //recalculate carry
        random_shuffle(p.begin(), p.end());
        for (auto it = p.begin(); it != p.end(); ) {
            int s = *it;
            vector<pair<int, int> > tmp(realT);
            for (int i = 0; i < realT; i++) tmp[i] = make_pair(load[s][i], i);
            sort(tmp.rbegin(), tmp.rend());
            for (int i = 0; i < Limit; i++) {
                int ti = tmp[i].second;
                carry[ti].push_back(s);
                work::solve_record_load(demand, bandwidth, qos, carry[ti], cap, p, load, n, m, ti, intval[ti].first, intval[ti].second, Limit, qos_constraint, base_cost);
                //recalculate server load after allocating one carry
            }
            it++;
        }
    }
    output(demand, bandwidth, mtime, distb, clientid, serverid, streamid, n, m, T, realT, base_cost);

}