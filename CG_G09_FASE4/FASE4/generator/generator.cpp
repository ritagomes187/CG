#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>
#include <sstream>
#include <algorithm>
#include "utils/matrix.h"

#define _USE_MATH_DEFINES
#define M_PI 3.14159265358979323846

using namespace std;

void generatePlane(float size, int div, string nome);
void generateBox(float dim, int div, string nome);
void generateSphere(float radius, int slices, int stacks, string nome);
void generateCone(float radius, float height, int slices, int stacks, string nome);
string polarToCart(float radius, float alpha, float beta);
string polarToCartCone(float radius, float height, float alpha);

class vertex {

public:
	float x;
	float y;
	float z;

public:
	vertex() {
		x = 0;
		y = 0;
		z = 0;
	}
	vertex(float a, float b, float c) {
		x = a;
		y = b;
		z = c;
	}


};



static float* genPolyBernstein(float p) {
	float* b = (float*)malloc(sizeof(float)*4);
	b[0] = powf(1 - p, 3);
	b[1] = 3 * p * powf(1 - p, 2);
	b[2] = 3 * powf(p, 2) * (1 - p);
	b[3] = powf(p, 3);
	return b;
}


void getNormalPoints(float u, float v, float** matrixX, float** matrixY, float** matrixZ, float* res){
	// matriz de bezier
	float bM[4][4] = { { -1.0f, 3.0f, -3.0f, 1.0f },
                               { 3.0f, -6.0f, 3.0f, 0.0f },
                               { -3.0f, 3.0f, 0.0f, 0.0f },
    
	                           { 1.0f,  0.0f, 0.0f, 0.0f } };
    
	float vVec[4] = { v * v * v, v * v, v, 1 };
	float uVec[4] = { u * u * u, u * u, u, 1 };

    float derivV[4] = { 3 * v * v, 2 * v, 1, 0 };
    float derivU[4] = { 3 * u * u, 2 * u, 1, 0 };


    //Calcular u
    float uRes[3];
    float uPx[4];
    float uPy[4];
    float uPz[4];
    float uAux[4];
    float mxAuxU[4];
    float myAuxU[4];
    float mzAuxU[4];


    multMatrixVector((float*)bM, vVec, uAux);
    multMatrixVector((float*)matrixX, uAux, uPx);
    multMatrixVector((float*)matrixY, uAux, uPy);
    multMatrixVector((float*)matrixZ, uAux, uPz);

    multMatrixVector((float*)bM, uPx, mxAuxU);
    multMatrixVector((float*)bM, uPy, myAuxU);
    multMatrixVector((float*)bM, uPz, mzAuxU);
    
    uRes[0] = (mxAuxU[0] * derivU[0]) + (mxAuxU[1] * derivU[1]) + (mxAuxU[2] * derivU[2]) + (mxAuxU[3] * derivU[3]);
    uRes[1] = (myAuxU[0] * derivU[0]) + (myAuxU[1] * derivU[1]) + (myAuxU[2] * derivU[2]) + (myAuxU[3] * derivU[3]);
    uRes[2] = (mzAuxU[0] * derivU[0]) + (mzAuxU[1] * derivU[1]) + (mzAuxU[2] * derivU[2]) + (mzAuxU[3] * derivU[3]);

    //Calcular v
    
	float px[4];
    float py[4];
    float pz[4];

    float mxAuxV[4];
    float myAuxV[4];
    float mzAuxV[4];
	float vAux[4];
    float vRes[3];

    multMatrixVector((float*)bM, derivV, vAux);
    multMatrixVector((float*)matrixX, vAux, px);
    multMatrixVector((float*)matrixY, vAux, py);
    multMatrixVector((float*)matrixZ, vAux, pz);

    multMatrixVector((float*)bM, px, mxAuxV);
    multMatrixVector((float*)bM, py, myAuxV);
    multMatrixVector((float*)bM, pz, mzAuxV);

    vRes[0] = (mxAuxV[0] * uVec[0]) + (mxAuxV[1] * uVec[1]) + (mxAuxV[2] * uVec[2]) + (mxAuxV[3] * uVec[3]);
    vRes[2] = (mzAuxV[0] * uVec[0]) + (mzAuxV[1] * uVec[1]) + (mzAuxV[2] * uVec[2]) + (mzAuxV[3] * uVec[3]);
    vRes[1] = (myAuxV[0] * uVec[0]) + (myAuxV[1] * uVec[1]) + (myAuxV[2] * uVec[2]) + (myAuxV[3] * uVec[3]);
    

	// u X v
    cross(uRes, vRes, res);
    normalize(res);


}

