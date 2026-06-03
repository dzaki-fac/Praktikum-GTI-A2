#ifndef CAR_H
#define CAR_H

typedef struct {
    float x, z;
    float speed;
    float r, g, b;
    float width, panjang;
} Car;

void drawCar(const Car *c, int isShadow);
void renderCarWithShadow(
    float *lightPos,
    float *planePoint,
    float *planeNormal,
    const Car *c
);

#endif

#ifdef _WIN32
  #include <windows.h>
  #include <GL/freeglut.h>
#else
  #include <GL/glut.h>
#endif

#include "shapes.h"
#include "shadow.h"
#include "car.h"

static void setCarColor(int isShadow, float r, float g, float b) {
    if (isShadow)
        glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    else
        glColor4f(r, g, b, 1.0f);
}

static void drawCarBody(const Car *c, int isShadow) {
    setCarColor(isShadow, c->r, c->g, c->b);
    drawBox(c->panjang, 0.35f, 0.9f);
}

static void drawCarCabin(const Car *c, int isShadow) {
    glPushMatrix();
    glTranslatef(-0.1f, 0.28f, 0.0f);
    setCarColor(isShadow, c->r * 0.7f, c->g * 0.7f, c->b * 0.7f);
    drawBox(c->panjang * 0.6f, 0.28f, 0.7f);
    glPopMatrix();
}

static void drawCarWindow(const Car *c, float halfLength, int isShadow) {
    glPushMatrix();
    glTranslatef(halfLength * 0.3f, 0.3f, 0.0f);
    setCarColor(isShadow, 0.6f, 0.8f, 1.0f);
    drawBox(0.1f, 0.22f, 0.6f);
    glPopMatrix();
}

static void drawCarWheels(float halfLength, float halfWidth, int isShadow) {
    int i;
    float wx[2] = { halfLength - 0.2f, -(halfLength - 0.2f) };
    float wz = halfWidth + 0.05f;

    setCarColor(isShadow, 0.15f, 0.15f, 0.15f);

    for (i = 0; i < 2; i++) {
        glPushMatrix();
        glTranslatef(wx[i], -0.2f, -wz);
        drawBox(0.22f, 0.22f, 0.12f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(wx[i], -0.2f, wz);
        drawBox(0.22f, 0.22f, 0.12f);
        glPopMatrix();
    }
}

static void drawCarLights(float halfLength, float front, int isShadow) {
    setCarColor(isShadow, 1.0f, 1.0f, 0.7f);

    glPushMatrix();
    glTranslatef(front * halfLength, 0.1f, -0.3f);
    drawBox(0.05f, 0.1f, 0.15f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(front * halfLength, 0.1f, 0.3f);
    drawBox(0.05f, 0.1f, 0.15f);
    glPopMatrix();

    setCarColor(isShadow, 1.0f, 0.2f, 0.2f);

    glPushMatrix();
    glTranslatef(-front * halfLength, 0.1f, -0.3f);
    drawBox(0.05f, 0.1f, 0.15f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-front * halfLength, 0.1f, 0.3f);
    drawBox(0.05f, 0.1f, 0.15f);
    glPopMatrix();
}

void drawCar(const Car *c, int isShadow) {
    float halfLength = c->panjang / 2.0f;
    float halfWidth  = 0.9f / 2.0f;
    float front      = (c->speed > 0) ? 1.0f : -1.0f;

    glPushMatrix();
    glTranslatef(c->x, 0.35f, c->z);

    drawCarBody(c, isShadow);
    drawCarCabin(c, isShadow);
    drawCarWindow(c, halfLength, isShadow);
    drawCarWheels(halfLength, halfWidth, isShadow);
    drawCarLights(halfLength, front, isShadow);

    glPopMatrix();
}

void renderCarWithShadow(
    float *lightPos,
    float *planePoint,
    float *planeNormal,
    const Car *c
) {
    float shadowMat[16];
    buildShadowMatrix(shadowMat, lightPos, planePoint, planeNormal);

    /* ── Shadow ───────────────────────────────────────────────── */
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_STENCIL_BUFFER_BIT);

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-2.0f, -2.0f);

    glClear(GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

    glPushMatrix();
    glMultMatrixf(shadowMat);
    drawCar(c, 1);
    glPopMatrix();

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopAttrib();

    /* ── Mobil asli ───────────────────────────────────────────── */
    glEnable(GL_LIGHTING);
    drawCar(c, 0);
}