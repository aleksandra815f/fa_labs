#include "constants.h"

#include <math.h>
#include <stdlib.h>

typedef double (*Function)(double x);

#define MAX_DOUBLINGS 26   // n удваивается: максимум 2^26
#define MAX_TERMS 100000000L    // максимум слагаемых для медленных рядов
#define MAX_PI_LIMIT 20000000L   // максимум n для предела числа pi
#define MAX_GAMMA_M 36     // максимум m для предела числа gamma
#define MAX_GAMMA_GROUP 10000L // максимум групп j для ряда числа gamma
#define MAX_PRIME_LIMIT 16777216L // 2^24, граница решета для gamma
#define MAX_BISECTION_STEPS 200  // максимум шагов метода дихотомии

Status check_eps(const double eps)
{
    if (!isfinite(eps) || eps < MIN_EPS || eps >= 1.0) {
        return STATUS_BAD_EPS;
    }
    return STATUS_OK;
}
static Status check_args(const double eps, const double *result)
{
    if (result == NULL) {
        return STATUS_NULL;
    }
    return check_eps(eps);
}
//Метод дихотомии (деления отрезка пополам)
static Status bisection(const Function f, double left, double right,
                        const double eps, double *result)
{
    if (f == NULL || result == NULL) {
        return STATUS_NULL;
    }
    if (left >= right || f(left) * f(right) > 0.0) {
        return STATUS_BAD_INTERVAL;
    }

    for (int step = 0; step < MAX_BISECTION_STEPS; step++) {
        if (right - left < eps) {
            *result = (left + right) / 2.0;
            return STATUS_OK;
        }

        double middle = (left + right) / 2.0;
        if (f(left) * f(middle) <= 0.0) {
            right = middle;
        } else {
            left = middle;
        }
    }
    return STATUS_NOT_CONVERGED;
}

Status e_limit(const double eps, double *result){
    Status status = check_args(eps, result);
    if (status != STATUS_OK) {
        return status;
    }

    double n = 1.0;
    double previous = 2.0;  /* (1 + 1/1)^1 */
    for (int step = 1; step <= MAX_DOUBLINGS; step++) {
        n = n * 2.0;
        double current = pow(1.0 + 1.0 / n, n);
        if (fabs(current - previous) < eps) {
            *result = current;
            return STATUS_OK;
        }
        previous = current;
    }
    return STATUS_NOT_CONVERGED;
}

Status e_series(const double eps, double *result)
{
    Status status = check_args(eps, result);
    if (status != STATUS_OK) {
        return status;
    }

    double sum = 1.0;  //первое слагаемое 1/0!
    double term = 1.0; //теукущее
    for (long n = 1; n <= 100; n++) {
        term = term / (double)n;     /* теперь term = 1/n! */
        sum = sum + term;
        if (term < eps) {            /* остаток ряда меньше последнего слагаемого */
            *result = sum;
            return STATUS_OK;
        }
    }
    return STATUS_NOT_CONVERGED;
}
static double e_function(double x)
{
    return log(x) - 1.0;
}

Status e_equation(const double eps, double *result)
{
    Status status = check_args(eps, result);
    if (status != STATUS_OK) {
        return status;
    }
    return bisection(e_function, 1.0, 4.0, eps, result);
}

Status pi_limit(const double eps, double *result)
{
    Status status = check_args(eps, result);
    if (status != STATUS_OK) {
        return status;
    }

    double a = 1.0;
    for (long n = 1; n <= MAX_PI_LIMIT; n++) {
        double d = (double)n;
        a = a * (4.0 * d * d) / ((2.0 * d - 1.0) * (2.0 * d - 1.0));
        double p = a / d;
        if (p / (2.0 * d) < eps) {
            *result = p;
            return STATUS_OK;
        }
    }
    return STATUS_NOT_CONVERGED;
}

Status pi_series(const double eps, double *result){
    Status status = check_args(eps, result);
    if (status != STATUS_OK) {
        return status;
    }

    double sum = 0.0;
    double sign = 1.0;
    for (long n = 1; n <= MAX_TERMS; n++) {
        double term = 4.0 / (2.0 * (double)n - 1.0);
        sum = sum + sign * term;
        if (term < eps) {
            *result = sum;
            return STATUS_OK;
        }
        sign = -sign;
    }
    return STATUS_NOT_CONVERGED;
}

/* cos x = -1 касается нуля, но не меняет знак (cos x + 1 >= 0 всегда),
   поэтому для дихотомии берём sin x = 0: на отрезке [3, 4] у него тот же корень pi */
static double pi_function(double x)
{
    return sin(x);
}

Status pi_equation(const double eps, double *result)
{
    Status status = check_args(eps, result);
    if (status != STATUS_OK) {
        return status;
    }
    return bisection(pi_function, 3.0, 4.0, eps, result);
}

Status ln2_limit(const double eps, double *result)
{
    Status status = check_args(eps, result);
    if (status != STATUS_OK) {
        return status;
    }

    double n = 1.0;
    double previous = 1.0;                   // 1 * (2^1 - 1) 
    for (int step = 1; step <= MAX_DOUBLINGS; step++) {
        n = n * 2.0;
        double current = n * (pow(2.0, 1.0 / n) - 1.0);
        if (fabs(current - previous) < eps) {
            *result = current;
            return STATUS_OK;
        }
        previous = current;
    }
    return STATUS_NOT_CONVERGED;
}

