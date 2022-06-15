#ifndef CATMULL_H
#define CATMULL_H

#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include "structs.h"

using namespace std;


void buildRotMatrix(float *x, float *y, float *z, float *m);
void cross(float *a, float *b, float *res);
void normalize(float *a);
float length(float *v);
void multMatrixVector(float m[4][4], float* v, float* res);
void getCatmullRomPoint(float t, vertex p0, vertex p1, vertex p2, vertex p3, float* pos, float* deriv);
void getGlobalCatmullRomPoint(float gt, float *pos, float *deriv, vector<vertex> p);
vector<vector<vertex>> geraPontosCurva(vector<vertex> pontosC);


#endif 