// p(u, v) = u * bM * PointsControl * bM * v 
vertex getPointBezier(float u, float v, float** pcX, float** pcY, float** pcZ) {
	float bezierMatrix[4][4] = { { -1.0f, 3.0f, -3.0f, 1.0f },
                               { 3.0f, -6.0f, 3.0f, 0.0f },
                               { -3.0f, 3.0f, 0.0f, 0.0f },
                               { 1.0f,  0.0f, 0.0f, 0.0f } };

    float vetorU[4] = { u * u * u, u * u, u, 1 };
    float vetorV[4] = { v * v * v, v * v, v, 1 };

    float vetorAux[4];
    float px[4];
    float py[4];
    float pz[4];

    float mx[4];
    float my[4];
    float mz[4];

    multMatrixVector((float*)bezierMatrix, vetorV, vetorAux);
    multMatrixVector((float*)pcX, vetorAux, px);
    multMatrixVector((float*)pcY, vetorAux, py);
    multMatrixVector((float*)pcZ, vetorAux, pz);

    multMatrixVector((float*)bezierMatrix, px, mx);
    multMatrixVector((float*)bezierMatrix, py, my);
    multMatrixVector((float*)bezierMatrix, pz, mz);

	vertex res;
    res.x = (vetorU[0] * mx[0]) + (vetorU[1] * mx[1]) + (vetorU[2] * mx[2]) + (vetorU[3] * mx[3]);
    res.y = (vetorU[0] * my[0]) + (vetorU[1] * my[1]) + (vetorU[2] * my[2]) + (vetorU[3] * my[3]);
    res.z = (vetorU[0] * mz[0]) + (vetorU[1] * mz[1]) + (vetorU[2] * mz[2]) + (vetorU[3] * mz[3]);

	return res;
}

