namespace work{
    struct Request{
        int val, t, c;
        bool operator < (const Request &b)const{
            return val < b.val;
        }
    };
    int solve(const vector<vector<int>>& demand, const vector<int>& bandwidth,
              const vector<vector<int>>& qos,
              const vector<int>& carry,
              vector<vector<pair<int, int> >> &distb, vector<int>& cap, vector<int>& p, vector<long long>&tot,
              const int n, const int m, const int l, const int r, const int Limit, const int qos_constraint, const int base_cost){  
        const int pn = p.size();
        vector<int> ss(n);
        vector<Request> req[2];
        //random_shuffle(p.begin(), p.end());

        auto process = [&](vector<Request> &src, vector<Request> &dst, int s){
            for (auto r : src) {
                if (qos[s][r.c] < qos_constraint && ss[s] + r.val <= bandwidth[s]) {
                    ss[s] += r.val;
                    tot[s] += r.val;
                    distb[r.c].emplace_back(r.t, s);
                }else dst.push_back(r);
            }
            src.clear();
        };

        for (int t = l; t <= r; t++) {
            for (int i = 0; i < m; i++) 
                if (demand[t][i] > 0) {
                    req[0].push_back(Request{demand[t][i], t, i});
                }
        }

        sort(req[0].rbegin(), req[0].rend());
        int seq = 0;
        for (int s : carry) process(req[seq], req[seq ^ 1], s), seq ^= 1; 
        for (auto r : req[seq]){
            int sel = -1;
            for (int s : p) if (qos[s][r.c] < qos_constraint && ss[s] + r.val <= cap[s]) {
                if (sel == -1) sel = s;
                else if (cap[s] - ss[s] > cap[sel] - ss[sel]) sel = s;
            }
            if (sel >= 0){
                ss[sel] += r.val;
                tot[sel] += r.val;
                distb[r.c].emplace_back(r.t, sel);
            }else req[seq ^ 1].push_back(r);
        }
        seq ^= 1;
        if (!req[seq].empty()){
            for (auto r : req[seq]) {
                sort(p.begin(), p.end(), [&](int x, int y){
                    return 1LL * (cap[x] - base_cost) * bandwidth[y] < 1LL * (cap[y] - base_cost) * bandwidth[x];
                });
                for (int i = 0; i < pn; i++) if (qos[p[i]][r.c] < qos_constraint && ss[p[i]] + r.val <= bandwidth[p[i]]) {
                    ss[p[i]] += r.val;
                    cap[p[i]] = max(cap[p[i]], ss[p[i]]);
                    break;
                }
            }
            return -1;
        }
        return 1;
    }

    void solve_record_load(const vector<vector<int>>& demand, const vector<int>& bandwidth,
              const vector<vector<int>>& qos,
              const vector<int>& carry,
              vector<int>& cap, vector<int>& p, vector<vector<int>>  & load,
              const int n, const int m, const int Ti, const int l, const int r, const int Limit, const int qos_constraint, const int base_cost){  
        const int pn = p.size();
        vector<int> ss(n);
        vector<Request> req[2];
        //random_shuffle(p.begin(), p.end());

        auto process = [&](vector<Request> &src, vector<Request> &dst, int s){
            for (auto r : src) {
                if (qos[s][r.c] < qos_constraint && ss[s] + r.val <= bandwidth[s]) {
                    ss[s] += r.val;
                }else dst.push_back(r);
            }
            src.clear();
        };

        for (int t = l; t <= r; t++) {
            for (int i = 0; i < m; i++) 
                if (demand[t][i] > 0) {
                    req[0].push_back(Request{demand[t][i], t, i});
                }
        }

        sort(req[0].rbegin(), req[0].rend());
        int seq = 0;
        for (int s : carry) process(req[seq], req[seq ^ 1], s), seq ^= 1; 
        for (auto r : req[seq]){
            int sel = -1;
            for (int s : p) if (qos[s][r.c] < qos_constraint && ss[s] + r.val <= cap[s]) {
                if (sel == -1) sel = s;
                else if (cap[s] - ss[s] > cap[sel] - ss[sel]) sel = s;
            }
            if (sel >= 0) ss[sel] += r.val;
            else req[seq ^ 1].push_back(r);
        }
        seq ^= 1;
        if (!req[seq].empty()){
            for (auto r : req[seq]) {
                sort(p.begin(), p.end(), [&](int x, int y){
                    return 1LL * (cap[x] - base_cost) * bandwidth[y] < 1LL * (cap[y] - base_cost) * bandwidth[x];
                });
                for (int i = 0; i < pn; i++) if (qos[p[i]][r.c] < qos_constraint && ss[p[i]] + r.val <= bandwidth[p[i]]) {
                    ss[p[i]] += r.val;
                    cap[p[i]] = max(cap[p[i]], ss[p[i]]);
                    break;
                }
            }
        }
        for (int s : p) load[s][Ti] = ss[s];
    }
}