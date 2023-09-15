#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

class dinic
{
  private:
    int n, m, s, t;
    int cnt = 1;
    int head[100005], d[100005], q[100005];
    int now[100005]; // now[i]表示i点的当前弧
    struct Edge
    {
        ll t, next, w;
    } edge[200005];

  public:
    dinic(int n, int m, int s, int t)
    {
        this->n = n;
        this->m = m;
        this->s = s;
        this->t = t;
    };
    bool makelevel(int s, int t)
    {
        memset(d, 0, sizeof d); // 清空深度数组
        memset(q, 0, sizeof q); // 清空人工队列
        d[s] = 1;               // 源点深度为1
        int l = 0, r = 0;       // 人工队列的头尾指针
        q[r++] = s;             // 将源点加入队列
        now[s] = head[s];       // 源点的当前弧为head[s]
        while (l < r)
        {
            int x = q[l++]; // 取出队首元素
            if (x == t)     // 如果到达汇点，返回true
                return true;
            for (int i = head[x]; i != 0; i = edge[i].next) // 遍历点x的所有边
            {
                int v = edge[i].t;
                if ((d[v] == 0) and (edge[i].w != 0)) // 如果该边未被访问过且残量不为0
                {
                    q[r++] = v;       // 将该点加入队列
                    now[v] = head[v]; // 将该点的当前弧设为head[v]
                    d[v] = d[x] + 1;  // 该点的深度为x的深度+1
                }
            }
        }
        return false; // 残留网络不能够到达汇点，返回false
    }
    ll dfs(int x, int t, ll flow = 2005020600)
    {
        if (x == t)
            return flow;
        ll sum = 0; // sum用于记录当前点x的所有流量之和
        for (int i = now[x]; i != 0; i = edge[i].next)
        {
            now[x] = i;                                          // 更新当前弧
            if ((edge[i].w != 0) and (d[edge[i].t] == d[x] + 1)) // 如果该边的残量不为0且该边的终点深度为x的深度+1
            {
                ll tmp = dfs(edge[i].t, t,
                             min(flow - sum, edge[i].w)); // 一路搜索下去，直到到达汇点或者流量为0，得到路径上的最小残量
                edge[i].w -= tmp;     // 正向边减去流量
                edge[i ^ 1].w += tmp; // 反向边加上流量
                sum += tmp;
                if (sum == flow) // 如果当前点x没有可分配流量，返回sum，若存在可分配流量，在该点之后继续寻找增广路
                    return sum;
            }
        }
        return sum; // 若该点没有增广路，返回sum=0
    }
    void add(int a, int b, ll c)
    {
        edge[++cnt].t = b;
        edge[cnt].next = head[a];
        edge[cnt].w = c;
        head[a] = cnt;
        edge[++cnt].t = a;
        edge[cnt].next = head[b];
        edge[cnt].w = 0;
        head[b] = cnt;
    }
};

void solve()
{
    ll ans = 0;
    int n, m, s, t;
    cin >> n >> m >> s >> t;
    dinic d(n, m, s, t);
    for (int i = 1; i <= m; ++i)
    {
        long long u, v, w;
        cin >> u >> v >> w;
        d.add(u, v, w);
    }
    while (d.makelevel(s, t))
    {
        ans += d.dfs(s, t);
    }
    cout << ans << endl;
}

int main()
{
    solve();
    return 0;
}