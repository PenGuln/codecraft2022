namespace work{
    struct Request{
        int val, t, c;
        double difficulty;
        bool operator < (const Request &b)const{
            return difficulty > b.difficulty;
        }
    };
    inline int F(int base, int bw, double rt){
        return (int)(base * rt);
    }
    int solve(const vector<vector<int>>& demand, const vector<vector<int>>& qos, const vector<int>& bandwidth,
              vector<int>& carry, vector<pair<int, int> > *distb, vector<int>& cap, vector<int>& p,
              const int n, const int m, const int l, const int r, const int Limit, const int qos_constraint, const int base_cost, const double train_rate = 0.5){
        const int pn = p.size();
        vector<int> ss(n), resout(n);
        vector<bool> vis(n);
        random_shuffle(p.begin(), p.end());
        vector<Request> req[2];
        for (int t = l; t <= r; t++) {
            for (int i = 0; i < m; i++) 
                if (demand[t][i] > 0) {
                    int cnt = 0;
                    for (int server : p) {
                        if (qos[server][i] < qos_constraint) cnt += cap[server] / demand[t][i];
                    }
                    req[0].push_back(Request{demand[t][i], t, i, 1.0 / cnt});
                }
        }
        sort(req[0].begin(), req[0].end());
        for (auto r : req[0]){
            bool fine = false;
            int sel = p[rand() % pn];
            if (qos[sel][r.c] < qos_constraint && ss[sel] + r.val <= min(bandwidth[sel], cap[sel])) {
                ss[sel] += r.val;
                distb[r.c].emplace_back(r.t, sel);
                fine = true;
            }
            if (!fine){
                for (int server : p) 
                    if (qos[server][r.c] < qos_constraint && ss[server] + r.val <= min(bandwidth[server], cap[server])) {
                        ss[server] += r.val;
                        distb[r.c].emplace_back(r.t, server);
                        fine = true;
                        break;
                    }
            }
            if (!fine) req[1].push_back(r);
        }
        req[0].clear();
        int seq = 1;
        while (!req[0].empty() || !req[1].empty()){
            for (int i = 0; i < n; i++) resout[i] = 0;
            for (auto r : req[seq]) {
                for (int server : p) 
                    if (qos[server][r.c] < qos_constraint) resout[server] += r.val;
            }
            for (int i = 0; i < n; i++) resout[i] = min(resout[i], bandwidth[i] - ss[i]);
            sort(p.begin(), p.end(), [&](int x, int y){
                return resout[x] > resout[y];
            });
            int sel = -1;
            for (int server : p) if (carry[server] < Limit && !vis[server]) {
                sel = server;
                vis[server] = true;
                carry[server]++;
                break;
            }
            if (sel == -1) {
                for (auto r : req[seq]) {
                    int sel = p[rand() % pn];
                    while (!(qos[sel][r.c] < qos_constraint && ss[sel] + r.val <= bandwidth[sel])) sel = p[rand() % pn];
                    ss[sel] += r.val;
                    cap[sel] = max(cap[sel], ss[sel]);
                }
                return -1;
            }
            for (auto r : req[seq]) {
                if (qos[sel][r.c] < qos_constraint && ss[sel] + r.val <= bandwidth[sel]) {
                    ss[sel] += r.val;
                    distb[r.c].emplace_back(r.t, sel);
                }else{
                    req[seq ^ 1].push_back(r);
                }
            }
            req[seq].clear();
            seq ^= 1;
        }
        
        return 1;
    }
}