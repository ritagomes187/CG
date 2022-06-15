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

#include <IL/il.h>

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



class Color {
    public:
        float dif[4];
        float amb[4];
        float emi[4];
        float esp[4];
        float shine;
        int lightSize;

        
        void normalizeValues(vertex d, vertex a, vertex em, vertex es) {
            dif[0] =  d.x / 255.0f ; dif[1] = d.y / 255.0f ; dif[2] = d.z / 255.0f ; dif[3] = 1.0f ;
            amb[0] =  a.x / 255.0f ; amb[1] = a.y / 255.0f ; amb[2] = a.z / 255.0f ; amb[3] = 1.0f ;
            emi[0] = em.x / 255.0f ; emi[1] = em.y / 255.0f ; emi[2] = em.z / 255.0f ; emi[3] = 1.0f ;
            esp[0] =  es.x / 255.0f ; esp[1] = es.y / 255.0f ; esp[2] = es.z / 255.0f ; esp[3] = 1.0f ;
        }

        void runDefault() {
            for (int i = 0; i < lightSize; i++) {
                glLightfv(GL_LIGHT0+i, GL_DIFFUSE, dif);
                glLightfv(GL_LIGHT0+i, GL_AMBIENT, amb);
                glLightfv(GL_LIGHT0+i, GL_SPECULAR, esp);
                glLightfv(GL_LIGHT0+i, GL_EMISSION, emi);
            }
        }

        void run() {
            glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
            
            glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
            
            glMaterialfv(GL_FRONT, GL_EMISSION, emi);
            
            glMaterialfv(GL_FRONT, GL_SPECULAR, esp);
            glMaterialf(GL_FRONT, GL_SHININESS, shine);
        }
};


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
    GLuint vertec, normals, texCoord;
    GLuint texID = -1;
    GLuint vertexCount = 0;
    Color col;

public:    
    void prep_vbo(vector<vertex> verts, vector<vertex> norm, vector<float> tex) {
        vertexCount = verts.size();
        
        glGenBuffers(1, &vertec);
        glBindBuffer(GL_ARRAY_BUFFER, vertec);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float) * 3, verts.data(), GL_STATIC_DRAW);
        
        glGenBuffers(1, &normals);
        glBindBuffer(GL_ARRAY_BUFFER, normals);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * norm.size() * 3, norm.data(), GL_STATIC_DRAW);
        
        if (texID!= -1 ){
            glGenBuffers(1, &texCoord);
            glBindBuffer(GL_ARRAY_BUFFER, texCoord);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * tex.size(), &(tex[0]), GL_STATIC_DRAW);
        }

    }


    void run() {
        
        if (texID != -1){
            col.runDefault();
            glBindTexture(GL_TEXTURE_2D, texID);
            glBindBuffer(GL_ARRAY_BUFFER, vertec);
            glVertexPointer(3, GL_FLOAT, 0, 0);
            glBindBuffer(GL_ARRAY_BUFFER, normals);
            glNormalPointer(GL_FLOAT, 0, 0);
            glBindBuffer(GL_ARRAY_BUFFER, texCoord);
            glTexCoordPointer(2, GL_FLOAT, 0, 0);
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else{
            col.run();
            glBindBuffer(GL_ARRAY_BUFFER, vertec);
            glVertexPointer(3, GL_FLOAT, 0, 0);
            glBindBuffer(GL_ARRAY_BUFFER, normals);
            glNormalPointer(GL_FLOAT, 0, 0);  
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        }
    }
    void print(){}
};



struct transformation {
    bool haveChild = false;
    vector<operations*> ops;
    vector<transformation*> childs;
};

class Luz {
public:
    vertex pos;
    vertex dir;
    float cutoff;
    string type;
};



using namespace tinyxml2;

using namespace std;




transformation *transf = NULL;
camPos cp;
vector<vertex> allPoints;
vector<float> texPoints;
vector<vertex> normPoints;
vector<Luz> lights;
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

int loadTex(std::string s) {
    unsigned int t,tw,th;
    unsigned char *texData;
    unsigned int texID;

    ilInit();
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    ilGenImages(1,&t);
    ilBindImage(t);
    ilLoadImage((ILstring)s.c_str());
    tw = ilGetInteger(IL_IMAGE_WIDTH);
    th = ilGetInteger(IL_IMAGE_HEIGHT);
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    texData = ilGetData();

    glGenTextures(1,&texID);

    glBindTexture(GL_TEXTURE_2D,texID);
    glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_WRAP_S,      GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_WRAP_T,      GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_MAG_FILTER,      GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;

}

