#include <bits/stdc++.h>
using namespace std;

struct SegTree {
	int size;
	struct Item {
		int seg, pref, suff, len;
	};
	vector<int> op;
	vector<Item> vals;
	const Item NEUTRAL_FOR_MERGING = {0, 0, 0, 0};
	const int NO_OPERATION = INT_MAX;  // arbitrary

	int update_op(int a, int b) {
		if (b == NO_OPERATION) return a;
		return b;
	}

	Item update_item(Item a, int b, int len) {
		if (b == NO_OPERATION) return a;
		return {b * len, b * len, b * len, len};
	}

	Item merge(Item a, Item b) {
		Item tmp = {0, 0, 0, 0};
		tmp.pref = a.pref == a.len ? a.pref + b.pref : a.pref;
		tmp.suff = b.suff == b.len ? b.suff + a.suff : b.suff;
		tmp.seg = max({a.seg, b.seg, a.suff + b.pref});
		tmp.len = a.len + b.len;
		return tmp;
	}

	void apply_op(int &a, int b) { a = update_op(a, b); }

	void apply_item(Item &a, int b, int len) { a = update_item(a, b, len); }

	SegTree(int n) {
		size = 1;
		while (size < n) size *= 2;
		op.resize(2 * size, NO_OPERATION);
		vals.resize(2 * size, {0, 0, 0, 0});
	}

	void propagate(int x, int lx, int rx) {
		if (rx - lx == 1) return;
		int m = (lx + rx) / 2;
		apply_op(op[2 * x + 1], op[x]);
		apply_item(vals[2 * x + 1], op[x], m - lx);
		apply_op(op[2 * x + 2], op[x]);
		apply_item(vals[2 * x + 2], op[x], rx - m);
		op[x] = NO_OPERATION;
	}

	void set(int l, int r, int v, int x, int lx, int rx) {
		propagate(x, lx, rx);
		if (lx >= r || rx <= l) return;
		if (lx >= l && rx <= r) {
			apply_op(op[x], v);
			apply_item(vals[x], v, rx - lx);
			return;
		}
		int m = (lx + rx) / 2;
		set(l, r, v, 2 * x + 1, lx, m);
		set(l, r, v, 2 * x + 2, m, rx);
		vals[x] = merge(vals[2 * x + 1], vals[2 * x + 2]);
	}

	void set(int l, int r, int v) { return set(l, r, v, 0, 0, size); }
	// find leftmost index of first gap such that there are x consecutive 1s
	int find(int v, int x, int lx, int rx) {
		propagate(x, lx, rx);
		if (vals[x].seg < v) return -1;
		if (rx - lx == 1) return lx;
		int m = (lx + rx) / 2;
		int left = find(v, 2 * x + 1, lx, m);
		if (left != -1) {
			return left;
		}  // return if we found a valid segment in left subtree
		// since segment is in both ranges, we can deduce the start index
		if (vals[2 * x + 1].suff + vals[2 * x + 2].pref >= v) {
			return m - vals[2 * x + 1].suff;
		}
		return find(v, 2 * x + 2, m, rx);  // go down right subtree
	}

	int find(int v) { return find(v, 0, 0, size); }
};

int main() {
	freopen("seating.in", "r", stdin);
	freopen("seating.out", "w", stdout);
	int n, m;
	cin >> n >> m;

	SegTree st(n);
	st.set(0, n, 1);  // initialize all positions to 1
	int ans = 0;
	for (int query = 0; query < m; query++) {
		char c;
		cin >> c;
		if (c == 'A') {
			int x;
			cin >> x;
			int res = st.find(x);
			if (res == -1) {
				ans++;
			} else {
				// range set [res, res+x-1] to 0, indicating filled
				st.set(res, res + x, 0);
			}
		} else {
			int l, r;
			cin >> l >> r;
			--l;
			--r;
			st.set(l, r + 1, 1);
		}
	}

	cout << ans << endl;
}