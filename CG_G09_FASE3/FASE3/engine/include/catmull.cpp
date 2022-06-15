#include "catmull.h"

#define TESSELATION 100.0
// Points that make up the loop for catmull-rom interpolation




void buildRotMatrix(float *x, float *y, float *z, float *m) {

	m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
	m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
	m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}


void cross(float *a, float *b, float *res) {

	res[0] = a[1]*b[2] - a[2]*b[1];
	res[1] = a[2]*b[0] - a[0]*b[2];
	res[2] = a[0]*b[1] - a[1]*b[0];
}


void normalize(float *a) {

	float l = sqrt(a[0]*a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] = a[0]/l;
	a[1] = a[1]/l;
	a[2] = a[2]/l;
}


float length(float *v) {

	float res = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	return res;

}

void multMatrixVector(float m[4][4], float* v, float* res) {

	for (int j = 0; j < 4; ++j) {
		res[j] = 0;
		for (int k = 0; k < 4; ++k) {
			res[j] += v[k] * m[j][k];
		}
	}

}






void getCatmullRomPoint(float t, vertex p0, vertex p1, vertex p2, vertex p3, float* pos, float* deriv) {
	// catmull-rom matrix
	float m[4][4] = { {-0.5f,  1.5f, -1.5f,  0.5f},
						{ 1.0f, -2.5f,  2.0f, -0.5f},
						{-0.5f,  0.0f,  0.5f,  0.0f},
						{ 0.0f,  1.0f,  0.0f,  0.0f} };


	// x
	float px[4] = { p0.x, p1.x, p2.x, p3.x };
	float ax[4];
	multMatrixVector(m, px, ax);
	pos[0] = powf(t, 3.0) * ax[0] + powf(t, 2.0) * ax[1] + t * ax[2] + ax[3];
	deriv[0] = 3 * powf(t, 2.0) * ax[0] + 2 * t * ax[1] + ax[2];

	// y
	float py[4] = { p0.y, p1.y, p2.y, p3.y };
	float ay[4];
	multMatrixVector(m, py, ay);
	pos[1] = powf(t, 3.0) * ay[0] + powf(t, 2.0) * ay[1] + t * ay[2] + ay[3];
	deriv[1] = 3 * powf(t, 2.0) * ay[0] + 2 * t * ay[1] + ay[2];

	
	// z
	float pz[4] = { p0.z, p1.z, p2.z, p3.z };
	float az[4];
	multMatrixVector(m, pz, az);
	pos[2] = powf(t, 3.0) * az[0] + powf(t, 2.0) * az[1] + t * az[2] + az[3];
	deriv[2] = 3 * powf(t, 2.0) * az[0] + 2 * t * az[1] + az[2];
	
}


// given  global t, returns the point in the curve
void getGlobalCatmullRomPoint(float gt, float *pos, float *deriv, vector<vertex>p) {
	
	int numPoints = p.size();
	float t = gt * numPoints; // this is the real global t
	int index = floor(t);  // which segment
	t = t - index; // where within  the segment

	// indices store the points
	int indices[4]; 
	indices[0] = (index + numPoints-1)%numPoints;	
	indices[1] = (indices[0]+1)%numPoints;
	indices[2] = (indices[1]+1)%numPoints; 
	indices[3] = (indices[2]+1)%numPoints;

	getCatmullRomPoint(t, p[indices[0]], p[indices[1]], p[indices[2]], p[indices[3]], pos, deriv);
	
}

vector<vector<vertex>> geraPontosCurva(vector<vertex> pontosC) {
	float pos[3]; // vector das coordenadas 
	float deriv[3]; // vector das tangentes das coordenadas
	float z;
	vector<vector<vertex>> vPos;
	vector<vertex> posVec;
	vector<vertex> devVec;
	// 0.01 = 100 pontos
	for (z = 0; z < 1; z += 0.01) {
		getGlobalCatmullRomPoint(z, pos, deriv, pontosC);

		vertex vp = vertex(pos[0], pos[1], pos[2]);
		vertex vd = vertex(deriv[0], deriv[1], deriv[2]);
		posVec.push_back(vp);
		devVec.push_back(vd);
	}
	vPos.push_back(posVec);
	vPos.push_back(devVec);


	return vPos;
}