Status ln2_series(const double eps, double *result)
{
    Status status = check_args(eps, result);
    if (status != STATUS_OK) {
        return status;
    }

    double sum = 0.0;
    double sign = 1.0;
    for (long n = 1; n <= MAX_TERMS; n++) {
        double term = 1.0 / (double)n;
        sum = sum + sign * term;
        if (term < eps) {
            *result = sum;
            return STATUS_OK;
        }
        sign = -sign;
    }
    return STATUS_NOT_CONVERGED;
}

static double ln2_function(double x)
{
    return exp(x) - 2.0;
}

Status ln2_equation(const double eps, double *result)
{
    Status status = check_args(eps, result);
    if (status != STATUS_OK) {
        return status;
    }
    return bisection(ln2_function, 0.0, 1.0, eps, result);
}

Status sqrt2_limit(const double eps, double *result)
{
    Status status = check_args(eps, result);
    if (status != STATUS_OK) {
        return status;
    }

    double x = -0.5;
    for (int n = 1; n <= 1000; n++) {
        double next = x - x * x / 2.0 + 1.0;
        if (fabs(next - x) < eps) {
            *result = next;
            return STATUS_OK;
        }
        x = next;
    }
    return STATUS_NOT_CONVERGED;
}

Status sqrt2_product(const double eps, double *result)
{
    Status status = check_args(eps, result);
    if (status != STATUS_OK) {
        return status;
    }

    double product = 1.0;
    double power = 0.25;                    
    for (int k = 2; k <= 60; k++) {
        double next = product * pow(2.0, power);
        if (2.0 * fabs(next - product) < eps) {
            *result = next;
            return STATUS_OK;
        }
        product = next;
        power = power / 2.0;
    }
    return STATUS_NOT_CONVERGED;
}

static double sqrt2_function(double x)
{
    return x * x - 2.0;
}

Status sqrt2_equation(const double eps, double *result)
{
    Status status = check_args(eps, result);
    if (status != STATUS_OK) {
        return status;
    }
    return bisection(sqrt2_function, 1.0, 2.0, eps, result);
}



Status gamma_limit(const double eps, double *result)
{
    Status status = check_args(eps, result);
    if (status != STATUS_OK) {
        return status;
    }

    double previous = 0.0;
    for (int m = 1; m <= MAX_GAMMA_M; m++) {
        double sum = 0.0;
        double binom = 1.0;       // C(m, 0)
        double log_fact = 0.0;    // ln(0!) 

        for (int k = 1; k <= m; k++) {
            binom = binom * (double)(m - k + 1) / (double)k;   /* C(m,k) из C(m,k-1) */
            log_fact = log_fact + log((double)k);              /* ln(k!) из ln((k-1)!) */
            double term = binom * log_fact / (double)k;
            if (k % 2 == 1) {
                sum = sum - term;
            } else {
                sum = sum + term;
            }
        }

        if (m >= 2 && (double)m * fabs(sum - previous) < eps) {
            *result = sum;
            return STATUS_OK;
        }
        previous = sum;
    }
    return STATUS_NOT_CONVERGED;
}

Status gamma_series(const double eps, double *result)
{
    Status status = check_args(eps, result);
    if (status != STATUS_OK) {
        return status;
    }

    const double pi = acos(-1.0);
    double sum = -(pi * pi) / 6.0;

    for (long j = 1; j <= MAX_GAMMA_GROUP; j++) {
        double inverse_square = 1.0 / ((double)j * (double)j);
        long first = (j == 1) ? 2 : j * j;
        long last = (j + 1) * (j + 1) - 1;

        for (long k = first; k <= last; k++) {
            sum = sum + inverse_square - 1.0 / (double)k;
        }

        if (2.0 / (double)j < eps) {
            *result = sum;
            return STATUS_OK;
        }
    }
    return STATUS_NOT_CONVERGED;
}

Status gamma_equation(const double eps, double *result)
{
    Status status = check_args(eps, result);
    if (status != STATUS_OK) {
        return status;
    }

    /* is_composite[i] == 1 значит "число i составное". calloc заполняет память нулями */
    char *is_composite = calloc((size_t)MAX_PRIME_LIMIT + 1, sizeof(char));
    if (is_composite == NULL) {
        return STATUS_NO_MEMORY;
    }

    Status answer = STATUS_NOT_CONVERGED;
    double product = 1.0;
    long next_check = 1024;

    for (long i = 2; i <= MAX_PRIME_LIMIT; i++) {
        if (is_composite[i] == 0) {                  /* i - простое */
            product = product * ((double)i - 1.0) / (double)i;
            if (i <= MAX_PRIME_LIMIT / i) {          /* i * i не выйдет за границу */
                for (long j = i * i; j <= MAX_PRIME_LIMIT; j += i) {
                    is_composite[j] = 1;
                }
            }
        }

        if (i == next_check) {
            if (1.0 / sqrt((double)i) < eps) {
                *result = -log(log((double)i) * product);
                answer = STATUS_OK;
                break;
            }
            next_check = next_check * 2;
        }
    }

    free(is_composite);
    return answer;
}