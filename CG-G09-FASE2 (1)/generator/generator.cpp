#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <math.h>

#define _USE_MATH_DEFINES
#define M_PI 3.14159265358979323846

using namespace std;

void generatePlane(float size, int div, string nome);
void generateBox(float dim, int div, string nome);
void generateSphere(float radius, int slices, int stacks, string nome);
void generateCone(float radius, float height, int slices, int stacks, string nome);
string polarToCart(float radius, float alpha, float beta);
string polarToCartCone(float radius, float height, float alpha);

int main(int argc, char* argv[]){

	std::string obj = argv[1];
	if (obj == "plane") {
	
		if (argc != 5)
			cout << "Número de argumentos errado\n";
		else
			generatePlane(stof(argv[2]), stoi(argv[3]), argv[4]);
		// generator plane 1 3 plane.3d
		//  [0]       [1] [2][3] [4]	
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
	
	for(int i = 0; i < div ; ++i){
		for(int j = 0; j < div ; ++j){
			file << y+x*(j+1) << ", " << 0 << ", " << y+x*i << "\n";
			file << y+x*j << ", " << 0 << ", " << y+x*i << "\n";
			file << y+x*(j) << ", " << 0 << ", " << y+x*(i+1) << "\n";
			file << y+x*(j+1) << ", " << 0 << ", " << y+x*i << "\n";
			file << y+x*j << ", " << 0 << ", " << y+x*(i+1) << "\n";
			file << y+x*(j+1) << ", " << 0 << ", " << y+x*(i+1) << "\n";
		}
	}
	file.close();

	
}

void generateBox(float size, int div, string name) {

	string path = "../models/" + name;
	ofstream file;
	file.open(path);

	float comp = size / div;
	float pos = size / -2;

	// baixo 
	for (int i = 0; i < div; ++i) {
		for (int j = 0; j < div; ++j) {
			
			file << pos + comp * j << ", " << pos << ", " << pos + comp * i << "\n";
			file << pos + comp * (j + 1) << ", " << pos << ", " << pos + comp * i << "\n";
			file << pos + comp * (j) << ", " << pos << ", " << pos + comp * (i + 1) << "\n";

			file << pos + comp * j << ", " << pos << ", " << pos + comp * (i + 1) << "\n";
			file << pos + comp * (j + 1) << ", " << pos << ", " << pos + comp * i << "\n";
			file << pos + comp * (j + 1) << ", " << pos << ", " << pos + comp * (i + 1) << "\n";
		}
	}
	// cima
	for (int i = 0; i < div; ++i) {
		for (int j = 0; j < div; ++j) {
			
			file << pos + comp * (j + 1) << ", " << -pos << ", " << pos + comp * i << "\n";
			file << pos + comp * j << ", " << -pos << ", " << pos + comp * i << "\n";
			file << pos + comp * (j) << ", " << -pos << ", " << pos + comp * (i + 1) << "\n";

				file << pos + comp * (j + 1) << ", " << -pos << ", " << pos + comp * i << "\n";
			file << pos + comp * j << ", " << -pos << ", " << pos + comp * (i + 1) << "\n";
			file << pos + comp * (j + 1) << ", " << -pos << ", " << pos + comp * (i + 1) << "\n";
		}
	}

	// frente
	for (int i = 0; i < div; ++i) {
		for (int j = 0; j < div; ++j) {
			file << pos + comp * (j + 1) << ", " << pos + comp * i << ", " << -pos << "\n";
			file << pos + comp * j << ", " << pos + comp * (i+1) << ", " << -pos << "\n";
			file << pos + comp * j << ", " << pos + comp * i << ", " << -pos << "\n";

			file << pos + comp * (j + 1) << ", " << pos + comp * i << ", " << -pos << "\n";
			file << pos + comp * (j + 1) << ", " << pos + comp * (i + 1) << ", " << -pos << "\n";
			file << pos + comp * j << ", " << pos + comp * (i + 1) << ", " << -pos << "\n";
		}
	}

	// trás
	for (int i = 0; i < div; ++i) {
		for (int j = 0; j < div; ++j) {
			
			file << pos + comp * j << ", " << pos + comp * i << ", " << pos << "\n";
			file << pos + comp * j << ", " << pos + comp * (i+1) << ", " << pos << "\n";
			file << pos + comp * (j + 1) << ", " << pos + comp * i << ", " << pos << "\n";

			file << pos + comp * (j + 1) << ", " << pos + comp * i << ", " << pos << "\n";
			file << pos + comp * j << ", " << pos + comp * (i + 1) << ", " << pos << "\n";
			file << pos + comp * (j + 1) << ", " << pos + comp * (i + 1) << ", " << pos << "\n";
		}
	}
	
	// esquerda
	for (int i = 0; i < div; ++i) {
		for (int j = 0; j < div; ++j) {
			file << pos  << ", " << pos + comp * i << ", " << pos + comp * j << "\n";
			file << pos  << ", " << pos + comp * i << ", " << pos + comp * (j + 1) << "\n";
			file << pos  << ", " << pos + comp * (i+1) << ", " << pos + comp * (j + 1) << "\n";

			file << pos  << ", " << pos + comp * i << ", " << pos + comp * j << "\n";
			file << pos  << ", " << pos + comp * (i+1) << ", " << pos + comp * (j + 1) << "\n";
			file << pos  << ", " << pos + comp * (i+1) << ", " << pos + comp * j << "\n";
		}
	}

	// direita
	for (int i = 0; i < div; ++i) {
		for (int j = 0; j < div; ++j) {
			
			file << -pos  << ", " << pos + comp * i << ", " << pos + comp * j << "\n";
			file << -pos  << ", " << pos + comp * (i+1) << ", " << pos + comp * (j + 1) << "\n";
			file << -pos  << ", " << pos + comp * i << ", " << pos + comp * (j + 1) << "\n";

			file << -pos  << ", " << pos + comp * i << ", " << pos + comp * j << "\n";
			file << -pos  << ", " << pos + comp * (i+1) << ", " << pos + comp * j << "\n";
			file << -pos  << ", " << pos + comp * (i+1) << ", " << pos + comp * (j + 1) << "\n";
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

	for(int i = 0; i < stacks; ++i){
		for (int j = 0; j < slices; ++j)
		{
			file << polarToCart(radius,alpha,beta);
			file << polarToCart(radius,alpha,beta+stackSize);
			file << polarToCart(radius,alpha+sliceSize,beta);
			
			file << polarToCart(radius,alpha,beta+stackSize);
			file << polarToCart(radius,alpha+sliceSize,beta+stackSize);
			file << polarToCart(radius,alpha+sliceSize,beta);
			alpha+=sliceSize;
		}
		beta+=stackSize;
		alpha=0;
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

	for(int i=0;i<slices;i++){
	    alpha = i*sliceSize;
	    file << polarToCartCone(0,o,0);
        file << polarToCartCone(radius,o,alpha+sliceSize);
        file << polarToCartCone(radius,o,alpha);
	}
	for(int i = 0; i < stacks; i++){
        r=(((float)stacks-i)/stacks)*radius;
        rUp=(((float)stacks-(i+1))/stacks)*radius;
        for(int j = 0; j < slices; j++){
            alpha=j*sliceSize;
            file << polarToCartCone(rUp,o+stackSize*(i+1),alpha);
            file << polarToCartCone(r,o+stackSize*i,alpha);
            file << polarToCartCone(r,o+stackSize*i,alpha+sliceSize);
            file << polarToCartCone(rUp,o+stackSize*(i+1),alpha);
            file << polarToCartCone(r,o+stackSize*i,alpha+sliceSize);
            file << polarToCartCone(rUp,o+stackSize*(i+1),alpha+sliceSize);
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