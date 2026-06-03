#ifndef CHICKEN_H
#define CHICKEN_H

void drawChicken(float bobAngle, int isShadow);

void renderChickenWithShadow(
    float* lightPosGL, 
    float* planePoint, 
    float* planeNormal, 
    float bobAngle,
    float posX,  // <-- Tambahkan ini
    float posZ   // <-- Tambahkan ini
);

#endif