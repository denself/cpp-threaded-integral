#include <iostream>
#include <cmath>
#include <thread>
#include <iomanip>
#include <vector>
#include "functions.h"

double calculate_int(double(*f)(double, double), double x1_from, double x1_to, double x2_from, double x2_to,
                   double n1, double n2, bool shift_x1, bool shift_x2){
    double res = 0;
    for (int ni = shift_x1; ni < n1; ni+=2) {
        for (int nj = shift_x2; nj < n2; nj+=2) {
            double x1 = x1_from + (x1_to - x1_from) * ni / n1;
            double x2 = x2_from + (x2_to - x2_from) * nj / n2;
            res += f(x1, x2) * (x1_to - x1_from) / n1 * (x2_to - x2_from) / n2;
        }
    }
    return res;
}

void calculate_int_chunk(double(*f)(double, double), double x1_from, double x1_to, double x2_from, double x2_to,
                         double n1, double n2, double& result) {

    result = calculate_int(de_jong, x1_from, x1_to, x2_from, x2_to, n1, n2, true, false) +
             calculate_int(de_jong, x1_from, x1_to, x2_from, x2_to, n1, n2, false, true) +
             calculate_int(de_jong, x1_from, x1_to, x2_from, x2_to, n1, n2, true, true);
}

int main() {

    double int_curr = 0;
    int n_threads = 4;
    int n = n_threads;

    double x1_from = -50;
    double x1_to = 50;
    double x2_from = -50;
    double x2_to = 50;

    double int_prev = calculate_int(de_jong, x1_from, x1_to, x2_from, x2_to, n, n, false, false) * 4;
    for (int k = 0; k < 30; k++) {
        std::vector<std::thread> threads;
        std::vector<double> results(n_threads);
        double result = 0;
        for (int i = 0; i < n_threads; ++i){
            double x2_from_i = (n_threads * x2_from + i * (x2_to - x2_from)) / n_threads;
            double x2_to_i = (n_threads * x2_from + (i+1) * (x2_to - x2_from)) / n_threads;
            std::thread thr(calculate_int_chunk, de_jong, x1_from, x1_to,
                            x2_from_i, x2_to_i, n, n / n_threads, std::ref(results[i]));
            threads.push_back(std::move(thr));
        }

        for (int i = 0; i < n_threads; ++i){
            threads[i].join();
            result += results[i];
        }

        int_curr = result + int_prev / 4;
        std::cout << std::setprecision(12) << int_curr << std::endl;
        n *= 2;

        double err = std::abs(int_prev - int_curr) / int_curr;

        if (err < 0.000001){
            break;
        }

        int_prev = int_curr;
    }
    std::cout << '\n';
    std::cout << int_curr << std::endl;

    return 0;
}
