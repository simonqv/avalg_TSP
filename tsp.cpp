#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <chrono>
#include <cstring>

#define RANDOM_SEED 0
#define CUTOFF_US 1980000

using namespace std;
using namespace chrono;

typedef struct {
	float x;
	float y;
} point;

void printPath(vector<int> path) { 
	for (auto p : path)
		cout << p << endl;
}

void printDebugPath(vector<int> path) {
	for (auto i : path)
		cerr << i << " ";
	cerr << endl;
}

void printDebugPathLen(vector<int> path, vector<vector<float>> distMatrix) {
	float l = distMatrix[path.back()][path.front()];
	for (size_t i=0; i < path.size()-1; i++) {
		l += distMatrix[path.at(i)][path[i+1]];
	}
	cerr << "path len: " << l << endl;
}

float calcPathDist(const vector<int>& path, const vector<vector<float>>& distMatrix) {
	float l = distMatrix[path.back()][path.front()];
	for (size_t i=0; i < path.size()-1; i++) {
		l += distMatrix[path[i]][path[i+1]];
	}
	return l;
}

void printMatrix(vector<vector<float>> m) {
	for (auto dv : m) {
		for (auto d : dv) 
			cout << d << "\t";
		cout << endl;
	}
}

vector<point> parseInput() {
	int n;
	float x, y;

	cin >> n;
	vector<point> out(n);

	for (int i = 0; i < n; i++) {
		cin >> x >> y;
		out.at(i) = {x, y};
	}

	return out;
}

vector<vector<float>> euclideanDistance(vector<point> points) {
	int n = points.size();
	vector<vector<float>> out(n);
	for (int i = 0; i < n; i++) {
		out[i] = vector<float>(n);
	}

	for (int a = 0; a < n; a++) {
		for (int b = a+1; b < n; b++) {
			float dist = round(sqrt(pow(points[a].x - points[b].x, 2) + pow(points[a].y - points[b].y, 2)));
			out[a][b] = dist;
			out[b][a] = dist;
		}
	}

	for (int a = 0; a < n; a++)
		out[a][a] = numeric_limits<float>::max();

	return out;
}

vector<int> greedyPath(vector<vector<float>> distMatrix) {
	vector<int> path = vector<int>();
	path.reserve(distMatrix.size());
	path.push_back(0);

	while (path.size() < distMatrix.size())  {
		int from = path.back();

		for (int i = 0; i < (int) distMatrix.size(); i++) {
			distMatrix[i][from] = numeric_limits<float>::max();
		}

		auto result = min_element(distMatrix[from].begin(), distMatrix[from].end());
		int to = distance(distMatrix[from].begin(), result);
		

		path.push_back(to);
	}

	return path;
}

void do2opt(int a, int b, vector<int>& path) {
	int m1 = a < b ? a : b;
	int m2 = a >= b ? a : b;
	reverse(path.begin()+m1+1, path.begin()+m2+1);
}

void dod2opt(int a, int b, vector<int>& path) {
	int m1 = a < b ? a : b;
	int m2 = a >= b ? a : b;
	reverse(path.begin()+m1+1, path.begin()+m2+1);
	reverse(path.begin(), path.end());
	// a'bc -> c'b'a == ac'b'
}

void do3opt(int a, int b, int c, vector<int>& path) {
	do2opt(a, b, path);
	do2opt(b, c, path);
	do2opt(a, c, path);
}

vector<int> simAnnealing(vector<int> path, const vector<vector<float>> dist) {
	srand(RANDOM_SEED); // Seed the random number generator
	auto start = high_resolution_clock::now();
	int duration = duration_cast<microseconds>(high_resolution_clock::now() - start).count();
	int pathLen = path.size();
	float cases[8];

	while (duration < CUTOFF_US) {
		// First edge
        int a = rand() % pathLen;
		int an = (a+1) % pathLen;
        // Second edge
		int b = rand() % pathLen;
		int bn = (b+1) % pathLen;
        // Third edge
		int c = rand() % pathLen;
		int cn = (c+1) % pathLen;

        // Nodes of edges
		int a1 = path[a];
		int a2 = path[an];
		int b1 = path[b];
		int b2 = path[bn];
		int c1 = path[c];
		int c2 = path[cn];

		// org
		// d0 = distance(A, B) + distance(C, D) + distance(E, F)
		cases[0] = dist[a1][a2] + dist[b1][b2] + dist[c1][c2];

		// 2opt
    	// d1 = distance(A, C) + distance(B, D) + distance(E, F)
		// sw a b
		cases[1] = dist[a1][b1] + dist[a2][b2] + dist[c1][c2];
    	// d4 = distance(F, B) + distance(C, D) + distance(E, A)
		// sw a c
		cases[2] = dist[a2][c2] + dist[b1][b2] + dist[a1][c1];
    	// d2 = distance(A, B) + distance(C, E) + distance(D, F)
		// sw b c
		cases[3] = dist[a1][a2] + dist[b1][c1] + dist[b2][c2];

		// 2opt 2opt
		// sw a b -> ...
		cases[4] = dist[a1][b1] + dist[c1][a2] + dist[b2][c2];
		// sw a c -> ...
		cases[5] = dist[a1][c1] + dist[b1][c2] + dist[a2][b2];
		// sw b c -> ...
		cases[6] = dist[b1][c1] + dist[a1][b2] + dist[c2][a2];

		// 3opt
    	// d3 = distance(A, D) + distance(E, B) + distance(C, F)
		cases[7] = dist[a1][b2] + dist[a2][c1] + dist[b1][c2];

		auto result = min_element(cases, cases+8);
		int index = result - cases;

		switch (index) {
			case 1:
				do2opt(a, b, path);
				break;
			case 2:
				do2opt(a, c, path);
				break;
			case 3:
				do2opt(b, c, path);
				break;
			case 4:
				dod2opt(a, b, path);
				break;
			case 5:
				dod2opt(a, c, path);
				break;
			case 6:
				dod2opt(b, c, path);
				break;
			case 7:
				do3opt(a, b, c, path);
				break;
		}

		duration = duration_cast<microseconds>(high_resolution_clock::now() - start).count();
	}

	return path;
}

vector<int> solve(vector<point> points) {
	auto distMatrix = euclideanDistance(points);
	auto path = greedyPath(distMatrix);
	path = simAnnealing(path, distMatrix);
	return path;
}

int main() {
	auto points = parseInput();
	auto path = solve(points);
	printPath(path);
	return 0;
}

