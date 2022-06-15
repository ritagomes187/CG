#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { cout << "Error: " << a_eResult << endl;  }
#endif





#include <fstream>
#include <direct.h>
#define GetCurrentDir _getcwd
#include<iostream>
using namespace std;


#ifdef __APPLE__
#include <GLUT/glut.h

#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include "include\tinyxml2.h"
#include "include\catmull.h"
#include "include\structs.h"
#include <math.h>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <fstream>
#include <string>
#include <iostream>





#define M_PI 3.14159265358979323846
#define TESSELATION 100.0

float prev_y[3] = { 0,1,0 };





class operations {
    public:
        virtual void run() = 0;
        virtual void print() = 0;
};

class opTranslate : public operations
{
public:
    vector<vertex> points;
    vector<vertex> deriv;
    vector<vertex> pointsCatmull;
    bool align;
    float time;
    


    
    opTranslate() {
        time = 0.0f;
        align = true;
        //z = 0.0f;
    }
    
    

    void run() {
        
        if (time == 0.0f) {
            vertex v = points[0];
            glTranslatef(v.x, v.y, v.z);
        }
        else {
            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < pointsCatmull.size(); i++) {
                glColor3d(0.1, 0.1, 0.1);
                glVertex3f(pointsCatmull[i].x, pointsCatmull[i].y, pointsCatmull[i].z);
            }
            glEnd();

            
            float pAtual[3];
            float dAtual[3];
            float tAux = glutGet(GLUT_ELAPSED_TIME) % ((int)time * 1000);
            float t = tAux / (time * 1000);
            

            getGlobalCatmullRomPoint(t, pAtual, dAtual, points);
            glTranslatef(pAtual[0], pAtual[1], pAtual[2]);

            float x[3] = { dAtual[0], dAtual[1], dAtual[2]};
            normalize(x);
            float z[3];
            cross(x, prev_y, z);
            normalize(z);
            float y[3];
            cross(z, x, y);
            normalize(y);
            memcpy(prev_y, y, 3 * sizeof(float));

            float m[16];

            buildRotMatrix(x, y, z, m);

            glMultMatrixf(m);
            
        }
        
    }
    
    void print() {
    }
};

class opRotate : public operations
{
public:
    float angle = 0;
    float time = 0;
    float x = 0;
    float y = 0;
    float z = 0;

    void run() {
        
        if (angle != 0) glRotatef(angle, x, y, z);
        else {
            float angle = (((float)glutGet(GLUT_ELAPSED_TIME) / 1000) * 360) / time;
            glRotatef(angle, x, y, z);
        }
        

    }
    void print() {
        cout << "(Rotate) tenho as coordenadas: " << x << " " << y << " " << z << endl;

    }
};

class opScale : public operations
{
public:
    float x = 0;
    float y = 0;
    float z = 0;

    void run() {
        glScalef(x, y, z);
    }
    void print() {
        cout << "Scale recebi: " << x << " " << y << " " << z << endl;

    }
};

class opDraw : public operations
{
public:
    GLuint buffer;
    GLuint vertexCount = 0;

    float r = 0;
    float g = 0;
    float b = 0;

public:    
    void prep_vbo(vector<vertex> verts) {
        vertexCount = verts.size();
        
        
        glGenBuffers(1, &buffer);
        
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float) * 3, verts.data(), GL_STATIC_DRAW);
}


    void run() {
        glColor3ub(r, g, b);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }
    void print(){}
};



struct transformation {
    bool haveChild = false;
    vector<operations*> ops;
    vector<transformation*> childs;
};




using namespace tinyxml2;

using namespace std;




transformation *transf = NULL;
camPos cp;
vector<vertex> allPoints;
camera pointsCam;

float px , py , pz ; // coordenadas da pos camara
float tx, ty, tz = 0.0; // coordenadas para translação

float angle = 0.0f;
float scale =1.0f; 
int draw = GL_FILL;







