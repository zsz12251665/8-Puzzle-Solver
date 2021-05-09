#include <array>
#include <bitset>
#include <queue>
#include <utility>
#include <vector>
#include <emscripten/emscripten.h>
using namespace std;

const int FACT[] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800};
const int INF = 2147483647;
struct Arrangement : array<int, 9> {
	int zeroPos;
	using array<int, 9>::array;
	Arrangement() {
		for (int i = 0; i < 9; ++i)
			at(i) = i;
		zeroPos = 0;
	}
	Arrangement(int a) {
		for (int i = 0; i < 9; ++i)
			at(i) = i;
		for (int i = 0; i < 8; ++i)
			for (int t = a % FACT[9 - i] / FACT[8 - i]; t > 0; --t)
				std::swap(at(i + t), at(i + t - 1));
		for (int i = 0; i < 9; ++i)
			if (at(i) == 0)
				zeroPos = i;
	}
	operator int() const {
		int res = 0;
		for (int i = 0; i < 8; ++i)
			for (int j = i + 1; j < 9; ++j)
				if (at(j) < at(i))
					res += FACT[8 - i];
		return res;
	}
	bool moveable(int i) const {
		switch (i) {
		case 0: return zeroPos > 2;
		case 1: return zeroPos < 6;
		case 2: return zeroPos % 3 > 0;
		case 3: return zeroPos % 3 < 2;
		};
		return false;
	}
	void move(int i) {
		switch (i) {
		case 0:
			if (zeroPos > 2) {
				std::swap(at(zeroPos), at(zeroPos - 3));
				zeroPos -= 3;
			}
			break;
		case 1:
			if (zeroPos < 6) {
				std::swap(at(zeroPos), at(zeroPos + 3));
				zeroPos += 3;
			}
			break;
		case 2:
			if (zeroPos % 3 > 0) {
				std::swap(at(zeroPos), at(zeroPos - 1));
				--zeroPos;
			}
			break;
		case 3:
			if (zeroPos % 3 < 2) {
				std::swap(at(zeroPos), at(zeroPos + 1));
				++zeroPos;
			}
			break;
		};
	}
};

typedef unsigned long long ull;
typedef pair<int, int> pii;

EM_JS(void, res_clear, (), { res = []; });
EM_JS(void, res_push_back, (int x), { res.push(x); });
EM_JS(void, res_push_front, (int x), { res.unshift(x); });

extern "C" {

int EMSCRIPTEN_KEEPALIVE h(int s, int t) {
	Arrangement a(s), b(t);
	int res = 0;
	for (int i = 0; i < 9; ++i)
		res += a[i] != b[i];
	return (res + 1) / 2;
}

ull EMSCRIPTEN_KEEPALIVE dfs(int s, int t) {
	static bitset<362880> visited;
	static vector<pii> q;
	visited.reset();
	q.clear();
	ull cnt = 0;
	visited[s] = true;
	q.push_back({s, 0});
	while (!q.empty() && q.back().first != t) {
		++cnt;
		if (q.back().second > 3)
			q.pop_back();
		Arrangement a(q.back().first);
		a.move(q.back().second++);
		if (!visited[a]) {
			visited[a] = true;
			q.push_back({(int)a, 0});
		}
	}
	res_clear();
	for (auto i : q)
		res_push_back(i.first);
	return cnt;
}

ull EMSCRIPTEN_KEEPALIVE bfs(int s, int t) {
	static array<int, 362880> prev;
	static queue<int> q;
	prev.fill(-1);
	while (!q.empty()) q.pop();
	ull cnt = 0;
	prev[s] = s;
	q.push(s);
	while (!q.empty() && q.front() != t) {
		++cnt;
		Arrangement a(q.front());
		q.pop();
		for (int i = 0; i < 4; ++i)
			if (a.moveable(i)) {
				Arrangement b(a);
				b.move(i);
				if (prev[b] == -1) {
					prev[b] = a;
					q.push(b);
				}
			}
	}
	prev[s] = -1;
	res_clear();
	for (int i = t; i != -1; i = prev[i])
		res_push_front(i);
	return cnt;
}

ull EMSCRIPTEN_KEEPALIVE hfs(int s, int t) {
	static array<int, 362880> dis;
	static priority_queue<pii, vector<pii>, greater<pii> > q;
	dis.fill(INF);
	while (!q.empty()) q.pop();
	ull cnt = 0;
	dis[s] = 0;
	q.push({h(s, t), s});
	while (!q.empty() && q.top().second != t) {
		++cnt;
		Arrangement a(q.top().second);
		q.pop();
		for (int i = 0; i < 4; ++i)
			if (a.moveable(i)) {
				Arrangement b(a);
				b.move(i);
				if (dis[b] > dis[a] + 1) {
					dis[b] = dis[a] + 1;
					q.push({dis[b] + h(b, t), (int)b});
				}
			}
	}
	res_clear();
	Arrangement a(t);
	for(int prev = INF; dis[a] < prev;) {
		res_push_front(a);
		prev = dis[a];
		for (int i = 0; i < 4; ++i)
			if (a.moveable(i)) {
				Arrangement b(a);
				b.move(i);
				if (dis[b] < prev) {
					a.move(i);
					break;
				}
			}
	}
	return cnt;
}

ull EMSCRIPTEN_KEEPALIVE bab(int s, int t) {
	static array<int, 362880> dis;
	static queue<pii> q;
	dis.fill(INF);
	while (!q.empty()) q.pop();
	ull cnt = 0;
	int bound = INF;
	dis[s] = 0;
	q.push({h(s, t), s});
	while (!q.empty()) {
		++cnt;
		auto p = q.front();
		q.pop();
		if (p.second == t)
			bound = min(bound, dis[t]);
		if (p.first >= bound)
			continue;
		Arrangement a(p.second);
		for (int i = 0; i < 4; ++i)
			if (a.moveable(i)) {
				Arrangement b(a);
				b.move(i);
				if (dis[b] > dis[a] + 1) {
					int f = dis[a] + 1 + h(b, t);
					if (f < bound) {
						dis[b] = dis[a] + 1;
						q.push({f, (int)b});
					}
				}
			}
	}
	res_clear();
	Arrangement a(t);
	for(int prev = INF; dis[a] < prev;) {
		res_push_front(a);
		prev = dis[a];
		for (int i = 0; i < 4; ++i)
			if (a.moveable(i)) {
				Arrangement b(a);
				b.move(i);
				if (dis[b] < prev) {
					a.move(i);
					break;
				}
			}
	}
	return cnt;
}

}
