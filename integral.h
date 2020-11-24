//
// Created by divanets on 2020-11-24.
//

#ifndef THREADEDINTEGRAL_INTEGRAL_H
#define THREADEDINTEGRAL_INTEGRAL_H

struct IntegralStats {
    double result;
    double precision, precision_rel;
};

IntegralStats integrate(int n_threads, double (*int_func)(double, double), double x1_from, double x1_to,
                        double x2_from, double x2_to, double expected_precision, double expected_precision_rel,
                        int max_iterations);

#endif //THREADEDINTEGRAL_INTEGRAL_H
