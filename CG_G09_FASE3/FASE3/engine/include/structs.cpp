#include "structs.h"

vertex::vertex() {
    x = 0;
    y = 0;
    z = 0;
}
vertex::vertex(float a, float b, float c) {
    x = a;
    y = b;
    z = c;
}

camera::camera() {
        pos = vertex();
        lookAt = vertex();
        up = vertex();
        proj = vertex();
}