void generateBezier(string fileRead, int tess, string fileWrite) {
	string path = "./patch/" + fileRead;
	
	ifstream f(path);

	int patches = 0;
	int verts = 0;

	
	vector<vector<int>> indexPatch;
	vector<vertex> pontosControl;


	// Ler patch
	if(f.is_open()){
		string linha;
		if (getline(f, linha)) sscanf(linha.c_str(), "%d\n", &patches);
		for (int i = 0; i < patches; i++) {
			vector<int> auxIndex;
			if (getline(f, linha)) {
				int patch;
				std::replace(linha.begin(), linha.end(), ',', ' ');
				std::stringstream number(linha);
				while (number >> patch) {
					auxIndex.push_back(patch);
				}
			}
			indexPatch.push_back(auxIndex);
		}
		if (getline(f, linha)) sscanf(linha.c_str(), "%d\n", &verts);
		for (size_t i = 0; i < verts; i++)
		{
			vertex v;
			if (getline(f, linha)) sscanf(linha.c_str(), "%f, %f, %f\n", &v.x, &v.y, &v.z);
			pontosControl.push_back(v);
		}
	}
	else{
		cout << "Ficheiro nao encontrado" << endl;
		exit(0);
	}
	

	
	// Desenhar patch
	path = "../models/" + fileWrite;
	ofstream fout;
	fout.open(path);

	float pointsX[4][4];
    float pointsY[4][4];
    float pointsZ[4][4];
	float pos[4][3];
	float nivel = 1.0 / tess;
	int indice;
	float u , v;
	indice = 0;
	
	for (;indice < indexPatch.size(); indice++)
	{
		u = 0;
		while(u < 1) {
			v = 0;
			while(v < 1)
			{
				/* obter pontos de bezier */
				
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 4; j++) {
						int index = indexPatch[indice][i * 4 + j];
						// control point
						pointsX[i][j] = pontosControl[index].x;
						pointsY[i][j] = pontosControl[index].y;
						pointsZ[i][j] = pontosControl[index].z;
					}
				}


				// p(u,v) 
				vertex point1 = getPointBezier(u, v, (float**)pointsX, (float**)pointsY, (float**)pointsZ);
				
				// p(u+niv,v)
				vertex point2 = getPointBezier(u + nivel, v, (float**)pointsX, (float**)pointsY, (float**)pointsZ);
				
				// p(u, v+niv)
				vertex point3 = getPointBezier(u, v + nivel, (float**)pointsX, (float**)pointsY, (float**)pointsZ);
				
				// p(u+niv,v+niv)
				vertex point4 = getPointBezier(u + nivel, v + nivel, (float**)pointsX, (float**)pointsY, (float**)pointsZ);

				
				
				//pontos normais
				getNormalPoints(u, v, (float**)pointsX, (float**)pointsY, (float**)pointsZ, pos[0]);
				getNormalPoints(u + nivel, v, (float**)pointsX, (float**)pointsY, (float**)pointsZ, pos[1]);
				getNormalPoints(u, v + nivel, (float**)pointsX, (float**)pointsY, (float**)pointsZ, pos[2]);
				getNormalPoints(u + nivel, v + nivel, (float**)pointsX, (float**)pointsY, (float**)pointsZ, pos[3]);

				/* escrita dos pontos no ficheiro .3d */ 
				//pontos
				fout << point1.x << ", " << point1.y << ", " << point1.z << "\n";
				fout << point3.x << ", " << point3.y << ", " << point3.z << "\n";
				fout << point4.x << ", " << point4.y << ", " << point4.z << "\n";
				
				//normais
				fout << -pos[0][0] << ", " << -pos[0][1] << ", " << -pos[0][2] << "\n";
                fout << -pos[2][0] << ", " << -pos[2][1] << ", " << -pos[2][2] << "\n";
                fout << -pos[3][0] << ", " << -pos[3][1] << ", " << -pos[3][2] << "\n";
                
				//textura
				fout << u << ", " << v << ", 0\n" ;
				fout << u+nivel << ", " << v << ", 0\n";
				fout << u+nivel << ", " << v+nivel << ", 0\n";

				//pontos
				fout << point1.x << ", " << point1.y << ", " << point1.z << "\n";
				fout << point4.x << ", " << point4.y << ", " << point4.z << "\n";
				fout << point2.x << ", " << point2.y << ", " << point2.z << "\n";
				//normais
				fout << -pos[0][0] << ", " << -pos[0][1] << ", " << -pos[0][2] << "\n";
                fout << -pos[3][0] << ", " << -pos[3][1] << ", " << -pos[3][2] << "\n";
				fout << -pos[1][0] << ", " << -pos[1][1] << ", " << -pos[1][2] << "\n";

				fout << u+nivel << ", " << v+nivel << ", 0\n";
				fout << u << ", " << v+nivel << ", 0\n";
				fout << u << ", " << v << ", 0\n";

				v+=nivel;
				
			}
			u+=nivel;
			
		}

	}
	fout.close();
}




int main(int argc, char* argv[]){

	std::string obj = argv[1];
	if (obj == "plane") {
	
		if (argc != 5)
			cout << "Número de argumentos errado\n";
		else
			generatePlane(stof(argv[2]), stoi(argv[3]), argv[4]);	
	}
	else if (obj == "box") {
		if (argc != 5)
			cout << "Número de argumentos errado\n";
		else
			generateBox(stof(argv[2]), stoi(argv[3]), argv[4]);
	}
	
	else if (obj == "sphere") {
		if (argc != 6)
			cout << "Número de argumentos errado\n";
		else
			generateSphere(stof(argv[2]), stoi(argv[3]), stoi(argv[4]), argv[5]);
	}
	
	
	else if (obj == "cone") {
		if (argc != 7)
			cout << "Número de argumentos errado\n";
		else
			generateCone(stof(argv[2]), stof(argv[3]), stoi(argv[4]), stoi(argv[5]), argv[6]);
	}
	else if (obj == "bezier") {
		if (argc != 5)
			cout << "Número de argumentos errado\n";
		else
			generateBezier(argv[2],stoi(argv[3]), argv[4]);
	}

	else {
			cout << "Comando não foi reconhecido\n";
	}

	return 0;
}

