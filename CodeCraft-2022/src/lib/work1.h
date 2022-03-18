namespace work1{
    void work(vector<string> & clientid, vector<string> & serverid, 
            vector<vector<int>> &demand, vector<vector<int>> &qos, vector<int> &bandwidth, 
            const int n, const int m, const int T, const int qos_constraint){
        vector<int> p(n), flag(n);

        #ifdef DEBUG    
            vector<vector<int> > K(n);
        #endif
        
        ofstream fout("/output/solution.txt");  
        
        for (int t = 0; t < T; t++) {
            for (int i = 0; i < n; i++) {
                p[i] = i;
                flag[i] = rand() % 1009;
            }
            random_shuffle(p.begin(), p.end());
            tol = 2;
            memset(head, -1, sizeof(head));
            for (int i = 0; i < n; i++) if (flag[p[i]] > 50) addedge(0, p[i] + 1, bandwidth[p[i]], 0);
            for (int i = 0; i < n; i++) if (flag[p[i]] <= 50) addedge(0, p[i] + 1, bandwidth[p[i]], 0);
            for (int i = 0; i < n; i++)
                for (int j = 0; j < m; j++)
                    if (qos[i][j] < qos_constraint) addedge(i + 1, n + j + 1, INF, 0);

            for (int i = 0; i < m; i++)
                addedge(n + i + 1, n + m + 1, demand[t][i], 0);
            
            int maxflow = dinic(0, n + m + 1, INF);

            #ifdef DEBUG    
                vector<int> sumwidth(n);
            #endif

            for (int u = n + 1; u <= n + m; u++) {
                fout << clientid[u - n - 1] << ":";
                int cnt = 0;
                for (int i = head[u]; ~i; i = e[i].nex) {
                    if (e[i].to >= 1 && e[i].to <= n && e[i].cap > 0) {
                        if (cnt > 0) fout << ",";
                        fout << "<" << serverid[e[i].to - 1] << "," << e[i].cap << ">";
                        cnt++;
                        #ifdef DEBUG    
                            sumwidth[e[i].to - 1] += e[i].cap;
                        #endif
                    }
                }
                if (!(t == T - 1 && u == n + m)) fout << endl;
            }

            #ifdef DEBUG    
                for (int i = 0; i < n; i++) K[i].push_back(sumwidth[i]);    
                cout << "flow: " << maxflow << endl;
            #endif
        }
        fout.close();    
        #ifdef DEBUG
            int ans = 0;
            int w95 = (T * 95 + 99) / 100;
            for (int i = 0; i < n; i++){
                sort(K[i].begin(), K[i].end());
                ans += K[i][w95 - 1];
            }
            cout << ans << endl;
        #endif
    }
}