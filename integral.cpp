//
// Created by divanets on 2020-11-24.
//
#include <iostream>
#include <cmath>
#include <thread>
#include <vector>
#include <list>
#include "integral.h"

//#define DEBUG true

#ifdef DEBUG

#include <iomanip>
#include "timeit.h"

#endif

double calculate_int(double(*f)(double, double), double x1_from, double x1_to, double x2_from, double x2_to) {

#ifdef DEBUG
    auto start_time = get_current_time_fenced();
#endif
    double res = 0;
    res += f(x1_from, x2_from) * (x1_to - x1_from) * (x2_to - x2_from);

#ifdef DEBUG
    auto step_time = get_current_time_fenced();
    std::cerr << "Thread time: " << to_us(step_time - start_time) << std::endl;
#endif

    return res;
}

void calculate_int_c(double(*f)(double, double), double x1_from, double x1_to, double x2_from, double x2_to,
                     int n, int n1_from, int n1_to, int n2_from, int n2_to, double &result) {

#ifdef DEBUG
    auto start_time = get_current_time_fenced();
#endif

    double res = 0;
    for (int ni = n1_from; ni < n1_to; ni += 1) {
        for (int nj = n2_from; nj < n2_to; nj += 1) {
            if (ni % 2 || nj % 2) {
                double x1 = x1_from + (x1_to - x1_from) * ni / n;
                double x2 = x2_from + (x2_to - x2_from) * nj / n;
                res += f(x1, x2) * (x1_to - x1_from) / n * (x2_to - x2_from) / n;
            }
        }
    }

#ifdef DEBUG
    auto step_time = get_current_time_fenced();
    std::cerr << "Thread time: " << to_us(step_time - start_time) << " Steps: " << (n1_to - n1_from) * (n2_to - n2_from)
              << std::endl;
#endif

    result = res;
}

auto chunkIt(int l, int n) {
    int k = l / n;
    int m = l % n;
    std::list<std::tuple<int, int>> result;

    for (int i = 0; i < n; ++i) {
        int from = i * k + std::min(i, m);
        int to = (i + 1) * k + std::min(i + 1, m);
        if (from < to) {
            result.emplace_back(from, to);
        }
    }

    return result;
}


IntegralStats integrate(int n_threads, double (*int_func)(double, double), double x1_from, double x1_to,
                        double x2_from, double x2_to, double expected_precision, double expected_precision_rel,
                        int max_iterations) {
    double int_curr = 0;
    double precision = 0;
    double precision_rel = 0;
    int n = 1;
    double int_prev = calculate_int(int_func, x1_from, x1_to, x2_from, x2_to);

#ifdef DEBUG
    std::cerr << "Initial Result: " << std::setprecision(15) << int_prev << "\n";
#endif

#ifdef DEBUG
    auto start_time = get_current_time_fenced();
#endif

    for (int k = 0; k < max_iterations; k++) {
        n *= 2;
        std::vector<std::thread> threads;
        std::list<double> results;

        for (auto[from, to]: chunkIt(n, n_threads)) {
            if (to > from) {
                results.push_back(0);
                std::thread thr(calculate_int_c, int_func, x1_from, x1_to, x2_from, x2_to, n,
                                0, n, from, to, std::ref(results.back()));
                threads.push_back(std::move(thr));
            }
        }

        for (auto &thread : threads) {
            thread.join();
        }
        double result = 0;
        for (auto r: results) {
            result += r;
        }

        int_curr = result + int_prev / 4;

        precision = std::abs(int_prev - int_curr);
        precision_rel = precision / int_curr;

#ifdef DEBUG
        auto step_time = get_current_time_fenced();
        std::cerr << "Step:  " << k + 1 << "\tResult: " << std::setprecision(15) << int_curr << "\t Partial: " << result
                  << "\t Precision: " << precision << "/" << precision_rel << "\n";
        std::cerr << "Step time: " << to_us(step_time - start_time) << std::endl;
        start_time = step_time;
#endif

        if ((precision < expected_precision) && (precision_rel < expected_precision_rel)) {
#ifdef DEBUG
            std::cerr << "Break due to precision requirements reached\n";
#endif
            break;
        }

        int_prev = int_curr;
    }
    return {
            int_curr,
            precision, precision_rel
    };
}