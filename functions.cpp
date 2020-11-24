//
// Created by divanets on 2020-11-24.
//
#include <cmath>

double de_jong(double x1, double x2) {
    double res = 0.002;
    for (short i = -2; i <= 2; ++i) {
        for (short j = -2; j <= 2; ++j) {
            res += 1 / (5 * (i + 2) + j + 3 + pow(x1 - 16 * j, 6) + pow(x2 - 16 * i, 6));
        }
    }
    return pow(res, -1);
}