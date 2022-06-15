#ifndef STRUCTS_H
#define STRUCTS_H

class vertex{

public:
    float x;
    float y;
    float z;
    
public:
    vertex();
    vertex(float a, float b, float c) ;
};

class camera {

public:
    vertex pos;
    vertex lookAt;
    vertex up;
    vertex proj;
    

public:
    camera();
};

struct camPos {
    float raio;
    float alpha;
    float beta;
};




#endif