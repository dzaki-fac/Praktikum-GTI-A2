#ifdef _WIN32
  #include <windows.h>
  #include <GL/freeglut.h>
#else
  #include <GL/glut.h>
#endif

#include <math.h>
#include "shapes.h"
#include "chicken.h"


static void drawChickenHead(float bob) {
    glPushMatrix();
    glTranslatef(0, 0.55f + bob, 0.3f);

    glColor3f(0.95f, 0.85f, 0.5f);
    drawBox(0.45f, 0.4f, 0.45f);

    /* Paruh */
    glPushMatrix();
    glTranslatef(0, -0.05f, 0.28f);
    glColor3f(1.0f, 0.55f, 0.0f);
    drawBox(0.15f, 0.1f, 0.15f);
    glPopMatrix();

    /* Mata kiri */
    glPushMatrix();
    glTranslatef(-0.17f, 0.09f, 0.1f);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawBox(0.08f, 0.08f, 0.05f);
    glPopMatrix();

    /* Mata kanan */
    glPushMatrix();
    glTranslatef(0.17f, 0.09f, 0.1f);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawBox(0.08f, 0.08f, 0.05f);
    glPopMatrix();

    glPopMatrix();
}

static void drawChickenBody() {
    glColor3f(0.95f, 0.9f, 0.6f);
    drawBox(0.7f, 0.6f, 0.9f);
}

static void drawChickenComb(float bob) {
    glPushMatrix();
    glTranslatef(0, 0.85f + bob, 0.3f);

    glColor3f(1.0f, 0.1f, 0.1f);
    drawBox(0.12f, 0.2f, 0.12f);

    glPopMatrix();
}

static void drawChickenWing(float side) {
    glPushMatrix();

    glTranslatef(side * 0.42f, 0.05f, 0.0f);
    glRotatef(side * 20.0f, 0, 0, 1);

    glColor3f(0.85f, 0.75f, 0.4f);
    drawBox(0.15f, 0.45f, 0.7f);

    glPopMatrix();
}

static void drawChickenLeg(float side) {
    glPushMatrix();

    glTranslatef(side * 0.15f, -0.38f, 0.0f);

    glColor3f(1.0f, 0.55f, 0.0f);
    drawBox(0.1f, 0.25f, 0.1f);

    glPopMatrix();
}

static void drawChickenTail() {
    glPushMatrix();

    glTranslatef(0, 0.2f, -0.52f);
    glRotatef(35, 1, 0, 0);

    glColor3f(0.7f, 0.6f, 0.3f);
    drawBox(0.5f, 0.35f, 0.25f);

    glPopMatrix();
}


void drawChicken(float bobAngle) {
    float bob = (float)sin(bobAngle) * 0.08f;

    glPushMatrix();

    drawChickenBody();
    drawChickenHead(bob);
    drawChickenComb(bob);

    drawChickenWing(-1.0f);
    drawChickenWing( 1.0f);

    drawChickenLeg(-1.0f);
    drawChickenLeg( 1.0f);

    drawChickenTail();

    glPopMatrix();
}