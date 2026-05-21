#include <iostream>
#include <vector>
#include <algorithm>
#include "cp_gen.hpp"
using namespace std;
// https://codeforces.com/problemset/problem/2187/C
void generate(int L, int R, vector<pair<int, int>> &edges)
{
    if (R - L <= 1)
        return;
    edges.emplace_back(L, R);
    int M = Gen::rand(L + 1, R - 1);
    generate(L, M, edges);
    generate(M, R, edges);
}
int main()
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    int MAXN = 50;
    int n = Gen::rand(2, MAXN);
    vector<pair<int, int>> edges;
    generate(1, n, edges);
    int m = Gen::rand(0, edges.size());
    cout << n << " " << m << "\n";
    shuffle(edges.begin(), edges.end(), Gen::rng);
    for (int i = 0; i < m; i++)
        cout << edges[i].first << " " << edges[i].second << "\n";
    return 0;
}
