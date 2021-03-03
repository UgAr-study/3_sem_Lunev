//
// Created by artem on 01.03.2021.
//

double TrueIntegral (double begin, double end, int n_steps, double (*foo) (double x)) {

    if (end < begin) {
        double tmp = end;
        end = begin;
        begin = tmp;
    }

    const double step = (end - begin) / n_steps;

    double res = 0;
    double x2 = begin + step;
    double f1 = foo (begin);
    double f2 = foo (x2);

    for (int i = 0; i < n_steps; ++i) {

        res += step * (f1 + f2) / 2;
        f1 = f2;
        x2 += step;
        f2 = foo (x2);
    }

    return res;
}