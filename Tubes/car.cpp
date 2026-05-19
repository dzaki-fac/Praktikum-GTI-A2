#ifdef _WIN32
  #include <windows.h>
  #include <GL/freeglut.h>
#else
  #include <GL/glut.h>
#endif

#include "shapes.h"
#include "car.h"


static void drawCarBody(const Car *c) {
    glColor3f(c->r, c->g, c->b);
    drawBox(c->panjang, 0.35f, 0.9f);
}

static void drawCarCabin(const Car *c) {
    glPushMatrix();

    glTranslatef(-0.1f, 0.28f, 0.0f);

    glColor3f(c->r * 0.7f, c->g * 0.7f, c->b * 0.7f);
    drawBox(c->panjang * 0.6f, 0.28f, 0.7f);

    glPopMatrix();
}

static void drawCarWindow(const Car *c, float halfLength) {
    glPushMatrix();

    glTranslatef(halfLength * 0.3f, 0.3f, 0.0f);

    glColor3f(0.6f, 0.8f, 1.0f);
    drawBox(0.1f, 0.22f, 0.6f);

    glPopMatrix();
}

static void drawCarWheels(float halfLength, float halfWidth) {
    int i;

    float wx[2] = {
        halfLength - 0.2f,
        -(halfLength - 0.2f)
    };

    float wz = halfWidth + 0.05f;

    glColor3f(0.15f, 0.15f, 0.15f);

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

static void drawCarLights(float halfLength, float front) {
    /* Lampu depan */
    glColor3f(1.0f, 1.0f, 0.7f);

    glPushMatrix();
    glTranslatef(front * halfLength, 0.1f, -0.3f);
    drawBox(0.05f, 0.1f, 0.15f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(front * halfLength, 0.1f, 0.3f);
    drawBox(0.05f, 0.1f, 0.15f);
    glPopMatrix();

    /* Lampu belakang */
    glColor3f(1.0f, 0.2f, 0.2f);

    glPushMatrix();
    glTranslatef(-front * halfLength, 0.1f, -0.3f);
    drawBox(0.05f, 0.1f, 0.15f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-front * halfLength, 0.1f, 0.3f);
    drawBox(0.05f, 0.1f, 0.15f);
    glPopMatrix();
}


void drawCar(const Car *c) {
    float halfLength = c->panjang / 2.0f;
    float halfWidth  = 0.9f / 2.0f;

    float front = (c->speed > 0) ? 1.0f : -1.0f;

    glPushMatrix();

    glTranslatef(c->x, 0.35f, c->z);

    drawCarBody(c);
    drawCarCabin(c);
    drawCarWindow(c, halfLength);
    drawCarWheels(halfLength, halfWidth);
    drawCarLights(halfLength, front);

    glPopMatrix();
}