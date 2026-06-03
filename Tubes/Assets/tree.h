#ifndef TREE_H
#define TREE_H

#ifdef _WIN32
#include <windows.h>
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

void drawTree(float px, float pz, GLuint texBark, GLuint texLeaf, int isShadow);

#endif