const int INF = 0x3f3f3f3f, MAXN = 200, MAXM = 20000;
struct Edge { int to, nex, cap, constraint;  } e[MAXM];
int tol, head[MAXN], dep[MAXN];

void addedge(int u, int v, int cap, int constraint, int rev = 0) {
	e[tol] = Edge { v, head[u], cap, constraint}; head[u] = tol++;
	e[tol] = Edge { u, head[v], rev, constraint}; head[v] = tol++;
}

bool dinic_bfs(int s, int t) {
	static int Q[MAXN]; int front = 0, rear = 0;
	memset(dep, 0, sizeof(dep)); Q[rear++] = s, dep[s] = 1;
	while (front != rear) {
		int u = Q[front++], v;
		for (int i = head[u]; ~i; i = e[i].nex) {
			if (e[i].cap > 0 && dep[v = e[i].to] == 0) {
				dep[v] = dep[u] + 1;
				if (v == t) return true;
				Q[rear++] = v;
			}
		}
	}
	return false;
}

int dinic_dfs(int u, int t, int f, int step) {
	if (u == t) return f; 
    int d, v, c = 0;
	for (int i = head[u]; ~i; i = e[i].nex) {
		if (e[i].cap > 0 && dep[u] + 1 == dep[v = e[i].to]) {
			if (e[i].constraint) d = dinic_dfs(v, t, min(f - c, min(step, e[i].cap)), step);
			else d = dinic_dfs(v, t, min(f - c, e[i].cap), step);
			if (d > 0) {
				e[i].cap -= d, e[i^1].cap += d, c += d;
				if (f == c) break;
			} else dep[v] = -1;
		}
	}
	return c;
}

int dinic(int s, int t, int step) {
	int maxflow = 0;
	while (dinic_bfs(s, t))
		maxflow += dinic_dfs(s, t, INF, step);
	return maxflow;
}
