#include <iostream>
#include <cmath>
#include <thread>

double de_jong(double x1, double x2) {
    double res = 0.002;
    for (short i = -2; i <= 2; ++i) {
        for (short j = -2; j <= 2; ++j) {
            res += 1 / (5 * (i + 2) + j + 3 + pow(x1 - 16 * j, 6) + pow(x2 - 16 * i, 6));
        }
    }
    return pow(res, -1);
}

//double calculate_int(double(*f)(double, double), double x1_from, double x1_to, double x2_from, double x2_to, double dx){
//    double res = 0;
//    for (double x1 = x1_from; x1 < x1_to; x1 += dx*2) {
//        for (double x2 = x2_from; x2 < x2_to; x2 += dx*2) {
//            res += de_jong(x1, x2) * dx * dx;
//        }
//    }
//    return dx;
//}

void calculate_int(double(*f)(double, double), double x1_from, double x1_to, double x2_from, double x2_to, double n, bool shift_x1, bool shift_x2, double& result){
    double res = 0;
    for (int ni = shift_x1; ni < n; ni+=2) {
        for (int nj = shift_x2; nj < n; nj+=2) {
            double x1 = x1_from + (x1_to - x1_from) * ni / n;
            double x2 = x2_from + (x2_to - x2_from) * nj / n;
            res += f(x1, x2) * (x1_to - x1_from) / n * (x2_to - x2_from) / n;
        }
    }
    result = res;
}

int main() {

    double int_curr = 0;
    int n = 4;
    double int_prev;
    calculate_int(de_jong, -50, 50, -50, 50, n, false, false, std::ref(int_prev));
    int_prev *= 4;
    for (int k = 0; k < 30; k++) {
        double i1=1, i2=2, i3;

        std::thread t1(calculate_int, de_jong, -50, 50, -50, 50, n, true, false, std::ref(i1));
        std::thread t2(calculate_int, de_jong, -50, 50, -50, 50, n, false, true, std::ref(i2));
        std::thread t3(calculate_int, de_jong, -50, 50, -50, 50, n, true, true, std::ref(i3));
        t1.join();
        t2.join();
        t3.join();

        int_curr = i1 + i2 + i3 + int_prev / 4;
        std::cout << int_curr << std::endl;
        n *= 2;

        double err = abs(int_prev - int_curr)

        if (false){
            break;
        }

        int_prev = int_curr;
    }
    std::cout << '\n';
    std::cout << int_prev << std::endl;

    return 0;
}
