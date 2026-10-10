#ifndef LOGIC_H
#define LOGIC_H

#include <stdbool.h>

#include "status.h"

#define MAX_PERMUTATIONS 6   /* 3! = 6 перестановок трёх чисел */

// Три коэффициента квадратного уравнения
typedef struct {
    double a;
    double b;
    double c;
} Coefficients;

//ответ у уравнения 
typedef enum {
    SOLUTION_TWO_ROOTS,  
    SOLUTION_ONE_ROOT,   
    SOLUTION_NO_ROOTS,  
    SOLUTION_LINEAR,       
    SOLUTION_ANY,          
    SOLUTION_NONE          
} SolutionKind;

Status make_permutations(const double a, const double b, const double c,
                         const double eps,
                         Coefficients *result, const int capacity, int *count);

Status solve_quadratic(const double a, const double b, const double c,
                       const double eps,
                       SolutionKind *kind, double *x1, double *x2);

Status is_multiple(const long long first, const long long second, bool *result);
Status is_right_triangle(const double a, const double b, const double c,
                         const double eps, bool *result);

#endif