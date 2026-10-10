#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "status.h"

#define MIN_EPS 1e-12
Status check_eps(const double eps);

Status e_limit(const double eps, double *result);
Status e_series(const double eps, double *result);
Status e_equation(const double eps, double *result);

Status pi_limit(const double eps, double *result);
Status pi_series(const double eps, double *result);
Status pi_equation(const double eps, double *result);

Status ln2_limit(const double eps, double *result);
Status ln2_series(const double eps, double *result);
Status ln2_equation(const double eps, double *result);

Status sqrt2_limit(const double eps, double *result);
Status sqrt2_product(const double eps, double *result);
Status sqrt2_equation(const double eps, double *result);
// gamma (постоянная Эйлера) 
Status gamma_limit(const double eps, double *result);
Status gamma_series(const double eps, double *result);
Status gamma_equation(const double eps, double *result);

#endif