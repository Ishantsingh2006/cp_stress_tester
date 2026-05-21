#include <vector>
#include <string>
#include <set>
#include <utility>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>
#include <cassert>
using namespace std;
#define ll long long

namespace Gen
{
    inline mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
    inline ll rand(ll l, ll r)
    {
        uniform_int_distribution<ll> uni(l, r);
        return uni(rng);
    }
    inline vector<ll> rand_array(ll n, ll L, ll R)
    {
        vector<ll> res(n);
        for (ll i = 0; i < n; i++)
            res[i] = rand(L, R);
        return res;
    }
    inline vector<ll> perm(ll n, ll start = 0)
    {
        vector<ll> res(n);
        iota(res.begin(), res.end(), start);
        shuffle(res.begin(), res.end(), rng);
        return res;
    }
    inline vector<ll> sorted(ll n, ll L, ll R, bool is_reversed)
    {
        vector<ll> res = rand_array(n, L, R);
        sort(res.begin(), res.end());
        if (is_reversed)
            reverse(res.begin(), res.end());
        return res;
    }
    inline vector<ll> extreme(ll n, ll L, ll R)
    {
        vector<ll> vec = {L, R};
        vector<ll> res(n);
        for (ll i = 0; i < n; i++)
            res[i] = vec[rand(0, 1)];
        return res;
    }
    inline string gen_str(ll n, char L, char R)
    {
        string res;
        for (ll i = 0; i < n; i++)
            res += 'a' + rand(L - 'a', R - 'a');
        return res;
    }
    inline string binary_str(ll n)
    {
        string res;
        for (ll i = 0; i < n; i++)
            res += '0' + rand(0, 1);
        return res;
    }
    inline string palindrome(ll n, char L, char R)
    {
        string half = gen_str(n / 2, L, R);
        string res = half;
        if (n % 2)
            res += 'a' + rand(L - 'a', R - 'a');
        reverse(half.begin(), half.end());
        res += half;
        return res;
    }
    inline vector<pair<ll, ll>> linear_tree(ll n)
    {
        vector<pair<ll, ll>> ed(n - 1);
        for (ll i = 0; i < n - 1; i++)
            ed[i] = {i + 1, i + 2};
        return ed;
    }
    inline vector<pair<ll, ll>> star_tree(ll n)
    {
        vector<pair<ll, ll>> ed;
        ll x = rand(0, n - 1);
        for (ll i = 0; i < n; i++)
        {
            if (i != x)
            {
                ed.emplace_back(x + 1, i + 1);
            }
        }
        return ed;
    }
    inline vector<pair<ll, ll>> tree(ll n)
    {
        if (n <= 1)
            return {};
        if (n == 2)
            return {{1, 2}};
        vector<ll> prufer = rand_array(n - 2, 1, n);
        vector<ll> degree(n + 1, 1);
        for (ll v : prufer)
            degree[v]++;
        set<ll> leaves;
        for (ll i = 1; i <= n; i++)
        {
            if (degree[i] == 1)
                leaves.insert(i);
        }
        vector<pair<ll, ll>> ed;
        for (ll v : prufer)
        {
            ll leaf = *leaves.begin();
            leaves.erase(leaves.begin());
            ed.emplace_back(leaf, v);
            degree[v]--;
            if (degree[v] == 1)
                leaves.insert(v);
        }
        ed.emplace_back(*leaves.begin(), *leaves.rbegin());
        return ed;
    }
    inline vector<pair<ll, ll>> graph(ll n, ll m)
    {
        assert(m <= n * (n - 1) / 2);
        set<pair<ll, ll>> ed;
        while (ed.size() < m)
        {
            ll u = rand(1, n);
            ll v = rand(1, n);
            if (u != v)
            {
                if (u > v)
                    swap(u, v);
                ed.emplace(u, v);
            }
        }
        return vector<pair<ll, ll>>(ed.begin(), ed.end());
    }
    inline vector<pair<ll, ll>> connected_graph(ll n, ll m)
    {
        assert(m >= n - 1);
        vector<pair<ll, ll>> res = tree(n);
        set<pair<ll, ll>> ed;
        for (auto e : res)
        {
            if (e.first > e.second)
                swap(e.first, e.second);
            ed.insert(e);
        }
        while (ed.size() < m)
        {
            ll u = rand(1, n);
            ll v = rand(1, n);
            if (u != v)
            {
                if (u > v)
                    swap(u, v);
                ed.emplace(u, v);
            }
        }
        return vector<pair<ll, ll>>(ed.begin(), ed.end());
    }
    inline vector<pair<ll, ll>> dag(ll n, ll m)
    {
        assert(m <= n * (n - 1) / 2);
        set<pair<ll, ll>> ed;
        while (ed.size() < m)
        {
            ll u = rand(1, n - 1);
            ll v = rand(u + 1, n);
            ed.emplace(u, v);
        }
        vector<pair<ll, ll>> res(ed.begin(), ed.end());
        vector<ll> p = perm(n, 1);
        for (auto &e : res)
        {
            e.first = p[e.first - 1];
            e.second = p[e.second - 1];
        }
        shuffle(res.begin(), res.end(), rng);
        return res;
    }
    inline vector<pair<ll, ll>> bipartite_graph(ll n, ll m)
    {
        vector<ll> A, B;
        for (ll i = 1; i <= n; i++)
        {
            if (rand(0, 1))
                A.push_back(i);
            else
                B.push_back(i);
        }
        assert(m <= (n / 2) * (n - n / 2));
        if (A.empty() || B.empty())
            return bipartite_graph(n, m);
        if (A.size() * B.size() < m)
            return bipartite_graph(n, m);
        set<pair<ll, ll>> ed;
        while (ed.size() < m)
        {
            ll u = A[rand(0, A.size() - 1)];
            ll v = B[rand(0, B.size() - 1)];
            if (u > v)
                swap(u, v);
            ed.emplace(u, v);
        }
        return vector<pair<ll, ll>>(ed.begin(), ed.end());
    }
    inline pair<ll, ll> single_range(ll L, ll R)
    {
        ll l = rand(L, R);
        ll r = rand(L, R);
        if (l > r)
            swap(l, r);
        return {l, r};
    }
    inline vector<pair<ll, ll>> ranges(ll q, ll L, ll R, bool disjoint = false)
    {
        vector<pair<ll, ll>> res;
        if (!disjoint)
        {
            for (ll i = 0; i < q; i++)
            {
                res.push_back(single_range(L, R));
            }
        }
        else
        {
            assert(2 * q <= R - L + 1);
            set<ll> pts;
            while (pts.size() < 2 * q)
            {
                pts.insert(rand(L, R));
            }
            vector<ll> p(pts.begin(), pts.end());
            for (ll i = 0; i < 2 * q; i += 2)
            {
                res.emplace_back(p[i], p[i + 1]);
            }
            shuffle(res.begin(), res.end(), rng);
        }
        return res;
    }
}