void generatePlane(float size, int div, string nome){

	string path = "../models/" + nome;
	ofstream file;
	file.open(path);

	float x = size/div;
	float y = size/-2;
	float textsize = 1.0/div;

	for(int i = 0; i < div ; ++i){
		for(int j = 0; j < div ; ++j){
			/* triangulo 1 */
			// verts
			file << y+x*(j+1) << ", " << 0 << ", " << y+x*i << "\n";
			file << y+x*j << ", " << 0 << ", " << y+x*i << "\n";
			file << y+x*(j) << ", " << 0 << ", " << y+x*(i+1) << "\n";
			
			// normais
			file << 0 << ", " << 1 << ", " << 0 << "\n";
			file << 0 << ", " << 1 << ", " << 0 << "\n";
			file << 0 << ", " << 1 << ", " << 0 << "\n";
			
			// texture
			file << textsize*(j+1) << ", " << textsize*(i+1) << ", " << 0 << "\n";
			file << textsize*j << ", " << textsize*(i+1) << ", " << 0 << "\n";
			file << textsize*j << ", " << textsize*i << ", " << 0 << "\n";
			 
			
			/* triangulo 2 */
			// verts
			file << y+x*(j+1) << ", " << 0 << ", " << y+x*i << "\n";
			file << y+x*j << ", " << 0 << ", " << y+x*(i+1) << "\n";
			file << y+x*(j+1) << ", " << 0 << ", " << y+x*(i+1) << "\n";
			// normais
			file << 0 << ", " << 1 << ", " << 0 << "\n";
			file << 0 << ", " << 1 << ", " << 0 << "\n";
			file << 0 << ", " << 1 << ", " << 0 << "\n";
			// texture
			file << textsize*(j+1) << ", " << textsize*(i+1) << ", " << 0 << "\n";
			file << textsize*j << ", " << textsize*i << ", " << 0 << "\n";
			file << textsize*(j+1) << ", " << textsize*i << ", " << 0 << "\n";			
		}
	}
	file.close();

	
}

