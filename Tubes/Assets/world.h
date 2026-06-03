#ifndef WORLD_H
#define WORLD_H

#include <GL/freeglut.h>


void drawWorld(float roadLen,
               GLuint texGrass,
               GLuint texBark,
               GLuint texLeaf,
               float *lightPos);

#endif