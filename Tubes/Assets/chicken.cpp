#ifdef _WIN32
  #include <windows.h>
  #include <GL/freeglut.h>
#else
  #include <GL/glut.h>
#endif

#include <math.h>
#include "shapes.h"
#include "chicken.h"
#include "shadow.h"

static void setChickenColor(
    int isShadow,
    float r,
    float g,
    float b
) {
    if (isShadow)
        // 0.4f adalah nilai alpha untuk transparansi bayangan (40% opasitas)
        glColor4f(0.0f, 0.0f, 0.0f, 0.4f); 
    else
        // Warna normal ayam menggunakan alpha penuh (1.0f / tidak transparan)
        glColor4f(r, g, b, 1.0f); 
}

static void drawChickenHead(float bob, int isShadow) {
    glPushMatrix();

    glTranslatef(0, 0.55f + bob, 0.3f);

    setChickenColor(isShadow, 0.95f, 0.85f, 0.5f);
    drawBox(0.45f, 0.4f, 0.45f);

    /* Paruh */
    glPushMatrix();

    glTranslatef(0, -0.05f, 0.28f);

    setChickenColor(isShadow, 1.0f, 0.55f, 0.0f);
    drawBox(0.15f, 0.1f, 0.15f);

    glPopMatrix();

    /* Mata kiri */
    glPushMatrix();

    glTranslatef(-0.17f, 0.09f, 0.1f);

    setChickenColor(isShadow, 0.1f, 0.1f, 0.1f);
    drawBox(0.08f, 0.08f, 0.05f);

    glPopMatrix();

    /* Mata kanan */
    glPushMatrix();

    glTranslatef(0.17f, 0.09f, 0.1f);

    setChickenColor(isShadow, 0.1f, 0.1f, 0.1f);
    drawBox(0.08f, 0.08f, 0.05f);

    glPopMatrix();

    glPopMatrix();
}

static void drawChickenBody(int isShadow) {
    setChickenColor(isShadow, 0.95f, 0.9f, 0.6f);
    drawBox(0.7f, 0.6f, 0.9f);
}

static void drawChickenComb(float bob, int isShadow) {
    glPushMatrix();

    glTranslatef(0, 0.85f + bob, 0.3f);

    setChickenColor(isShadow, 1.0f, 0.1f, 0.1f);
    drawBox(0.12f, 0.2f, 0.12f);

    glPopMatrix();
}

static void drawChickenWing(float side, int isShadow) {
    glPushMatrix();

    glTranslatef(side * 0.42f, 0.05f, 0.0f);
    glRotatef(side * 20.0f, 0, 0, 1);

    setChickenColor(isShadow, 0.85f, 0.75f, 0.4f);
    drawBox(0.15f, 0.45f, 0.7f);

    glPopMatrix();
}

static void drawChickenLeg(float side, int isShadow) {
    glPushMatrix();

    glTranslatef(side * 0.15f, -0.38f, 0.0f);

    setChickenColor(isShadow, 1.0f, 0.55f, 0.0f);
    drawBox(0.1f, 0.25f, 0.1f);

    glPopMatrix();
}

static void drawChickenTail(int isShadow) {
    glPushMatrix();

    glTranslatef(0, 0.2f, -0.52f);
    glRotatef(35, 1, 0, 0);

    setChickenColor(isShadow, 0.7f, 0.6f, 0.3f);
    drawBox(0.5f, 0.35f, 0.25f);

    glPopMatrix();
}

void drawChicken(float bobAngle, int isShadow) {
    float bob = (float)sin(bobAngle) * 0.08f;

    glPushMatrix();

    drawChickenBody(isShadow);

    drawChickenHead(bob, isShadow);

    drawChickenComb(bob, isShadow);

    drawChickenWing(-1.0f, isShadow);
    drawChickenWing( 1.0f, isShadow);

    drawChickenLeg(-1.0f, isShadow);
    drawChickenLeg( 1.0f, isShadow);

    drawChickenTail(isShadow);

    glPopMatrix();
}

void renderChickenWithShadow(
    float* lightPosGL, 
    float* planePoint, 
    float* planeNormal, 
    float bobAngle,
    float posX,  // <-- Tambah parameter posisi X ayam
    float posZ   // <-- Tambah parameter posisi Z ayam
) {
    float shadowMat[16];

    // 1. Hitung matriks proyeksi berdasarkan posisi lampu dan lantai
    buildShadowMatrix(shadowMat, lightPosGL, planePoint, planeNormal);

    // =================================================================
    // 1. RENDER SHADOW AYAM (Transparan & Anti-Overlapping)
    // =================================================================
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
        glTranslatef(posX, 0.5f, posZ);

        drawChicken(bobAngle, 1);
    glPopMatrix();

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopAttrib();

    // =================================================================
    // 2. RENDER AYAM ASLI
    // =================================================================
    glEnable(GL_LIGHTING);

    glPushMatrix();
        // Geser ayam asli ke koordinat dunianya
        glTranslatef(posX, 0.5f, posZ);
        drawChicken(bobAngle, 0);
    glPopMatrix();
}