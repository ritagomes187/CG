#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>


using namespace std;


void buildRotMatrix(float *x, float *y, float *z, float *m);
void cross(float *a, float *b, float *res);
void normalize(float *a);
float length(float *v);
void multMatrixVector(float *m, float* v, float* res);

#endif 