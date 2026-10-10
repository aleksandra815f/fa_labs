#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "logic.h"
#include "parser.h"
#include "status.h"

static void print_status_message(const Status status)
{
    switch (status) {
        case STATUS_OK:
            break;
        case STATUS_NULL:
            fprintf(stderr, "внутренняя ошибка: передан пустой указатель\n");
            break;
        case STATUS_BAD_ARGC:
            fprintf(stderr, "неверное количество аргументов командной строки\n");
            break;
        case STATUS_BAD_FLAG:
            fprintf(stderr, "неизвестный флаг: допустимы q, m, t\n");
            break;
        case STATUS_BAD_NUMBER:
            fprintf(stderr, "не является корректным числом (или inf/nan, или слишком большое)\n");
            break;
        case STATUS_BAD_EPS:
            fprintf(stderr, "эпсилон должен быть конечным и строго больше 0\n");
            break;
        case STATUS_ZERO:
            fprintf(stderr, "числа должны быть ненулевыми\n");
            break;
        case STATUS_NOT_POSITIVE:
            fprintf(stderr, "длина стороны должна быть больше эпсилон\n");
            break;
        case STATUS_OVERFLOW:
            fprintf(stderr, "переполнение при вычислениях\n");
            break;
        case STATUS_SMALL_BUFFER:
            fprintf(stderr, "внутренняя ошибка: слишком маленький буфер\n");
            break;
        default:
            fprintf(stderr, "неизвестная ошибка\n");
            break;
    }
}
//сколько знаков после ,
static int precision_from_eps(const double eps)
{
    const int digits = (int) ceil(-log10(eps));
    if (digits < 0) {
        return 0;
    }
    if (digits > 15) {
        return 15;
    }
    return digits;
}

static void print_real(const double x, const double eps, const int digits)
{
    double shown = x;
    if (fabs(x) <= eps) {
        shown = 0.0;
    }

    if (fabs(shown) >= 1e15) {
        printf("%.*e", digits, shown);
    } else {
        printf("%.*f", digits, shown);
    }
}

static Status parse_real_param(const char *arg, const int position, double *out)
{
    const Status status = parse_double(arg, out);

    if (status != STATUS_OK) {
        fprintf(stderr, "Ошибка в параметре %d (\"%s\"): ", position, arg);
        print_status_message(status);
    }
    return status;
}

static Status parse_integer_param(const char *arg, const int position, long long *out)
{
    const Status status = parse_integer(arg, out);

    if (status != STATUS_OK) {
        fprintf(stderr, "Ошибка в параметре %d (\"%s\"): ", position, arg);
        print_status_message(status);
    }
    return status;
}

static void print_solution(const SolutionKind kind, const double x1, const double x2,
                           const double eps, const int digits)
{
    switch (kind) {
        case SOLUTION_TWO_ROOTS:
            printf("x1 = ");
            print_real(x1, eps, digits);
            printf(", x2 = ");
            print_real(x2, eps, digits);
            break;
        case SOLUTION_ONE_ROOT:
            printf("один корень кратности 2: x = ");
            print_real(x1, eps, digits);
            break;
        case SOLUTION_NO_ROOTS:
            printf("вещественных корней нет (D < 0)");
            break;
        case SOLUTION_LINEAR:
            printf("уравнение линейное (a = 0): x = ");
            print_real(x1, eps, digits);
            break;
        case SOLUTION_NONE:
            printf("решений нет (a = 0, b = 0, c != 0)");
            break;
        case SOLUTION_ANY:
            printf("решение — любое x (a = b = c = 0)");
            break;
    }
}
static Status run_quadratic(char *const argv[])
{
    double eps;
    double a;
    double b;
    double c;

    Status status = parse_real_param(argv[2], 1, &eps);
    if (status != STATUS_OK) {
        return status;
    }
    status = parse_real_param(argv[3], 2, &a);
    if (status != STATUS_OK) {
        return status;
    }
    status = parse_real_param(argv[4], 3, &b);
    if (status != STATUS_OK) {
        return status;
    }
    status = parse_real_param(argv[5], 4, &c);
    if (status != STATUS_OK) {
        return status;
    }

    Coefficients perms[MAX_PERMUTATIONS];
    int count = 0;

    status = make_permutations(a, b, c, eps, perms, MAX_PERMUTATIONS, &count);
    if (status != STATUS_OK) {
        fprintf(stderr, "Ошибка: ");
        print_status_message(status);
        return status;
    }

    const int digits = precision_from_eps(eps);
    printf("Уникальных перестановок: %d\n", count);

    for (int i = 0; i < count; i++) {
        printf("%d) (", i + 1);
        print_real(perms[i].a, eps, digits);
        printf(")*x^2 + (");
        print_real(perms[i].b, eps, digits);
        printf(")*x + (");
        print_real(perms[i].c, eps, digits);
        printf(") = 0: ");

        SolutionKind kind;
        double x1;
        double x2;
        status = solve_quadratic(perms[i].a, perms[i].b, perms[i].c,
                                 eps, &kind, &x1, &x2);

        if (status == STATUS_OK) {
            print_solution(kind, x1, x2, eps, digits);
            printf("\n");
        } else if (status == STATUS_OVERFLOW) {
            printf("не удалось решить: переполнение при вычислениях\n");
        } else {
            printf("\n");
            fprintf(stderr, "Ошибка: ");
            print_status_message(status);
            return status;
        }
    }
    return STATUS_OK;
}