void generateBox(float size, int div, string name) {

	string path = "../models/" + name;
	ofstream file;
	file.open(path);
	float coltex = (float)(1.0/2.0);
	float lintex = (float)(1.0/3.0);
	float divcoltex = coltex/div;
	float divlintex = lintex/div;

	float xtexAtual = 0.0f;
	float ytexAtual = 0.0f;

	float comp = size / div;
	float pos = size / -2;

	// baixo 
	for (int i = 0; i < div; ++i) {
		for (int j = 0; j < div; ++j) {
			
			file << pos + comp * j << ", " << pos << ", " << pos + comp * i << "\n";
			file << pos + comp * (j + 1) << ", " << pos << ", " << pos + comp * i << "\n";
			file << pos + comp * (j) << ", " << pos << ", " << pos + comp * (i + 1) << "\n";

			// normais
			file << 0 << ", " << -1 << ", " << 0 << "\n";
			file << 0 << ", " << -1 << ", " << 0 << "\n";
			file << 0 << ", " << -1 << ", " << 0 << "\n";


			// tex
			file << xtexAtual+ divcoltex*j << ", " << ytexAtual+ divlintex*i  << ", " << 0 << "\n";
			file << xtexAtual + divcoltex*j << ", " << ytexAtual+ divlintex*(i+1) << ", " << 0 << "\n";
			file << xtexAtual + divcoltex*(j+1) << ", " << ytexAtual+ divlintex*i << ", " << 0 << "\n";			


			file << pos + comp * j << ", " << pos << ", " << pos + comp * (i + 1) << "\n";
			file << pos + comp * (j + 1) << ", " << pos << ", " << pos + comp * i << "\n";
			file << pos + comp * (j + 1) << ", " << pos << ", " << pos + comp * (i + 1) << "\n";

			// normais
			file << 0 << ", " << -1 << ", " << 0 << "\n";
			file << 0 << ", " << -1 << ", " << 0 << "\n";
			file << 0 << ", " << -1 << ", " << 0 << "\n";


			// tex
			file << xtexAtual + divcoltex*j << ", " << ytexAtual+ divlintex*(i+1) << ", " << 0 << "\n";
			file << xtexAtual+ divcoltex*(j+1) << ", " << ytexAtual+ divlintex*(i+1)  << ", " << 0 << "\n";
			file << xtexAtual + divcoltex*(j+1) << ", " << ytexAtual+ divlintex*i << ", " << 0 << "\n";


		}
	}
	ytexAtual += lintex;
	// cima
	for (int i = 0; i < div; ++i) {
		for (int j = 0; j < div; ++j) {
			
			file << pos + comp * (j + 1) << ", " << -pos << ", " << pos + comp * i << "\n";
			file << pos + comp * j << ", " << -pos << ", " << pos + comp * i << "\n";
			file << pos + comp * (j) << ", " << -pos << ", " << pos + comp * (i + 1) << "\n";

			// normais
			file << 0 << ", " << 1 << ", " << 0 << "\n";
			file << 0 << ", " << 1 << ", " << 0 << "\n";
			file << 0 << ", " << 1 << ", " << 0 << "\n";


			// tex
			file << xtexAtual+ divcoltex*j << ", " << ytexAtual+ divlintex*i  << ", " << 0 << "\n";
			file << xtexAtual + divcoltex*(j+1) << ", " << ytexAtual+ divlintex*i << ", " << 0 << "\n";
			file << xtexAtual + divcoltex*j << ", " << ytexAtual+ divlintex*(i+1) << ", " << 0 << "\n";


			file << pos + comp * (j + 1) << ", " << -pos << ", " << pos + comp * i << "\n";
			file << pos + comp * j << ", " << -pos << ", " << pos + comp * (i + 1) << "\n";
			file << pos + comp * (j + 1) << ", " << -pos << ", " << pos + comp * (i + 1) << "\n";
		
			// normais
			file << 0 << ", " << 1 << ", " << 0 << "\n";
			file << 0 << ", " << 1 << ", " << 0 << "\n";
			file << 0 << ", " << 1 << ", " << 0 << "\n";

			file << xtexAtual + divcoltex*(j+1) << ", " << ytexAtual+ divlintex*i << ", " << 0 << "\n";
			file << xtexAtual + divcoltex*(j+1) << ", " << ytexAtual+ divlintex*(i+1) << ", " << 0 << "\n";
			file << xtexAtual+ divcoltex*j << ", " << ytexAtual+ divlintex*(i+1) << ", " << 0 << "\n";

			
		}

	}
	ytexAtual += lintex;
	// frente
	for (int i = 0; i < div; ++i) {
		for (int j = 0; j < div; ++j) {
			file << pos + comp * (j + 1) << ", " << pos + comp * i << ", " << -pos << "\n";
			file << pos + comp * j << ", " << pos + comp * (i+1) << ", " << -pos << "\n";
			file << pos + comp * j << ", " << pos + comp * i << ", " << -pos << "\n";

			// normais
			file << 0 << ", " << 0 << ", " << 1 << "\n";
			file << 0 << ", " << 0 << ", " << 1 << "\n";
			file << 0 << ", " << 0 << ", " << 1 << "\n";

			// tex
			file << xtexAtual + divcoltex*(j+1) << ", " << ytexAtual+ divlintex*i << ", " << 0 << "\n";
			file << xtexAtual + divcoltex*j << ", " << ytexAtual+ divlintex*(i+1) << ", " << 0 << "\n";
			file << xtexAtual+ divcoltex*j << ", " << ytexAtual+ divlintex*i  << ", " << 0 << "\n";

			file << pos + comp * (j + 1) << ", " << pos + comp * i << ", " << -pos << "\n";
			file << pos + comp * (j + 1) << ", " << pos + comp * (i + 1) << ", " << -pos << "\n";
			file << pos + comp * j << ", " << pos + comp * (i + 1) << ", " << -pos << "\n";

			// normais
			file << 0 << ", " << 0 << ", " << 1 << "\n";
			file << 0 << ", " << 0 << ", " << 1 << "\n";
			file << 0 << ", " << 0 << ", " << 1 << "\n";

			// tex
			file << xtexAtual + divcoltex*(j+1) << ", " << ytexAtual+ divlintex*i << ", " << 0 << "\n";
			file << xtexAtual+ divcoltex*(j+1) << ", " << ytexAtual+ divlintex*(i+1)  << ", " << 0 << "\n";
			file << xtexAtual + divcoltex*j << ", " << ytexAtual+ divlintex*(i+1) << ", " << 0 << "\n";

		}
	}
	xtexAtual += coltex;
	// trás
	for (int i = 0; i < div; ++i) {
		for (int j = 0; j < div; ++j) {
			
			file << pos + comp * j << ", " << pos + comp * i << ", " << pos << "\n";
			file << pos + comp * j << ", " << pos + comp * (i+1) << ", " << pos << "\n";
			file << pos + comp * (j + 1) << ", " << pos + comp * i << ", " << pos << "\n";

			// normais
			file << 0 << ", " << 0 << ", " << -1 << "\n";
			file << 0 << ", " << 0 << ", " << -1 << "\n";
			file << 0 << ", " << 0 << ", " << -1 << "\n";

			// tex
			file << xtexAtual + divcoltex*j << ", " << ytexAtual+ divlintex*(i+1) << ", " << 0 << "\n";
			file << xtexAtual + divcoltex*(j+1) << ", " << ytexAtual+ divlintex*i << ", " << 0 << "\n";
			file << xtexAtual+ divcoltex*j << ", " << ytexAtual+ divlintex*i  << ", " << 0 << "\n";

			file << pos + comp * (j + 1) << ", " << pos + comp * i << ", " << pos << "\n";
			file << pos + comp * j << ", " << pos + comp * (i + 1) << ", " << pos << "\n";
			file << pos + comp * (j + 1) << ", " << pos + comp * (i + 1) << ", " << pos << "\n";
		
			// normais
			file << 0 << ", " << 0 << ", " << -1 << "\n";
			file << 0 << ", " << 0 << ", " << -1 << "\n";
			file << 0 << ", " << 0 << ", " << -1 << "\n";

			// tex
			file << xtexAtual + divcoltex*j << ", " << ytexAtual+ divlintex*(i+1) << ", " << 0 << "\n";
			file << xtexAtual + divcoltex*(j+1) << ", " << ytexAtual+ divlintex*(i+1) << ", " << 0 << "\n";
			file << xtexAtual+ divcoltex*(j+1) << ", " << ytexAtual+ divlintex*i  << ", " << 0 << "\n";
		}
	}
	ytexAtual -= lintex;
	// esquerda
	for (int i = 0; i < div; ++i) {
		for (int j = 0; j < div; ++j) {
			file << pos  << ", " << pos + comp * i << ", " << pos + comp * j << "\n";
			file << pos  << ", " << pos + comp * i << ", " << pos + comp * (j + 1) << "\n";
			file << pos  << ", " << pos + comp * (i+1) << ", " << pos + comp * (j + 1) << "\n";

			// normais
			file << -1 << ", " << 0 << ", " << 0 << "\n";
			file << -1 << ", " << 0 << ", " << 0 << "\n";
			file << -1 << ", " << 0 << ", " << 0 << "\n";

			// tex
			file << xtexAtual+ divcoltex*j << ", " << ytexAtual+ divlintex*i  << ", " << 0 << "\n";
			file << xtexAtual + divcoltex*j << ", " << ytexAtual+ divlintex*(i+1) << ", " << 0 << "\n";
			file << xtexAtual + divcoltex*(j+1) << ", " << ytexAtual+ divlintex*i << ", " << 0 << "\n";

			file << pos  << ", " << pos + comp * i << ", " << pos + comp * j << "\n";
			file << pos  << ", " << pos + comp * (i+1) << ", " << pos + comp * (j + 1) << "\n";
			file << pos  << ", " << pos + comp * (i+1) << ", " << pos + comp * j << "\n";
		
			// normais
			file << -1 << ", " << 0 << ", " << 0 << "\n";
			file << -1 << ", " << 0 << ", " << 0 << "\n";
			file << -1 << ", " << 0 << ", " << 0 << "\n";

			// tex
			file << xtexAtual + divcoltex*(j+1) << ", " << ytexAtual+ divlintex*i << ", " << 0 << "\n";
			file << xtexAtual + divcoltex*j << ", " << ytexAtual+ divlintex*(i+1) << ", " << 0 << "\n";
			file << xtexAtual+ divcoltex*(j+1) << ", " << ytexAtual+ divlintex*(i+1)  << ", " << 0 << "\n";

		}
	}
	ytexAtual -= lintex;
	// direita
	for (int i = 0; i < div; ++i) {
		for (int j = 0; j < div; ++j) {
			
			file << -pos  << ", " << pos + comp * i << ", " << pos + comp * j << "\n";
			file << -pos  << ", " << pos + comp * (i+1) << ", " << pos + comp * (j + 1) << "\n";
			file << -pos  << ", " << pos + comp * i << ", " << pos + comp * (j + 1) << "\n";

			// normais
			file << 1 << ", " << 0 << ", " << 0 << "\n";
			file << 1 << ", " << 0 << ", " << 0 << "\n";
			file << 1 << ", " << 0 << ", " << 0 << "\n";

			// tex
			file << xtexAtual + divcoltex*(j+1) << ", " << ytexAtual+ divlintex*i << ", " << 0 << "\n";
			file << xtexAtual + divcoltex*j << ", " << ytexAtual+ divlintex*(i+1) << ", " << 0 << "\n";
			file << xtexAtual+ divcoltex*j << ", " << ytexAtual+ divlintex*i  << ", " << 0 << "\n";


			file << -pos  << ", " << pos + comp * i << ", " << pos + comp * j << "\n";
			file << -pos  << ", " << pos + comp * (i+1) << ", " << pos + comp * j << "\n";
			file << -pos  << ", " << pos + comp * (i+1) << ", " << pos + comp * (j + 1) << "\n";
		
			// normais
			file << 1 << ", " << 0 << ", " << 0 << "\n";
			file << 1 << ", " << 0 << ", " << 0 << "\n";
			file << 1 << ", " << 0 << ", " << 0 << "\n";

			// tex
			file << xtexAtual + divcoltex*(j+1) << ", " << ytexAtual+ divlintex*i << ", " << 0 << "\n";
			file << xtexAtual+ divcoltex*(j+1) << ", " << ytexAtual+ divlintex*(i+1)  << ", " << 0 << "\n";
			file << xtexAtual + divcoltex*j << ", " << ytexAtual+ divlintex*(i+1) << ", " << 0 << "\n";

		}
	}

	file.close();

}



