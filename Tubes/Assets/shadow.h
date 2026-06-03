#ifndef SHADOW_H
#define SHADOW_H

#ifdef _WIN32
    #include <windows.h>
    #include <GL/freeglut.h>
#else
    #include <GL/glut.h>
#endif

void buildShadowMatrix(
    float shadowMat[16],
    float lightPos[4],
    float planePoint[3],
    float planeNormal[3]
);

#endif