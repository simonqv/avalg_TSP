#include <iostream>
#include <vector>
#include <tuple>
#include <cmath>
#include <limits>
#include <algorithm>


using namespace std;

typedef struct {
	float x;
	float y;
} point;

vector<point> parseInput() {
	int n;
	float x, y;

	cin >> n;
	vector<point> out(n);

	for (int i = 0; i < n; i++) {
		cin >> x >> y;
		out[i] = {x, y};
	}

	return out;
}

// TODO: optimization, do a triangular matrix
vector<vector<float>> euclideanDistance(vector<point> points) {
	int n = points.size();
	vector<vector<float>> out(n);
	for (int i = 0; i < n; i++) {
		out[i] = vector<float>(n);
	}

	for (int a = 0; a < n; a++) {
		for (int b = 0; b < n; b++) {
			out[a][b] = pow(points[a].x - points[b].x, 2) + pow(points[a].y - points[b].y, 2);
			if (out[a][b] <= 0)
				out[a][b] = numeric_limits<float>::max();
		}
	}

	return out;
}

// TODO: there are may optimizations posible
vector<int> solve(vector<point> points) {
	auto dists = euclideanDistance(points);
	/*
	for (auto dv : dists) {
		for (auto d : dv) 
			cout << d << "\t";
		cout << endl;
	}
	*/

	vector<int> path = vector<int>();
	path.push_back(0);

	while (path.size() < dists.size())  {
		int from = path.back();

		for (int i = 0; i < (int) dists.size(); i++) {
			dists[i][from] = numeric_limits<float>::max();
		}

		vector<float>::iterator result = min_element(dists[from].begin(), dists[from].end());
		int to = distance(dists[from].begin(), result);
		

		path.push_back(to);
	}

	return path;
}

void printPath(vector<int> path) { 
	for (auto p : path)
		cout << p << endl;
}

int main() {
	auto points = parseInput();
	auto path = solve(points);
	printPath(path);
	return 0;
}
