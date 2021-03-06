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
#include "networkflow.h"
//#define DEBUG
#include "work4.h"
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
int main(){
    string str;
    vector<string> lines;
    getlines(lines, PATH + "/demand.csv");
    const int T = lines.size() - 1;
    vector<string> clientid;
    vector<vector<int>> demand(T);
    for (int i = 0; i < lines.size(); i++) {
        istringstream sin(lines[i]);
        int id = 0;
        if (i == 0) {
            while (getline(sin, str, ',')) {
                if (id > 0) clientid.push_back(str);
                id++;
            }
        }else{
            while (getline(sin, str, ',')) {
                if (id > 0) demand[i - 1].push_back(atoi(str.c_str()));
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
    vector<double> rate(n);
    for (int i = 0; i < n; i++) rate[i] = 0.8;
    vector<double> sdemand(m), sum(m), out(n);
    vector<int> deg(m);
    for (int t = 0; t < T; t++)
        for (int i = 0; i < m; i++)
            sdemand[i] += demand[t][i];
    for (int i = 0; i < m; i++) sdemand[i] /= T;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            if (qos[i][j] < qos_constraint) sum[j] += bandwidth[i], deg[j]++;
    for (int i = 0; i < m; i++) if (deg[i] == 0) deg[i]++;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            if (qos[i][j] < qos_constraint) out[i] += sdemand[j] / deg[j];
    while (work4::work(clientid, serverid, demand, qos, bandwidth, sdemand, sum, out, deg, n, m, T, qos_constraint, rate, 1.2) == -1);
    
}