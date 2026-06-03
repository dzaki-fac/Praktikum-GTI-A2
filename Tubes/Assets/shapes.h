#ifndef SHAPES_H
#define SHAPES_H

void drawBox(float w, float h, float d);

void drawBoxTextured(
    float w,
    float h,
    float d,
    float uTile,
    float vTile
);

void drawShadow(float radiusX, float radiusZ);

static void glShadowProjection(float *l, float *e, float *n);

#endif