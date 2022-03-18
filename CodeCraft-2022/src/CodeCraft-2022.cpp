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
    //cout << T << endl;
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

    getlines(lines, PATH + "/qos.csv");
    const int n = lines.size() - 1;
    vector<string> serverid;
    vector<vector<int>> qos(n);
    for (int i = 1; i < lines.size(); i++) {
        istringstream sin(lines[i]);
        int id = 0;
        while (getline(sin, str, ',')) {
            if (id == 0) serverid.push_back(str);
            else qos[i - 1].push_back(atoi(str.c_str()));
            id++;
        }
    }

    getlines(lines, PATH + "/site_bandwidth.csv");
    vector<int> bandwidth(n);
    for (int i = 1; i < lines.size(); i++) {
        istringstream sin(lines[i]);
        int id = 0;
        while (getline(sin, str, ',')) {
            if (id > 0) bandwidth[i - 1] = atoi(str.c_str());
            id++;
        }
    }
    Config config(PATH + "/config.ini");
    int qos_constraint = config.Read("qos_constraint", 400);
    vector<double> rate(n);
    for (int i = 0; i < n; i++) rate[i] = 0.2;
    vector<double> sdemand(m), sum(m), out(n);
    vector<int> deg(m);
    for (int t = 0; t < T; t++)
        for (int i = 0; i < m; i++)
            sdemand[i] += demand[t][i];
    for (int i = 0; i < m; i++) sdemand[i] /= T;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            if (qos[i][j] < qos_constraint) sum[j] += bandwidth[i], deg[j]++;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            if (qos[i][j] < qos_constraint) out[i] += sdemand[j] / deg[j];

    while (work4::work(clientid, serverid, demand, qos, bandwidth, sdemand, sum, out, deg, n, m, T, qos_constraint, rate, 1.15) == -1);
    
}