void generateSphere(float radius, int slices, int stacks, string nome){

	string path = "../models/" + nome;
	ofstream file;
	file.open(path);
	
	float alpha = 0.0f; 
	float beta =  M_PI/-2; 
	float sliceSize = (2*M_PI)/ slices; 
	float stackSize = M_PI/stacks; 
	float texlin = 1.0 / (float)stacks;
	float texcol = 1.0 / (float)slices;
	float u = 0.0f;
	float v = 0.0f;

	for(int i = 0; i < stacks; ++i){
		for (int j = 0; j < slices; ++j)
		{
			file << polarToCart(radius,alpha,beta);
			file << polarToCart(radius,alpha,beta+stackSize);
			file << polarToCart(radius,alpha+sliceSize,beta);
			
			//normais
			
			file << polarToCart(-1,alpha,beta);
			file << polarToCart(-1,alpha,beta+stackSize);
			file << polarToCart(-1,alpha+sliceSize,beta);

			// tex
			file << u << ", " << v  << ", " << 0 << "\n";
			file << u << ", " << v+texlin << ", " << 0 << "\n";
			file << u+texcol << ", " << v  << ", " << 0 << "\n";


			file << polarToCart(radius,alpha,beta+stackSize);
			file << polarToCart(radius,alpha+sliceSize,beta+stackSize);
			file << polarToCart(radius,alpha+sliceSize,beta);


			//normais
			
			file << polarToCart(-1,alpha,beta+stackSize);
			file << polarToCart(-1,alpha+sliceSize,beta+stackSize);
			file << polarToCart(-1,alpha+sliceSize,beta);

			
			// tex
		
			file << u << ", " << v+texlin  << ", " << 0 << "\n";
			file << u+texcol << ", " << v+texlin  << ", " << 0 << "\n";
			file << u+texcol << ", " << v << ", " << 0 << "\n";


			alpha+=sliceSize;
			u+=texcol;
		}
		beta+=stackSize;
		v+=texlin;
		alpha=0;
		u = 0.0f;
	}
	
	file.close();
}



