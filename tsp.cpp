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

inline void do2opt(int a, int b, vector<int>& path) {
	int m1 = a < b ? a : b;
	int m2 = a >= b ? a : b;
	reverse(path.begin()+m1+1, path.begin()+m2+1);
}

inline void do3opt(int a, int b, int c, vector<int>& path) {
	vector<int> t = {a, b, c};
	sort(t.begin(), t.end());
	int i = t[0];
	int j = t[1];
	int k = t[2];
	auto tour = path;
	memcpy(path.data()+i+1, tour.data()+j+1, (k-j)*sizeof(int));
	memcpy(path.data()+i+(k-j)+1, tour.data()+i+1, (j-i)*sizeof(int));
}

vector<int> simAnnealing2opt(vector<int> path, const vector<vector<float>> dist) {
	srand(RANDOM_SEED); // Seed the random number generator
	auto start = high_resolution_clock::now();
	int duration = duration_cast<microseconds>(high_resolution_clock::now() - start).count();
	int pathLen = path.size();

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

		// d0 = distance(A, B) + distance(C, D) + distance(E, F)
		float orgDist = dist[a1][a2] + dist[b1][b2] + dist[c1][c2];
    	// d1 = distance(A, C) + distance(B, D) + distance(E, F)
		float sw1Dist = dist[a1][b1] + dist[a2][b2] + dist[c1][c2];
    	// d4 = distance(F, B) + distance(C, D) + distance(E, A)
		float sw2Dist = dist[a2][c2] + dist[b1][b2] + dist[a1][c1];
    	// d2 = distance(A, B) + distance(C, E) + distance(D, F)
		float sw3Dist = dist[a1][a2] + dist[b1][c1] + dist[b2][c2];
    	// d3 = distance(A, D) + distance(E, B) + distance(C, F)
		float sw4Dist = dist[a1][b2] + dist[a2][c1] + dist[b1][c2];

		if (orgDist > sw1Dist) {
			do2opt(a, b, path);
		} else if (orgDist > sw2Dist) {
			do2opt(a, c, path);
		} else if (orgDist > sw3Dist) {
			do2opt(b, c, path);
		} else if (orgDist > sw4Dist) {
			do3opt(a, b, c, path);
		}

		duration = duration_cast<microseconds>(high_resolution_clock::now() - start).count();
	}

	return path;
}

// TODO: there are may optimizations posible
vector<int> solve(vector<point> points) {
	auto distMatrix = euclideanDistance(points);
	auto path = greedyPath(distMatrix);
	// printPath(path);
	// printPathLen(path, distMatrix);
	path = simAnnealing2opt(path, distMatrix);
	// printPath(path);
	// printPathLen(
	printDebugPathLen(path, distMatrix);
	return path;
}

int main() {
	auto points = parseInput();
	auto path = solve(points);
	printPath(path);
	return 0;
}

