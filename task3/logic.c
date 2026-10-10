#include "logic.h"

#include <math.h>
#include <stddef.h>

static bool is_close(const double x, const double y, const double eps)
{
    return fabs(x - y) <= eps;
}

Status make_permutations(const double a, const double b, const double c,
                         const double eps,
                         Coefficients *result, const int capacity, int *count)
{
    if (result == NULL || count == NULL) {
        return STATUS_NULL;
    }
    if (!isfinite(a) || !isfinite(b) || !isfinite(c)) {
        return STATUS_BAD_NUMBER;
    }
    if (!isfinite(eps) || eps <= 0.0) {
        return STATUS_BAD_EPS;
    }
    if (capacity < MAX_PERMUTATIONS) {
        return STATUS_SMALL_BUFFER;
    }

    const double values[3] = {a, b, c};

    const int order[MAX_PERMUTATIONS][3] = {
        {0, 1, 2}, {0, 2, 1},
        {1, 0, 2}, {1, 2, 0},
        {2, 0, 1}, {2, 1, 0}
    };

    int found = 0;
    for (int i = 0; i < MAX_PERMUTATIONS; i++) {
        Coefficients candidate;
        candidate.a = values[order[i][0]];
        candidate.b = values[order[i][1]];
        candidate.c = values[order[i][2]];

        bool duplicate = false;
        for (int j = 0; j < found; j++) {
            if (is_close(candidate.a, result[j].a, eps)
                && is_close(candidate.b, result[j].b, eps)
                && is_close(candidate.c, result[j].c, eps)) {
                duplicate = true;
                break;
            }
        }

        if (!duplicate) {
            result[found] = candidate;
            found++;
        }
    }

    *count = found;
    return STATUS_OK;
}

Status solve_quadratic(const double a, const double b, const double c,
                       const double eps,
                       SolutionKind *kind, double *x1, double *x2)
{
    if (kind == NULL || x1 == NULL || x2 == NULL) {
        return STATUS_NULL;
    }
    if (!isfinite(a) || !isfinite(b) || !isfinite(c)) {
        return STATUS_BAD_NUMBER;
    }
    if (!isfinite(eps) || eps <= 0.0) {
        return STATUS_BAD_EPS;
    }

    *x1 = 0.0;
    *x2 = 0.0;
    if (fabs(a) <= eps) {
        if (fabs(b) <= eps) {
            if (fabs(c) <= eps) {
                *kind = SOLUTION_ANY;
            } else {
                *kind = SOLUTION_NONE;
            }
            return STATUS_OK;
        }

        const double root = -c / b;
        if (!isfinite(root)) {
            return STATUS_OVERFLOW;
        }
        *kind = SOLUTION_LINEAR;
        *x1 = root;
        return STATUS_OK;
    }

    const double discriminant = b * b - 4.0 * a * c;
    if (!isfinite(discriminant)) {
        return STATUS_OVERFLOW;
    }

    if (fabs(discriminant) <= eps) {
        const double root = -b / (2.0 * a);
        if (!isfinite(root)) {
            return STATUS_OVERFLOW;
        }
        *kind = SOLUTION_ONE_ROOT;
        *x1 = root;
        return STATUS_OK;
    }

    if (discriminant < -eps) {
        *kind = SOLUTION_NO_ROOTS;
        return STATUS_OK;
    }

    const double sqrt_d = sqrt(discriminant);
    const double root1 = (-b - sqrt_d) / (2.0 * a);
    const double root2 = (-b + sqrt_d) / (2.0 * a);
    if (!isfinite(root1) || !isfinite(root2)) {
        return STATUS_OVERFLOW;
    }

    *kind = SOLUTION_TWO_ROOTS;
    *x1 = root1;
    *x2 = root2;
    return STATUS_OK;
}

Status is_multiple(const long long first, const long long second, bool *result)
{
    if (result == NULL) {
        return STATUS_NULL;
    }
    if (first == 0 || second == 0) {
        return STATUS_ZERO;
    }

    if (second == 1 || second == -1) {
        *result = true;
        return STATUS_OK;
    }

    *result = (first % second == 0);
    return STATUS_OK;
}

Status is_right_triangle(const double a, const double b, const double c,
                         const double eps, bool *result)
{
    if (result == NULL) {
        return STATUS_NULL;
    }
    if (!isfinite(a) || !isfinite(b) || !isfinite(c)) {
        return STATUS_BAD_NUMBER;
    }
    if (!isfinite(eps) || eps <= 0.0) {
        return STATUS_BAD_EPS;
    }
    if (a <= eps || b <= eps || c <= eps) {
        return STATUS_NOT_POSITIVE;
    }

    const double a2 = a * a;
    const double b2 = b * b;
    const double c2 = c * c;
    if (!isfinite(a2) || !isfinite(b2) || !isfinite(c2)) {
        return STATUS_OVERFLOW;
    }

    const double sum_ab = a2 + b2;
    const double sum_ac = a2 + c2;
    const double sum_bc = b2 + c2;
    if (!isfinite(sum_ab) || !isfinite(sum_ac) || !isfinite(sum_bc)) {
        return STATUS_OVERFLOW;
    }

    *result = is_close(sum_ab, c2, eps)
           || is_close(sum_ac, b2, eps)
           || is_close(sum_bc, a2, eps);
    return STATUS_OK;
}