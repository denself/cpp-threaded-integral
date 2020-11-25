#include <iostream>
#include <fstream>
#include <iomanip>
#include "functions.h"
#include "integral.h"
#include "timeit.h"


int main(int argc, char ** argv) {

    if (argc != 2) {
        std::cout << "Incorrect number of arguments. Provide path to configuration file\n";
        return 1;
    }
    auto file = std::ifstream(argv[1]);
    if (!file) {
        std::cout << "Incorrect path to config file. Provide a valid path\n";
        return 1;
    }

    int n_threads = 4;

    double x1_from = -50;
    double x1_to = 50;
    double x2_from = -50;
    double x2_to = 50;
    double (*int_func)(double, double) = de_jong;
    double expected_precision = 0.001;
    double expected_precision_rel = 0.000001;
    int max_iterations = 30;

    std::string skip;
    file >> expected_precision;
    if (expected_precision <= 0) {
        std::cout << "Precision must be grater then 0";
        return 1;
    }
    std::getline(file, skip);
    file >> expected_precision_rel;
    if (expected_precision_rel <= 0) {
        std::cout << "Relative precision must be grater then 0";
        return 1;
    }
    std::getline(file, skip);
    file >> n_threads;
    if (n_threads <= 0) {
        std::cout << "Number of threads must be grater then 0";
        return 1;
    }
    std::getline(file, skip);
    file >> x1_from;
    std::getline(file, skip);
    file >> x1_to;
    if (x1_to <= x1_from) {
        std::cout << "x1_from must be less then x1_to";
        return 1;
    }
    std::getline(file, skip);
    file >> x2_from;
    std::getline(file, skip);
    file >> x2_to;
    if (x2_to <= x2_from) {
        std::cout << "x2_from must be less then x2_to";
        return 1;
    }
    std::getline(file, skip);
    file >> max_iterations;
    if (max_iterations <= 0) {
        std::cout << "Maximum iterations must be grater then 0";
        return 1;
    }
    std::getline(file, skip);

    auto stage1_start_time = get_current_time_fenced();

    auto int_curr = integrate(n_threads, int_func, x1_from, x1_to, x2_from, x2_to, expected_precision,
                                expected_precision_rel, max_iterations);
    auto finish_time = get_current_time_fenced();
    std::cout << std::setprecision(12) << int_curr.result << std::endl;
    std::cout << std::setprecision(12) << int_curr.precision << std::endl;
    std::cout << std::setprecision(12) << int_curr.precision_rel << std::endl;
    std::cout << std::setprecision(12) << to_us(finish_time - stage1_start_time) << std::endl;

    return 0;
}