void calcRadius() {
    cp.raio = sqrt( pow(pointsCam.pos.x,2) + pow(pointsCam.pos.z,2) + pow(pointsCam.pos.y, 2));
}

void calcBeta() {
    cp.beta = atan(pointsCam.pos.y / cp.raio);
}
void calcAlf() {
    cp.alpha =  atan(pointsCam.pos.x/pointsCam.pos.z);
}

float polarX() {
    return cp.raio * cos(cp.beta) * sin(cp.alpha);
}

float polarY() {
    return cp.raio * sin(cp.beta);
}

float polarZ() {
    return cp.raio * cos(cp.beta) * cos(cp.alpha);
}


void readFile(string fich) {

    string linha; // linha que contem 1 vertice
    string novo; // 1 posição
    string delimiter = ","; // x , y , z
    int pos; // Posição antes do delimitador no ficheiro
    float xx, yy, zz; // Coordenadas
    vertex v;

    //fich = "../models/" + fich; // ficheiro .3d

    ifstream file(fich);

    if (file.is_open()) {

        while (getline(file, linha)) {

            pos = linha.find(delimiter);
            novo = linha.substr(0, pos);
            xx = atof(novo.c_str());
            linha.erase(0, pos + delimiter.length()); // Apaga x + "," do ficheiro
            v.x = xx;
            


            pos = linha.find(delimiter);
            novo = linha.substr(0, pos);
            yy = atof(novo.c_str());
            linha.erase(0, pos + delimiter.length()); // Apaga y + "," do ficheiro
            v.y = yy;
            



            pos = linha.find(delimiter);
            novo = linha.substr(0, pos);
            zz = atof(novo.c_str());
            linha.erase(0, pos + delimiter.length()); // Apaga z + "\n" do ficheiro
            v.z = zz;
            


            allPoints.push_back(v); // Adiciona o vertex p ao vetor allPoints

        }

        file.close();


    }
    else {

        cout << "ERRO AO LER FICHEIRO" << endl;
    }
}


/*
* * Faz a leitura dos Groups do ficheiro xml
*/
transformation* readGroups(XMLElement* pGroup, transformation *tra){
    if (pGroup) {
            
            if (tra == NULL) {
                tra = new transformation();
            }
            
            
            XMLElement* pTransform = pGroup->FirstChildElement("transform");
            if (pTransform) {
                XMLElement* pTranslate = pTransform->FirstChildElement("translate");
                if (pTranslate) {
                    opTranslate *t = new opTranslate();
                    pTranslate->QueryFloatAttribute("time", &(t->time));
                    pTranslate->QueryBoolAttribute("align", &(t->align));
                    XMLElement* pTpoints = pTranslate->FirstChildElement("point");
                    while (pTpoints) {
                        vertex v;
                        pTpoints->QueryFloatAttribute("x", &(v.x));
                        pTpoints->QueryFloatAttribute("y", &(v.y));
                        pTpoints->QueryFloatAttribute("z", &(v.z));
                        t->points.push_back(v);
                        pTpoints = pTpoints->NextSiblingElement("point");

                    }
                    
                    vector<vector<vertex>> catVecs;
                    catVecs = geraPontosCurva(t->points);
                    t->pointsCatmull = catVecs[0];
                    t->deriv = catVecs[1];
                    

                    //pTranslate->QueryFloatAttribute("x", &(t->x));
                    //pTranslate->QueryFloatAttribute("y", &(t->y));
                    //pTranslate->QueryFloatAttribute("z", &(t->z));
                    //t->print();
                    tra->ops.push_back(t);
                }
                XMLElement* pRotate = pTransform->FirstChildElement("rotate");
                if (pRotate) {
                    
                    opRotate *t = new opRotate();
                    if (pRotate->QueryFloatAttribute("time", &(t->time)));
                    if (pRotate->QueryFloatAttribute("angle", &(t->angle)));
                    pRotate->QueryFloatAttribute("x", &(t->x));
                    pRotate->QueryFloatAttribute("y", &(t->y));
                    pRotate->QueryFloatAttribute("z", &(t->z));
                    //t->print();
                    tra->ops.push_back(t);
                    
                }
                XMLElement* pScale = pTransform->FirstChildElement("scale");
                if (pScale) {
                    
                    opScale *t = new opScale();
                    pScale->QueryFloatAttribute("x", &(t->x));
                    pScale->QueryFloatAttribute("y", &(t->y));
                    pScale->QueryFloatAttribute("z", &(t->z));
                    //t->print();
                    tra->ops.push_back(t);
                    

                }
            }
            XMLElement * pModels = pGroup -> FirstChildElement("models");
            if(NULL != pModels){
                
                XMLElement * pModel = pModels -> FirstChildElement("model");
                
                while(pModel){
                    string pathWindows = "D:/Uminho/CG/projeto/CG2122/FASE3/models/";
                    string fileName = pModel->Attribute("file");
                    readFile(pathWindows+fileName);
                    opDraw* draw = new opDraw();
                    pModel->QueryFloatAttribute("R", &(draw->r));
                    pModel->QueryFloatAttribute("G", &(draw->g));
                    pModel->QueryFloatAttribute("B", &(draw->b));
                    

                    draw->prep_vbo(allPoints);

                    
                    tra->ops.push_back(draw);
                    
                    allPoints.clear();
                    pModel = pModel -> NextSiblingElement("model");
                   

                   

                }
            }
           
           
            XMLElement * pGroup2 = pGroup -> FirstChildElement("group");
            while(pGroup2){
               tra->haveChild = true;
               transformation* child = NULL;
               child = readGroups(pGroup2, child);
               tra->childs.push_back(child);
               pGroup2 = pGroup2 -> NextSiblingElement("group"); 
               
            }
            
    }
    return tra;
}