void readFile(string fich) {

    string linha; // linha que contem 1 vertice
    string novo; // 1 posição
    string delimiter = ","; // x , y , z
    int pos; // Posição antes do delimitador no ficheiro
    float xx, yy, zz; // Coordenadas
    vertex v;
    int i = 0;

    //fich = "../models/" + fich; // ficheiro .3d

    ifstream file(fich);

    if (file.is_open()) {

        while (getline(file, linha)) {
            if (i==9) i = 0;
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
            

            if (i < 3) allPoints.push_back(v); // Adiciona o vertex p ao vetor allPoints
            else if (i >= 3 && i < 6 ) normPoints.push_back(v);
            else{
                texPoints.push_back(v.x);
                texPoints.push_back(v.y);
            }
            i++;
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
                    if (pTpoints) {
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
                    }
                    else{
                        vertex v;
                        pTranslate->QueryFloatAttribute("x", &(v.x));
                        pTranslate->QueryFloatAttribute("y", &(v.y));
                        pTranslate->QueryFloatAttribute("z", &(v.z));
                        t->points.push_back(v);
                        t->time = 0;
                    }
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
                    
                    tra->ops.push_back(t);
                    
                }
                XMLElement* pScale = pTransform->FirstChildElement("scale");
                if (pScale) {
                    
                    opScale *t = new opScale();
                    pScale->QueryFloatAttribute("x", &(t->x));
                    pScale->QueryFloatAttribute("y", &(t->y));
                    pScale->QueryFloatAttribute("z", &(t->z));
                    
                    tra->ops.push_back(t);
                    

                }
            }
            XMLElement * pModels = pGroup -> FirstChildElement("models");
            if(NULL != pModels){
                
                XMLElement * pModel = pModels -> FirstChildElement("model");
                
                while(pModel){
                    string pathWindows = "D:/Uminho/CG/projeto/CG2122/FASE4/models/";
                    string fileName = pModel->Attribute("file");
                    readFile(pathWindows+fileName);
                    opDraw* draw = new opDraw();
                    
                    XMLElement* pColor = pModel->FirstChildElement("color");
                    if (pColor) {
                        draw->col.lightSize = lights.size();
                        XMLElement* pDiffuse = pColor->FirstChildElement("diffuse");
                        vertex difuso;
                        pDiffuse->QueryFloatAttribute("R", &(difuso.x));
                        pDiffuse->QueryFloatAttribute("G", &(difuso.y));
                        pDiffuse->QueryFloatAttribute("B", &(difuso.z));

                        XMLElement* pAmbient = pColor->FirstChildElement("ambient");
                        vertex ambiente;
                        pAmbient->QueryFloatAttribute("R", &(ambiente.x));
                        pAmbient->QueryFloatAttribute("G", &(ambiente.y));
                        pAmbient->QueryFloatAttribute("B", &(ambiente.z));

                        XMLElement* pSpecular = pColor->FirstChildElement("specular");
                        vertex espec;
                        pSpecular->QueryFloatAttribute("R", &(espec.x));
                        pSpecular->QueryFloatAttribute("G", &(espec.y));
                        pSpecular->QueryFloatAttribute("B", &(espec.z));
                        
                        XMLElement* pEmissive = pColor->FirstChildElement("emissive");
                        vertex emiss;
                        pEmissive->QueryFloatAttribute("R", &(emiss.x));
                        pEmissive->QueryFloatAttribute("G", &(emiss.y));
                        pEmissive->QueryFloatAttribute("B", &(emiss.z));

                        XMLElement* pShine = pColor->FirstChildElement("shininess");
                        pShine->QueryFloatAttribute("value", &draw->col.shine);

                        draw->col.normalizeValues(difuso,ambiente,emiss,espec);
                        
                        
                        
                     
                    }
                    XMLElement* pTexture = pModel->FirstChildElement("texture");
                    if (pTexture) {
                        string s = pTexture->Attribute("file");
                        s = "D:/Uminho/CG/projeto/CG2122/FASE4/textures/" + s;
                        draw->texID = loadTex(s);
                    }
                    draw->prep_vbo(allPoints, normPoints, texPoints);
                    tra->ops.push_back(draw);
                    
                    allPoints.clear();
                    normPoints.clear();
                    texPoints.clear();
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
    
    XMLError eResult = xmlDoc.LoadFile("D:/Uminho/CG/projeto/CG2122/FASE4/XMLfiles/test_4_7.xml");
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
        
        XMLElement* pLights = pRoot->FirstChildElement("lights");

        if (NULL != pLights) {

            XMLElement* pLight = pLights->FirstChildElement("light");

            while (pLight) {
                Luz l ;
                string type; 
                type = pLight->Attribute("type");
                if(type == "point"){
                    pLight->QueryFloatAttribute("posx", &(l.pos.x));
                    pLight -> QueryFloatAttribute("posy", &(l.pos.y));
                    pLight -> QueryFloatAttribute("posz", &(l.pos.z));
                }
                else if(type == "directional"){
                    pLight->QueryFloatAttribute("dirx", &(l.dir.x));
                    pLight -> QueryFloatAttribute("diry", &(l.dir.y));
                    pLight -> QueryFloatAttribute("dirz", &(l.dir.z));
                }
                else{
                    pLight->QueryFloatAttribute("posx", &(l.pos.x));                   
                    pLight -> QueryFloatAttribute("posy", &(l.pos.y));                                       
                    pLight -> QueryFloatAttribute("posz", &(l.pos.z));                                    
                    pLight->QueryFloatAttribute("dirx", &(l.dir.x));                                       
                    pLight -> QueryFloatAttribute("diry", &(l.dir.y));                                       
                    pLight -> QueryFloatAttribute("dirz", &(l.dir.z));
                    pLight -> QueryFloatAttribute("cutoff", &(l.cutoff));
                }
                l.type = type;
                lights.push_back(l);
                pLight = pLight->NextSiblingElement("light");
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


void enableLight(int i, string type, Luz l) {
    switch (i)
    {
    case 1:
        if (type == "point") {
            float pos[4] = { l.pos.x, l.pos.y, l.pos.z, 1.0f };
            glLightfv(GL_LIGHT1, GL_POSITION, pos);
        }
        else if (type == "directional") {
            
            float dir[4] = { l.dir.x, l.dir.y, l.dir.z, 0.0f };
            glLightfv(GL_LIGHT1, GL_POSITION, dir);
        }
        else if (type == "spotlight") {
            float pos[4] = { l.pos.x, l.pos.y, l.pos.z, 1.0f };
            float dir[4] = { l.dir.x, l.dir.y, l.dir.z, 0.0f };
            glLightfv(GL_LIGHT1, GL_POSITION, pos);
            glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dir);
            glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, l.cutoff);
            glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 0.0);
        }
        break;
    case 2:
        if (type == "point") {
            float pos[4] = { l.pos.x, l.pos.y, l.pos.z, 1.0f };
            glLightfv(GL_LIGHT2, GL_POSITION, pos);
        }
        else if (type == "directional") {
            
            float dir[4] = { l.dir.x, l.dir.y, l.dir.z, 0.0f };
            glLightfv(GL_LIGHT2, GL_POSITION, dir);
        }
        else if (type == "spotlight") {
            float pos[4] = { l.pos.x, l.pos.y, l.pos.z, 1.0f };
            float dir[4] = { l.dir.x, l.dir.y, l.dir.z, 0.0f };
            glLightfv(GL_LIGHT2, GL_POSITION, pos);
            glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, dir);
            glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, l.cutoff);
            glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 0.0);
        }
        break;

    case 3:
        if (type == "point") {
            float pos[4] = { l.pos.x, l.pos.y, l.pos.z, 1.0f };
            glLightfv(GL_LIGHT3, GL_POSITION, pos);
        }
        else if (type == "directional") {
            
            float dir[4] = { l.dir.x, l.dir.y, l.dir.z, 0.0f };
            glLightfv(GL_LIGHT3, GL_POSITION, dir);
            
        }
        else if (type == "spotlight") {
            float pos[4] = { l.pos.x, l.pos.y, l.pos.z, 1.0f };
            float dir[4] = { l.dir.x, l.dir.y, l.dir.z, 0.0f };
            glLightfv(GL_LIGHT3, GL_POSITION, pos);
            glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, dir);
            glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, l.cutoff);
            glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 0.0);
        }
        break;

    case 4:
        if (type == "point") {
            float pos[4] = { l.pos.x, l.pos.y, l.pos.z, 1.0f };
            glLightfv(GL_LIGHT4, GL_POSITION, pos);
        }
        else if (type == "directional") {
            
            float dir[4] = { l.dir.x, l.dir.y, l.dir.z, 0.0f };
            glLightfv(GL_LIGHT4, GL_POSITION, dir);
            
        }
        else if (type == "spotlight") {
            float pos[4] = { l.pos.x, l.pos.y, l.pos.z, 1.0f };
            float dir[4] = { l.dir.x, l.dir.y, l.dir.z, 0.0f };
            glLightfv(GL_LIGHT4, GL_POSITION, pos);
            glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, dir);
            glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, l.cutoff);
            glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, 0.0);
        }
        break;

    case 5:
        if (type == "point") {
            float pos[4] = { l.pos.x, l.pos.y, l.pos.z, 1.0f };
            glLightfv(GL_LIGHT5, GL_POSITION, pos);
        }
        else if (type == "directional") {
            
            float dir[4] = { l.dir.x, l.dir.y, l.dir.z, 0.0f };
            glLightfv(GL_LIGHT5, GL_POSITION, dir);

        }
        else if (type == "spotlight") {
            float pos[4] = { l.pos.x, l.pos.y, l.pos.z, 1.0f };
            float dir[4] = { l.dir.x, l.dir.y, l.dir.z, 0.0f };
            glLightfv(GL_LIGHT5, GL_POSITION, pos);
            glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, dir);
            glLightf(GL_LIGHT5, GL_SPOT_CUTOFF, l.cutoff);
            glLightf(GL_LIGHT5, GL_SPOT_EXPONENT, 0.0);
        }
        break;

    case 6:
        if (type == "point") {
            float pos[4] = { l.pos.x, l.pos.y, l.pos.z, 1.0f };
            glLightfv(GL_LIGHT6, GL_POSITION, pos);
        }
        else if (type == "directional") {
            
            float dir[4] = { l.dir.x, l.dir.y, l.dir.z, 0.0f };
            glLightfv(GL_LIGHT6, GL_POSITION, dir);
            
        }
        else if (type == "spotlight") {
            float pos[4] = { l.pos.x, l.pos.y, l.pos.z, 1.0f };
            float dir[4] = { l.dir.x, l.dir.y, l.dir.z, 0.0f };
            glLightfv(GL_LIGHT6, GL_POSITION, pos);
            glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, dir);
            glLightf(GL_LIGHT6, GL_SPOT_CUTOFF, l.cutoff);
            glLightf(GL_LIGHT6, GL_SPOT_EXPONENT, 0.0);
        }
        break;

    case 7:

        if (type == "point") {
            float pos[4] = { l.pos.x, l.pos.y, l.pos.z, 1.0f };
            glLightfv(GL_LIGHT7, GL_POSITION, pos);
        }
        else if (type == "directional") {
            
            float dir[4] = { l.dir.x, l.dir.y, l.dir.z, 0.0f };
            glLightfv(GL_LIGHT7, GL_POSITION, dir);
            
        }
        else if (type == "spotlight") {
            float pos[4] = { l.pos.x, l.pos.y, l.pos.z, 1.0f };
            float dir[4] = { l.dir.x, l.dir.y, l.dir.z, 0.0f };
            glLightfv(GL_LIGHT7, GL_POSITION, pos);
            glLightfv(GL_LIGHT7, GL_SPOT_DIRECTION, dir);
            glLightf(GL_LIGHT7, GL_SPOT_CUTOFF, l.cutoff);
            glLightf(GL_LIGHT7, GL_SPOT_EXPONENT, 0.0);
        }
        break;

    default:
        if (type == "point") {
            float pos[4] = { l.pos.x, l.pos.y, l.pos.z, 1.0f };
            glLightfv(GL_LIGHT0, GL_POSITION, pos);
        }
        else if (type == "directional") {
            
            float dir[4] = { l.dir.x, l.dir.y, l.dir.z, 0.0f };
            glLightfv(GL_LIGHT0, GL_POSITION, dir);
            
        }
        else if (type == "spotlight") {
            float pos[4] = { l.pos.x, l.pos.y, l.pos.z, 1.0f };
            float dir[4] = { l.dir.x, l.dir.y, l.dir.z, 0.0f };
            glLightfv(GL_LIGHT0, GL_POSITION, pos);
            glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
            glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, l.cutoff);
            glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0.0);
        }
        break;
    }
}

void renderScene(void){

    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // set the camera
    glLoadIdentity();
    gluLookAt(polarX(), polarY(), polarZ(),
              pointsCam.lookAt.x,pointsCam.lookAt.y,pointsCam.lookAt.z,
              pointsCam.up.x, pointsCam.up.y, pointsCam.up.z);
    

    for (int i = 0; i < lights.size(); i++){
        string lightType = lights[i].type;
        enableLight(i, lightType, lights[i]);
    }
    
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

void initGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_RESCALE_NORMAL);
    glClearColor(0, 0, 0, 0);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1); 
    glEnable(GL_LIGHT2); 
    glEnable(GL_LIGHT3);
    glEnable(GL_LIGHT4);
    glEnable(GL_LIGHT5);
    glEnable(GL_LIGHT6);
    glEnable(GL_LIGHT7);
    glEnable(GL_TEXTURE_2D);
    
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
    
    initGL();
    
    transf = readXML();
    printf("%d\n", lights.size());


    
    // enter GLUT's main loop
    glutMainLoop();

    return 1;

}

