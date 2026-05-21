#include <bits/stdc++.h>

using namespace std;

using i64 = long long;

i64 solve_tree(int n, int r, vector<vector<int>> g)
{
    vector<int> dep(n), sz(n, 1), pa(n);
    i64 ans = 0;
    auto dfs = [&](auto &&self, int u, int p) -> map<int, int>
    {
        pa[u] = p;
        map<int, int> mp;
        mp[dep[u]] = 1;
        for (auto v : g[u])
            if (v != p)
            {
                dep[v] = dep[u] + 1;
                auto nmp = self(self, v, u);
                if (mp.size() < nmp.size())
                    swap(mp, nmp);
                for (auto &[d, f] : nmp)
                {
                    ans += 1LL * f * mp[d] * (d - dep[u]);
                    mp[d] += f;
                }
                sz[u] += sz[v];
            }
        return mp;
    };
    dfs(dfs, r, -1);
    for (int u = 0; u < n; u++)
    {
        i64 s = 1LL * sz[u] * (sz[u] - 1) / 2;
        for (auto v : g[u])
            if (v != pa[u])
                s -= 1LL * sz[v] * (sz[v] - 1) / 2;
        ans -= 1LL * dep[u] * s;
    }
    sort(dep.begin(), dep.end());
    for (int u = 0; u < n; u++)
        ans += 1LL * dep[u] * (n - u - 1);
    return ans;
}

void solve()
{
    int n, m;
    cin >> n >> m;
    vector<int> a(n);
    for (int i = 0; i < n - 1; i++)
        a[i] = i + 1;
    for (int i = 0; i < m; i++)
    {
        int u, v;
        cin >> u >> v;
        u--;
        v--;
        a[u] = max(a[u], v);
    }
    vector<vector<int>> g(n);
    for (int i = 0; i < n - 1; i++)
        g[a[i]].push_back(i);
    cout << solve_tree(n, n - 1, g) << "\n";
}

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);
    solve();
}
