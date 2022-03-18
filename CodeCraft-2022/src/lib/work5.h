namespace work5{
    int work(const vector<string> & clientid, const vector<string> & serverid, 
            const vector<vector<int>> &demand, const vector<vector<int>> &qos, const vector<int> &bandwidth, 
            const int n, const int m, const int T, const int qos_constraint, const double W = 1.1, const int seed = 131131){
        
        vector<double> sdemand(m), out(n);
        for (int t = 0; t < T; t++)
            for (int i = 0; i < m; i++)
                sdemand[i] += demand[t][i];
        for (int i = 0; i < m; i++) sdemand[i] = sdemand[i] / T;
        
        for (int i = 0; i < m; i++) {
            double sum = 0;
            for (int j = 0; j < n; j++)
                if (qos[j][i] < qos_constraint) sum += bandwidth[j];
            for (int j = 0; j < n; j++)
                if (qos[j][i] < qos_constraint) out[j] += sdemand[i] * (bandwidth[j] / sum);
        }

        vector<int> p(n), q(m);
        for (int i = 0; i < n; i++) p[i] = i;
        for (int i = 0; i < m; i++) q[i] = i;
        vector<vector<int> > K(n);
        
        ofstream fout("/output/solution.txt");  
        const int Limit = T - (T * 95 + 99) / 100;
        for (int t = 0; t < T; t++) {
            random_shuffle(p.begin(), p.end());
            random_shuffle(q.begin(), q.end());
            tol = 2;
            memset(head, -1, sizeof(head));
            for (int i = 0; i < n; i++) addedge(0, p[i] + 1, min((int)(out[p[i]] / W), bandwidth[p[i]]), 0);
            for (int i = 0; i < n; i++)
                for (int j = 0; j < m; j++)
                    if (qos[i][q[j]] < qos_constraint) addedge(i + 1, n + q[j] + 1, INF, 0);
            for (int i = 0; i < m; i++)
                addedge(n + i + 1, n + m + 1, demand[t][i], 0);
            int maxflow = dinic(0, n + m + 1, 10);
            for (int i = 0; i < n; i++) e[2 + (i << 1)].cap += max(0, bandwidth[p[i]] - (int)(out[p[i]] / W));
            maxflow += dinic(0, n + m + 1, 10);

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
            int d = 0;
            for (int i = 0; i < m; i++) d += demand[t][i];
            assert(maxflow == d);
            //cout << "flow: " << maxflow + maxflow2 << '/' << d << endl;
           
        }

        fout.close();
        int ans = 0;
        int w95 = (T * 95 + 99) / 100;
        for (int i = 0; i < n; i++){
            sort(K[i].begin(), K[i].end());
            #ifdef DEBUG
                for (int j = 0; j < T; j++) printf("%d ", K[i][j]);
                puts("");
            #endif
            ans += K[i][w95 - 1];
        }
        cout << ans << endl;
        return ans;
    }
}