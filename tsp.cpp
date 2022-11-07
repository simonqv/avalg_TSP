#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <chrono>

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

void printPathLen(vector<int> path, vector<vector<float>> distMatrix) {
	float l = distMatrix[path.back()][path.front()];
	for (size_t i=0; i < path.size()-1; i++) {
		l += distMatrix[path.at(i)][path[i+1]];
	}
	cout << "path len: " << l << endl;
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
			float dist = pow(points[a].x - points[b].x, 2) + pow(points[a].y - points[b].y, 2);
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

inline void do2opt(int a, int b, vector<int>& path) {
	int m1 = a < b ? a : b;
	int m2 = a >= b ? a : b;
	reverse(path.begin()+m1+1, path.begin()+m2+1);
}

vector<int> simAnnealing2opt(vector<int> path, const vector<vector<float>> dist) {
	srand(RANDOM_SEED); // Seed the random nubmer generator
	auto start = high_resolution_clock::now();
	int64_t duration = duration_cast<microseconds>(high_resolution_clock::now() - start).count();
	int pathLen = path.size();
	auto bestPath = path;
	float bestPathDist = calcPathDist(path, dist);
	float diff = 0;

	long unsigned int countLoops = 0;
	long unsigned int countResets = 0;
	long unsigned int countBadSteps = 0;

	while (duration < CUTOFF_US) {
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

		if (orgDist > swDist) {
			do2opt(a, b, path);
			diff -= orgDist - swDist;

			if (diff < 0) {
				bestPath = path;
				bestPathDist = calcPathDist(path, dist);
				diff = 0;
			}
		} else {
			bool doBadStep = (rand() % CUTOFF_US) > (duration + (CUTOFF_US/2));
			if (doBadStep) {
				do2opt(a, b, path);
				diff -= orgDist - swDist;
				countBadSteps++;
			}
			// Reset if very bad
			if (diff > 0.1*bestPathDist) {
				path = bestPath;
				diff = 0;
				countResets++;
			}
		}

		duration = duration_cast<microseconds>(high_resolution_clock::now() - start).count();
		countLoops++;
	}

	cerr << "loops: " << countLoops << "\tbad steps: " << countBadSteps << "\tresets: " << countResets << endl;

	return bestPath;
}

// TODO: there are may optimizations posible
vector<int> solve(vector<point> points) {
	auto distMatrix = euclideanDistance(points);
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
