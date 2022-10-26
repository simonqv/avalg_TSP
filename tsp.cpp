#include <iostream>
#include <vector>
#include <tuple>
#include <cmath>


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
	size_t n = points.size();
	vector<vector<float>> out(n);
	for (size_t i = 0; i < n; i++) {
		out[i] = vector<float>(n);
	}

	for (size_t a = 0; a < n; a++) {
		for (size_t b = 0; b < n; b++) {
			out[a][b] = pow(points[a].x - points[b].x, 2) + pow(points[a].y - points[b].y, 2);
		}
	}

	return out;
}

int main() {
	cout << "Hello World!" << endl;

	auto points = parseInput();
	for (auto p : points)
		cout << p.x << ", " << p.y << endl;

	auto dists = euclideanDistance(points);
	for (auto dv : dists) {
		for (auto d : dv) 
			cout << d << "\t";
		cout << endl;
	}
	return 0;
}
