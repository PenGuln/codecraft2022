#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>
#include <cassert>
#include "config.h"
//#define DEBUG
#include "work.h"
using namespace std;
const string PATH = "/data";
void getlines(vector<string> &lines, string Path){
    lines.clear();
    ifstream inFile(Path, ios::in);
    string line;
    while (getline(inFile, line)) {
        lines.push_back(line.substr(0, line.length() - 1));
    }
    inFile.close();
}
void output(const vector<string> &mtime, vector<pair<int, int> > *distb, 
            const vector<string> &clientid, const vector<string> &serverid, const vector<string> &streamid, 
            int n, int m, int T) {
    vector<int> stmlist[n];
    vector<pair<int, int> >::iterator ps[m];
    ofstream fout("/output/solution.txt"); 
    for (int i = 0; i < m; i++) ps[i] = distb[i].begin();
    for (int t = 0; t < T; t++) {
        while (t + 1 < T && mtime[t + 1] == mtime[t]) t++;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) stmlist[j].clear();
            while (ps[i] != distb[i].end() && ps[i]->first <= t) stmlist[ps[i]->second].push_back(ps[i]->first), ps[i]++;
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
    }
    fout.close();
}
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

    for (int i = 0; i < lines.size(); i++) {
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
    for (int i = 0; i < lines.size(); i++) {
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
    for (int i = 1; i < lines.size(); i++) {
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
    int realT = 1;
    //base_cost = max(base_cost, 10);
    for (int t = 1; t < T; t++) {
        if (mtime[t] != mtime[t - 1]) realT++;
    }
    const int Limit = realT - (realT * 95 + 99) / 100;
    vector<int> carry(n), cap(n), p(n);
    vector<pair<int, int> > distb[m];
    bool okay = false;
    for (int i = 0; i < n; i++) cap[i] = base_cost, p[i] = i;
    while (!okay){
        okay = true;
        for (int i = 0; i < n; i++) carry[i] = 0;
        
        #ifdef DEBUG
            for (int i = 0; i < n; i++) cout << cap[i] << ' ';
            cout << endl;
        #endif

        for (int i = 0; i < m; i++) distb[i].clear();
        for (int t = 0; t < T; t++) {
            int l = t;
            while (t + 1 < T && mtime[t + 1] == mtime[t]) t++;
            int re = work::solve(demand, qos, bandwidth, carry, distb, cap, p, n, m, l, t, Limit, qos_constraint, base_cost); 
            if (re == -1) {
                okay = false;
                break;
            }
        }
        if (okay){
            double capsum = 0;
            for (auto it = p.begin(); it != p.end();) {
                capsum += cap[*it];
                if (cap[*it] < base_cost * 0.8) okay = false, p.erase(it);
                else ++it;
            }
        }
    }
    output(mtime, distb, clientid, serverid, streamid, n, m, T);
    
}