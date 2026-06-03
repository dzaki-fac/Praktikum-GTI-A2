#ifndef WORLD_H
#define WORLD_H

#include <GL/freeglut.h>
#include "game.h"

/*
 * Render dunia berdasarkan posisi ayam.
 * Hanya lane dalam jangkauan LANES_BEHIND..LANES_AHEAD yang digambar.
 */
void drawWorld(World *w,
               float chickenZ,
               GLuint texGrass,
               GLuint texBark,
               GLuint texLeaf,
               GLuint texRock,
               float *lightPos);

#endif
