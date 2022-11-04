#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <chrono>

#define RANDOM_SEED 0
#define CUTOFF_MS 1980

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

void printPathLen(vector<int> path, vector<vector<float>> distMatrix) {
	float l = distMatrix[path.back()][path.front()];
	for (size_t i=0; i < path.size()-1; i++) {
		l += distMatrix[path[i]][path[i+1]];
	}
	cout << "path len: " << l << endl;
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
		out[i] = {x, y};
	}

	return out;
}

// TODO: optimization, do a triangular matrix? Might not be worth it
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

vector<int> greedyPath(vector<vector<float>> distMatrix) {
	vector<int> path = vector<int>();
	path.push_back(0);

	while (path.size() < distMatrix.size())  {
		int from = path.back();

		for (int i = 0; i < (int) distMatrix.size(); i++) {
			distMatrix[i][from] = numeric_limits<float>::max();
		}

		vector<float>::iterator result = min_element(distMatrix[from].begin(), distMatrix[from].end());
		int to = distance(distMatrix[from].begin(), result);
		

		path.push_back(to);
	}

	return path;
}

inline void do2opt(int a, int an, int b, int bn, vector<int>& path, vector<int>& tmpPath) {
	int pathLen = (int)path.size();

	int m1 = a < b ? a : b;
	int m1n = a < b ? an : bn;
	int m2 = a >= b ? a : b;
	int m2n = a >= b ? an : bn;

	// printf("%d %d %d %d\n", m1, m1n, m2, m2n);

	int i = 0;
	int j = m2n != 0 ? 0 : 1;
	while (j<=m1) {
		tmpPath[i] = path[j];
		// cout << i << " 1= " << j << endl;
		i++; j++;
	}

	j = m2; 
	while (m1n<=j) {
		tmpPath[i] = path[j];
		// cout << i << " 2= " << j << endl;
		i++; j--;
	}

	j = m2n;
	while (j<pathLen && i<pathLen) {
		tmpPath[i] = path[j];
		// cout << i << " 3= " << j << endl;
		i++; j++;
	}

	path = tmpPath;
}

vector<int> simAnnealing2opt(vector<int> path, const vector<vector<float>> dist) {
	srand(RANDOM_SEED); // Seed the random nubmer generator
	auto start = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - start).count();
	int pathLen = path.size();
	auto tmpPath = vector<int>(path.size());

	long unsigned int countLoops = 0;
	long unsigned int countChanges = 0;
	long unsigned int countBadSteps = 0;

	while (duration < CUTOFF_MS) {
		int a = rand() % pathLen;
		int an = (a+1) % pathLen;
		int b = rand() % pathLen;
		int bn = (b+1) % pathLen;
		int a1 = path[a];
		int a2 = path[an];
		int b1 = path[b];
		int b2 = path[bn];
		float aDist = dist[a1][a2];
		float bDist = dist[b1][b2];
		float swDist1 = dist[a1][b1];
		float swDist2 = dist[a2][b2];

		float orgDist = aDist + bDist;
		float swDist = swDist1 + swDist2;

		/*
		int64_t r = (100*(rand() % CUTOFF_MS)) / CUTOFF_MS;
		int64_t prob = ((100*duration) / CUTOFF_MS);
		bool doBadStep = r > pow(prob, 4);
		*/
		if (orgDist > swDist /* || doBadStep */) {
			// cout << "shorter path found, switch " << a << " and " << b << endl;
			do2opt(a, an, b, bn, path, tmpPath);
			// printPath(path);
			// printPathLen(path, dist);
			// cout << "--" << endl;
			countChanges++;
			/*
			if (doBadStep) {
				countBadSteps++;
			}
			*/
		}

		countLoops++;
		duration = duration_cast<milliseconds>(high_resolution_clock::now() - start).count();
	}

	cerr << "loops: " << countLoops << "\tchanges: " << countChanges << "\tbad steps: " << countBadSteps << endl;

	return path;
}

// TODO: there are may optimizations posible
vector<int> solve(vector<point> points) {
	auto distMatrix  = euclideanDistance(points);
	auto path = greedyPath(distMatrix);
	// printPath(path);
	// printPathLen(path, distMatrix);
	path = simAnnealing2opt(path, distMatrix);
	// printPath(path);
	// printPathLen(path, distMatrix);
	return path;
}

int main() {
	auto points = parseInput();
	auto path = solve(points);
	printPath(path);
	return 0;
}