void generateCone(float radius, float height, int slices, int stacks, string nome){

	string path = "../models/" + nome;
	ofstream file;
	file.open(path);

	float stackSize = height/stacks;
	float sliceSize = 2*M_PI/slices;
	float alpha = 0.0f;
	float r = 0.0f;
	float rUp = 0.0f;
	float o = 0.0f;
	float texcol = 1.0f / slices;
	float texlin = 1.0f / stacks;

	// base
	for(int i=0;i<slices;i++){
	    alpha = i*sliceSize;
	    file << polarToCartCone(0,o,0);
        file << polarToCartCone(radius,o,alpha+sliceSize);
        file << polarToCartCone(radius,o,alpha);
		
		// normais
		file << 0 << ", " << -1 << ", " << 0 << "\n";
		file << 0 << ", " << -1 << ", " << 0 << "\n";
		file << 0 << ", " << -1 << ", " << 0 << "\n";

		// tex
		file << 0 << ", " << 0  << ", " << 0 << "\n";
		file << 0 << ", " << 0  << ", " << 0 << "\n";
		file << 0 << ", " << 0  << ", " << 0 << "\n";
	
	
	}

	float yNormal = cos(atan(height/radius));		
	for(int i = 0; i < stacks; i++){
        r=(((float)stacks-i)/stacks)*radius;
        rUp=(((float)stacks-(i+1))/stacks)*radius;
        for(int j = 0; j < slices; j++){
            alpha=j*sliceSize;
            file << polarToCartCone(rUp,o+stackSize*(i+1),alpha);
            file << polarToCartCone(r,o+stackSize*i,alpha);
            file << polarToCartCone(r,o+stackSize*i,alpha+sliceSize);
            
			//normais
			
			file << polarToCartCone(1,yNormal,alpha);
			file << polarToCartCone(1,yNormal,alpha);
			file << polarToCartCone(1,yNormal,alpha+sliceSize);

			// tex
			file << texcol*(j) << ", " << texlin*(i+1)  << ", " << 0 << "\n";
			file << texcol*(j) << ", " << texlin*(i)  << ", " << 0 << "\n";
			file << texcol*(j+1) << ", " << texlin*(i)  << ", " << 0 << "\n";
			
			file << polarToCartCone(rUp,o+stackSize*(i+1),alpha);
            file << polarToCartCone(r,o+stackSize*i,alpha+sliceSize);
            file << polarToCartCone(rUp,o+stackSize*(i+1),alpha+sliceSize);
		
		
			//normais
			file << polarToCartCone(1,yNormal,alpha);
			file << polarToCartCone(1,yNormal,alpha+sliceSize);
			file << polarToCartCone(1,yNormal,alpha+sliceSize);
			
			//tex
			file << texcol*(j) << ", " << texlin*(i+1)  << ", " << 0 << "\n";
			file << texcol*(j+1) << ", " << texlin*(i)  << ", " << 0 << "\n";
			file << texcol*(j+1) << ", " << texlin*(i+1)  << ", " << 0 << "\n";

		}

	}
}

string polarToCart(float radius, float alpha, float beta){
	float x = radius*cos(beta)*sin(alpha);
	float y = radius*sin(beta);
	float z = radius*cos(beta)*cos(alpha);
	return to_string(x) + ", " + to_string(y) + ", " + to_string(z) + "\n";
}


string polarToCartCone(float radius,float height, float alpha){
	float x = radius*sin(alpha);
	float y = height;
	float z = radius*cos(alpha);
	return to_string(x) + ", " + to_string(y) + ", " + to_string(z)+"\n";
}

