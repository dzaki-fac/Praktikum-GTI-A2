#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef _WIN32
#include <windows.h>
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

GLuint loadBMP(const char *filename);

#endif