/*
* * Faz a leitura do ficheiro XML
*/
transformation* readXML(){
    XMLDocument xmlDoc;
    
    XMLError eResult = xmlDoc.LoadFile("D:/Uminho/CG/projeto/CG2122/FASE3/XMLfiles/sistemaSolar.xml");
    XMLCheckResult(eResult)
    transformation* transfTemp = NULL;
    XMLNode * pRoot = xmlDoc.FirstChild();
    if (pRoot != nullptr) { 
        
        
        XMLElement * pCam = pRoot -> FirstChildElement("camera");
        if(NULL != pCam){
            
            XMLElement * pPos = pCam -> FirstChildElement("position");
            if (NULL != pPos) {
              vertex v ;
              pPos-> QueryFloatAttribute("x", &v.x);
              
              pPos-> QueryFloatAttribute("y", &v.y);
              

              pPos-> QueryFloatAttribute("z", &v.z);
              

              pointsCam.pos = v;
            
            }
            XMLElement * pLookAt = pCam -> FirstChildElement("lookAt");
            if (NULL != pLookAt) {
              vertex v ;
              pLookAt-> QueryFloatAttribute("x", &v.x);
              pLookAt-> QueryFloatAttribute("y", &v.y);
              pLookAt-> QueryFloatAttribute("z", &v.z);
              
              
              pointsCam.lookAt = v;
            
            } 
            XMLElement * pUp = pCam -> FirstChildElement("up");
            if (NULL != pUp) {
              vertex v ;
              pUp-> QueryFloatAttribute("x", &v.x);
              pUp-> QueryFloatAttribute("y", &v.y);
              pUp-> QueryFloatAttribute("z", &v.z);
              
              

              pointsCam.up = v;
            
            }
            XMLElement * pProj = pCam -> FirstChildElement("projection");
            if (NULL != pProj) {
               vertex v;
              
              pProj -> QueryFloatAttribute("fov", &v.x);
              pProj-> QueryFloatAttribute("near", &v.y);
              pProj -> QueryFloatAttribute("far", &v.z);
              
                            
              pointsCam.proj= v;
            
            }   
        }
        XMLElement* pGroup = pRoot->FirstChildElement("group");
        
        
        transfTemp = readGroups(pGroup,transfTemp);
       
    }
        px = pointsCam.pos.x;
        py = pointsCam.pos.y;
        pz = pointsCam.pos.z; 
        calcRadius();
        calcBeta();
        calcAlf();
        

        return transfTemp;
}


