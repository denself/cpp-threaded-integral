//
// Created by divanets on 2020-11-24.
//
#include <iostream>
#include <cmath>
#include <thread>
#include <iomanip>
#include <vector>
#include "integral.h"

double calculate_int(double(*f)(double, double), double x1_from, double x1_to, double x2_from, double x2_to,
                     double n1, double n2, bool shift_x1, bool shift_x2) {
    double res = 0;
    for (int ni = shift_x1; ni < n1; ni += 2) {
        for (int nj = shift_x2; nj < n2; nj += 2) {
            double x1 = x1_from + (x1_to - x1_from) * ni / n1;
            double x2 = x2_from + (x2_to - x2_from) * nj / n2;
            res += f(x1, x2) * (x1_to - x1_from) / n1 * (x2_to - x2_from) / n2;
        }
    }
    return res;
}

void calculate_int_chunk(double(*f)(double, double), double x1_from, double x1_to, double x2_from, double x2_to,
                         double n1, double n2, double &result) {

    result = calculate_int(f, x1_from, x1_to, x2_from, x2_to, n1, n2, true, false) +
             calculate_int(f, x1_from, x1_to, x2_from, x2_to, n1, n2, false, true) +
             calculate_int(f, x1_from, x1_to, x2_from, x2_to, n1, n2, true, true);
}

IntegralStats integrate(int n_threads, double (*int_func)(double, double), double x1_from, double x1_to,
                        double x2_from, double x2_to, double expected_precision, double expected_precision_rel,
                        int max_iterations) {
    double int_curr = 0;
    double precision = 0;
    double precision_rel = 0;
    int n = n_threads;
    double int_prev = calculate_int(int_func, x1_from, x1_to, x2_from, x2_to, n, n, false, false) * 4;
    for (int k = 0; k < max_iterations; k++) {
        std::vector<std::thread> threads;
        std::vector<double> results(n_threads);
        double result = 0;
        for (int i = 0; i < n_threads; ++i) {
            double x2_from_i = (n_threads * x2_from + i * (x2_to - x2_from)) / n_threads;
            double x2_to_i = (n_threads * x2_from + (i + 1) * (x2_to - x2_from)) / n_threads;
            std::thread thr(calculate_int_chunk, int_func, x1_from, x1_to,
                            x2_from_i, x2_to_i, n, n / n_threads, std::ref(results[i]));
            threads.push_back(std::move(thr));
        }

        for (int i = 0; i < n_threads; ++i) {
            threads[i].join();
            result += results[i];
        }

        int_curr = result + int_prev / 4;
        std::cout << std::setprecision(12) << int_curr << std::endl;
        n *= 2;

        precision = std::abs(int_prev - int_curr);
        precision_rel = precision / int_curr;

        if (precision < expected_precision) {
            break;
        }
        if (precision_rel < expected_precision_rel) {
            break;
        }

        int_prev = int_curr;
    }
    return {
        int_curr,
        precision, precision_rel
    };
}