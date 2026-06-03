#ifndef CAR_H
#define CAR_H

typedef struct {
    float x, z;
    float speed;
    float r, g, b;
    float width, panjang;
} Car;

void drawCar(const Car *c);
// Tambah deklarasi di bagian #ifndef CAR_H
void drawCarShadow(const Car *c);

void renderCarWithShadow(
    float *lightPos,
    float *planePoint,
    float *planeNormal,
    const Car *c
);

#endif