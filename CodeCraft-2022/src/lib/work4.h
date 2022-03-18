namespace work4{
    int work(const vector<string> & clientid, const vector<string> & serverid, 
            const vector<vector<int>> &demand, const vector<vector<int>> &qos, const vector<int> &bandwidth,
            const vector<double> &sdemand, const vector<double> &sum, const vector<double> &out, const vector<int> &deg, 
            const int n, const int m, const int T, const int qos_constraint, vector<double> &rate, const double train_rate = 1.2,  
            const int seed = 131131){
        
        vector<int> p(n), q(m), carry(n);
        for (int i = 0; i < n; i++) p[i] = i;
        for (int i = 0; i < m; i++) q[i] = i;
        vector<vector<int> > K(n);
        
        ofstream fout("/output/solution.txt"); 

        const int Limit = T - (T * 95 + 99) / 100;
        for (int t = 0; t < T; t++) {
            int d = 0;
            for (int i = 0; i < m; i++) d += demand[t][i];
            random_shuffle(q.begin(), q.end());
            tol = 2;
            memset(head, -1, sizeof(head));
            for (int i = 0; i < n; i++) addedge(0, i + 1, min((int)(out[i] * rate[i]), bandwidth[i]), 0);
            for (int i = 0; i < n; i++)
                for (int j = 0; j < m; j++)
                    if (qos[i][q[j]] < qos_constraint) addedge(i + 1, n + q[j] + 1, INF, 0);
            for (int i = 0; i < m; i++)
                addedge(n + i + 1, n + m + 1, demand[t][i], 0);
            int maxflow = dinic(0, n + m + 1, 10);
            vector<double> res(m), resout(n);
            vector<int> vis(n);
            while (maxflow < d){
                for (int i = 0; i < m; i++) res[i] = 0;
                for (int i = 0; i < n; i++) resout[i] = 0;
                for (int u = n + 1; u <= n + m; u++) {
                    for (int i = head[u]; ~i; i = e[i].nex) {
                        if (e[i].to == n + m + 1 && e[i ^ 1].cap > 0) res[u - n - 1] += e[i ^ 1].cap;
                    }
                }
                for (int i = 0; i < m; i++) res[i] = (demand[t][i] - res[i]) / sdemand[i] / deg[i];
                for (int i = 0; i < n; i++)
                    for (int j = 0; j < m; j++)
                        if (qos[i][j] < qos_constraint) resout[i] += res[j];
                sort(p.begin(), p.end(), [&](int x, int y){
                    return resout[x] > resout[y];
                });
                int sel = -1;
                for (int i = 0; i < n; i++) if (carry[p[i]] < Limit && !vis[p[i]]) {
                    sel = p[i];
                    carry[p[i]]++;
                    break;
                }
                if (sel == -1){
                    rate[p[0]] *= train_rate;
                    return -1;
                }
                assert(sel >= 0);
                vis[sel] = 1;
                e[2 + (sel << 1)].cap += max(0, bandwidth[sel] - (int)(out[sel] * rate[sel]));
                maxflow += dinic(0, n + m + 1, 10);
            }

            vector<int> sumwidth(n);
            
            for (int u = n + 1; u <= n + m; u++) {
                fout << clientid[u - n - 1] << ":";
                int cnt = 0;
                for (int i = head[u]; ~i; i = e[i].nex) {
                    if (e[i].to >= 1 && e[i].to <= n && e[i].cap > 0) {
                        if (cnt > 0) fout << ",";
                        fout << "<" << serverid[e[i].to - 1] << "," << e[i].cap << ">";
                        cnt++;  
                        sumwidth[e[i].to - 1] += e[i].cap;
                    }
                }
                if (!(t == T - 1 && u == n + m)) fout << endl;
            }
        

            for (int i = 0; i < n; i++) K[i].push_back(sumwidth[i]);
            assert(maxflow == d);
        }

        fout.close();
        #ifdef DEBUG
            int ans = 0;
            int w95 = (T * 95 + 99) / 100;
            for (int i = 0; i < n; i++){
                sort(K[i].begin(), K[i].end());
                //for (int j = 0; j < T; j++) printf("%d ", K[i][j]);
                //puts("");
                ans += K[i][w95 - 1];
            }
            cout << ans << endl;
        #endif
        
        return 1;
    }
}