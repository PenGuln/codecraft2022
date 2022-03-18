namespace work2{
    void work(vector<string> & clientid, vector<string> & serverid, 
            vector<vector<int>> &demand, vector<vector<int>> &qos, vector<int> &bandwidth, 
            const int n, const int m, const int T, const int qos_constraint){
        vector<int> sdemand(m);
        for (int t = 0; t < T; t++)
            for (int i = 0; i < m; i++)
                sdemand[i] += demand[t][i];
        
        vector<long long> out(n);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++) if (qos[i][j] < qos_constraint) out[i] += sdemand[j];
        vector<int> p(n), carry(n), flag(n);
        for (int i = 0; i < n; i++) p[i] = i;
        
        #ifdef DEBUG    
            vector<vector<int> > K(n);
        #endif
        
        ofstream fout("/output/solution.txt");  
        const int Limit = T - (T * 95 + 99) / 100;
        for (int t = 0; t < T; t++) {
            random_shuffle(p.begin(), p.end());
            for (int i = 0; i < n; i++) flag[i] = rand() % 1009;
            tol = 2;
            memset(head, -1, sizeof(head));
            for (int i = 0; i < n; i++) if (flag[p[i]] > 50 || carry[p[i]] >= Limit) addedge(0, p[i] + 1, bandwidth[p[i]], 0);
            for (int i = 0; i < n; i++) if (flag[p[i]] <= 50 && carry[p[i]] < Limit) addedge(0, p[i] + 1, bandwidth[p[i]], 0), carry[p[i]]++;
            
            for (int i = 0; i < n; i++)
                for (int j = 0; j < m; j++)
                    if (qos[i][j] < qos_constraint) addedge(i + 1, n + j + 1, INF, 0);

            for (int i = 0; i < m; i++)
                addedge(n + i + 1, n + m + 1, demand[t][i], 0);
            
            int maxflow = dinic(0, n + m + 1, 10);

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
                for (int j = 0; j < T; j++) printf("%d ", K[i][j]);
                puts("");
                ans += K[i][w95 - 1];
            }
            cout << ans << endl;
        #endif
    }
}