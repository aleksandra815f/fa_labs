#include <stdio.h>

#include "constants.h"
#include "parser.h"
#include "status.h"

typedef Status (*Method)(const double eps, double *result);
static void run_method(const char *name, const Method method, const double eps){
    double value = 0.0;
    Status status = method(eps, &value);

    switch (status) {
        case STATUS_OK:
            printf("  %-14s %.12f\n", name, value);
            break;
        case STATUS_NOT_CONVERGED:
            printf("  %-14s не сошлось: такая точность недостижима этим методом\n", name);
            break;
        case STATUS_BAD_INTERVAL:
            printf("  %-14s ошибка: на отрезке нет смены знака\n", name);
            break;
        case STATUS_NO_MEMORY:
            printf("  %-14s ошибка: не хватило памяти\n", name);
            break;
        case STATUS_BAD_EPS:
            printf("  %-14s ошибка: недопустимое значение eps\n", name);
            break;
        default:
            printf("  %-14s внутренняя ошибка\n", name);
            break;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Ошибка: нужен один аргумент - eps. Пример: ./task2 0.0001\n");
        return 1;
    }

    double eps = 0.0;
    Status status = parse_double(argv[1], &eps);
    if (status != STATUS_OK) {
        fprintf(stderr, "Ошибка: eps должно быть конечным вещественным числом.\n");
        return 1;
    }

    status = check_eps(eps);
    if (status != STATUS_OK) {
        fprintf(stderr, "Ошибка: eps должно быть в диапазоне от %g (включительно) до 1.\n", MIN_EPS);
        return 1;
    }

    printf("eps = %g\n", eps);

    printf("e:\n");
    run_method("предел", e_limit, eps);
    run_method("ряд", e_series, eps);
    run_method("уравнение", e_equation, eps);

    printf("pi:\n");
    run_method("предел", pi_limit, eps);
    run_method("ряд", pi_series, eps);
    run_method("уравнение", pi_equation, eps);

    printf("ln 2:\n");
    run_method("предел", ln2_limit, eps);
    run_method("ряд", ln2_series, eps);
    run_method("уравнение", ln2_equation, eps);

    printf("sqrt(2):\n");
    run_method("предел", sqrt2_limit, eps);
    run_method("произведение", sqrt2_product, eps);
    run_method("уравнение", sqrt2_equation, eps);

    printf("gamma:\n");
    run_method("предел", gamma_limit, eps);
    run_method("ряд", gamma_series, eps);
    run_method("уравнение", gamma_equation, eps);

    return 0;
}