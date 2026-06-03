#ifdef _WIN32
#include <windows.h>
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

#include "shadow.h"

void buildShadowMatrix(
    float shadowMat[16],
    float lightPos[4],
    float planePoint[3],
    float planeNormal[3]
) {
    float a = planeNormal[0];
    float b = planeNormal[1];
    float c = planeNormal[2];

    float d =
        -(a * planePoint[0] +
          b * planePoint[1] +
          c * planePoint[2]);

    float lx = lightPos[0];
    float ly = lightPos[1];
    float lz = lightPos[2];
    float lw = lightPos[3];

    float dot =
        a * lx +
        b * ly +
        c * lz +
        d * lw;

    shadowMat[0]  = dot - lx * a;
    shadowMat[4]  = -lx * b;
    shadowMat[8]  = -lx * c;
    shadowMat[12] = -lx * d;

    shadowMat[1]  = -ly * a;
    shadowMat[5]  = dot - ly * b;
    shadowMat[9]  = -ly * c;
    shadowMat[13] = -ly * d;

    shadowMat[2]  = -lz * a;
    shadowMat[6]  = -lz * b;
    shadowMat[10] = dot - lz * c;
    shadowMat[14] = -lz * d;

    shadowMat[3]  = -lw * a;
    shadowMat[7]  = -lw * b;
    shadowMat[11] = -lw * c;
    shadowMat[15] = dot - lw * d;
}