static Status run_multiple(char *const argv[])
{
    long long first;
    long long second;

    Status status = parse_integer_param(argv[2], 1, &first);
    if (status != STATUS_OK) {
        return status;
    }
    status = parse_integer_param(argv[3], 2, &second);
    if (status != STATUS_OK) {
        return status;
    }

    bool result = false;
    status = is_multiple(first, second, &result);
    if (status != STATUS_OK) {
        fprintf(stderr, "Ошибка: ");
        print_status_message(status);
        return status;
    }

    if (result) {
        printf("%lld кратно %lld\n", first, second);
    } else {
        printf("%lld не кратно %lld\n", first, second);
    }
    return STATUS_OK;
}

// -t: eps, a, b, c 
static Status run_triangle(char *const argv[])
{
    double eps;
    double sides[3];

    Status status = parse_real_param(argv[2], 1, &eps);
    if (status != STATUS_OK) {
        return status;
    }
    for (int i = 0; i < 3; i++) {
        status = parse_real_param(argv[3 + i], 2 + i, &sides[i]);
        if (status != STATUS_OK) {
            return status;
        }
    }

    bool result = false;
    status = is_right_triangle(sides[0], sides[1], sides[2], eps, &result);
    if (status != STATUS_OK) {
        fprintf(stderr, "Ошибка: ");
        print_status_message(status);
        return status;
    }

    const int digits = precision_from_eps(eps);

    if (result) {
        printf("Могут: треугольник со сторонами ");
        print_real(sides[0], eps, digits);
        printf(", ");
        print_real(sides[1], eps, digits);
        printf(", ");
        print_real(sides[2], eps, digits);
        printf(" является прямоугольным\n");
    } else {
        printf("Не могут: теорема Пифагора не выполняется ни при каком выборе гипотенузы\n");
    }
    return STATUS_OK;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Ошибка: не указан флаг. Использование: program <flag> <параметры>\n");
        return 1;
    }

    Flag flag;
    Status status = parse_flag(argv[1], &flag);
    if (status != STATUS_OK) {
        fprintf(stderr, "Ошибка в флаге \"%s\": ", argv[1]);
        print_status_message(status);
        return 1;
    }

    int needed = 0;
    status = params_count(flag, &needed);
    if (status != STATUS_OK) {
        fprintf(stderr, "Ошибка: ");
        print_status_message(status);
        return 1;
    }

    if (argc != 2 + needed) {
        fprintf(stderr, "Ошибка: для флага %s нужно параметров: %d, передано: %d\n",
                argv[1], needed, argc - 2);
        return 1;
    }

    switch (flag) {
        case FLAG_Q:
            status = run_quadratic(argv);
            break;
        case FLAG_M:
            status = run_multiple(argv);
            break;
        case FLAG_T:
            status = run_triangle(argv);
            break;
    }

    if (status != STATUS_OK) {
        return 1;
    }
    return 0;
}