#ifndef CAR_H
#define CAR_H

typedef struct {
    float x, z;
    float speed;
    float r, g, b;
    float width, panjang;
} Car;

void drawCar(const Car *c);

#endif