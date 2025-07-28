#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <cmath>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

struct Point {
    long long x;
    long long y;
};

// Convert string number in given base to decimal manually
long long baseToDecimal(const string& number, int base) {
    long long result = 0;
    long long multiplier = 1;
    for (int i = number.length() - 1; i >= 0; --i) {
        int digit = number[i] - '0';
        if (digit >= base) throw invalid_argument("Invalid digit in base conversion");
        result += digit * multiplier;
        multiplier *= base;
    }
    return result;
}

// Function to calculate Lagrange Interpolation at x = 0
long long lagrangeInterpolation(const vector<Point>& points) {
    long long secret = 0;

    for (int i = 0; i < points.size(); ++i) {
        long long xi = points[i].x;
        long long yi = points[i].y;

        long long num = 1;
        long long denom = 1;

        for (int j = 0; j < points.size(); ++j) {
            if (i == j) continue;
            long long xj = points[j].x;
            num *= -xj;
            denom *= (xi - xj);
        }

        secret += yi * (num / denom);  // Assume denom divides num cleanly (int arithmetic)
    }

    return secret;
}

// Check all combinations of nCk and return majority secret (filter wrong ones)
long long majoritySecret(const vector<Point>& allPoints, int k) {
    int n = allPoints.size();
    map<long long, int> frequency;

    // Generate all combinations of size k
    vector<int> indices(n);
    for (int i = 0; i < k; ++i) indices[i] = 1;
    sort(indices.begin(), indices.end());

    do {
        vector<Point> subset;
        for (int i = 0; i < n; ++i) {
            if (indices[i]) subset.push_back(allPoints[i]);
        }

        try {
            long long secret = lagrangeInterpolation(subset);
            frequency[secret]++;
        } catch (...) {
            continue;
        }

    } while (next_permutation(indices.begin(), indices.end()));

    // Return the most frequent result
    long long maxSecret = 0;
    int maxFreq = 0;
    for (auto& pair : frequency) {
        if (pair.second > maxFreq) {
            maxFreq = pair.second;
            maxSecret = pair.first;
        }
    }

    return maxSecret;
}

int main() {
    ifstream file("test_case1.json");
    if (!file.is_open()) {
        cerr << "Failed to open JSON file." << endl;
        return 1;
    }

    json data;
    file >> data;

    int n = data["keys"]["n"];
    int k = data["keys"]["k"];

    vector<Point> points;

    for (auto& el : data.items()) {
        if (el.key() == "keys") continue;

        long long x = stoll(el.key());
        string baseStr = el.value()["base"];
        string valueStr = el.value()["value"];

        int base = stoi(baseStr);
        long long y = baseToDecimal(valueStr, base);

        points.push_back({x, y});
    }

    cout << "Points from JSON:\n";
    for (auto& p : points) {
        cout << "(" << p.x << ", " << p.y << ")\n";
    }

    long long secret = majoritySecret(points, k);
    cout << "\nReconstructed Secret (constant term): " << secret << endl;

    return 0;
}