void reshape(int w, int h){
    // Prevent a divide by zero, when window is too short
    // (you cant make a window with zero width).
    if(h == 0)
        h = 1;
    float ratio = w *1.0 / h;
    // Set the projection matrix as current
    glMatrixMode(GL_PROJECTION);
    // Load Identity Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set perspective
    gluPerspective(pointsCam.proj.x, ratio, pointsCam.proj.y, pointsCam.proj.z);
    
    


    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}


/*
* * Realiza transformações e desenha as figuras  
*/
void drawFigures(transformation* t) {
    glPushMatrix();
    for (int i = 0; i < t->ops.size(); i++) {
        t->ops[i]->run();
    }
    if (t->haveChild) {
        for (int i = 0; i < t->childs.size(); i++) {
            drawFigures(t->childs[i]);
        }
    }
    glPopMatrix();
}

void renderScene(void){

    
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    glLoadIdentity();
    
    
    
    gluLookAt(polarX(), polarY(), polarZ(),
              pointsCam.lookAt.x,pointsCam.lookAt.y,pointsCam.lookAt.z,
              pointsCam.up.x, pointsCam.up.y, pointsCam.up.z);

    
    glTranslatef(tx, ty, tz);
    glRotatef(angle, 0.0, 1.0, 0.0);
    glScalef(scale,scale,scale);
    glPolygonMode(GL_FRONT_AND_BACK,draw);
    
    
    // transformations


    glBegin(GL_LINES);
// X axis in red
    glColor3f(1.0f, 0.0f, 0.0f);
    
    glVertex3f(-100.0f, 0.0f, 0.0f);
    glVertex3f( 100.0f, 0.0f, 0.0f);
// Y Axis in Green
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -100.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);
// Z Axis in Blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -100.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();

    
    // Do operations and Draw figures 

    transformation* taux = NULL;
    taux = new transformation;
    taux = transf;
    
    drawFigures(taux);
    
    glutSwapBuffers();
    
    
    

}




void keyboardspecial(int key, int x , int y){
    switch(key){
        case GLUT_KEY_UP:
            tz-=1;
            break;
        case GLUT_KEY_DOWN:
            tz+=1;
            break;
        case GLUT_KEY_LEFT:
            tx-=1;
            break;
        case GLUT_KEY_RIGHT:
            tx+=1;
            break;
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int a, int b) {
    switch (key) {
        case 'a':
        case 'A':
            angle -= 10;

            break;
        case 'd':
        case 'D':
            angle += 10;
            break;

        case 'w':
        case 'W':
            ty+=1;

            break;
        case 's':
        case 'S':
            ty-=1;
            break;

        case 'p':
        case 'P':
            draw = GL_POINT;
            break;

        case 'l':
        case 'L':
            draw = GL_LINE;
            break;

        case 'f':
        case 'F':
            draw = GL_FILL;
            break;

        case '-':
            scale-=0.1;
            break;

        case '+':
            scale+=0.1;
            break;

    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    //init glut and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("CG_Project");

    // put callback registration here
    glutDisplayFunc(renderScene);
    glutReshapeFunc(reshape);
    glutIdleFunc(renderScene);
    glutSpecialFunc(keyboardspecial);
    glutKeyboardFunc(keyboard);
    
    // init glew
#ifndef __APPLE__
    glewInit();
#endif
    glEnableClientState(GL_VERTEX_ARRAY);
    transf = readXML();

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // enter GLUT's main loop
    glutMainLoop();

    return 1;

}

