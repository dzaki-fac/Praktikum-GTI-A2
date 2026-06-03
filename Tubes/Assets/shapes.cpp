#ifdef _WIN32
#include <windows.h>
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

#include "shapes.h"
#include "math.h"

void drawBox(float w, float h, float d) {

    float hw = w / 2.0f;
    float hh = h / 2.0f;
    float hd = d / 2.0f;

    glBegin(GL_QUADS);

    /* depan */
    glNormal3f(0,0,1);
    glVertex3f(-hw,-hh, hd);
    glVertex3f( hw,-hh, hd);
    glVertex3f( hw, hh, hd);
    glVertex3f(-hw, hh, hd);

    /* belakang */
    glNormal3f(0,0,-1);
    glVertex3f(-hw,-hh,-hd);
    glVertex3f(-hw, hh,-hd);
    glVertex3f( hw, hh,-hd);
    glVertex3f( hw,-hh,-hd);

    /* kiri */
    glNormal3f(-1,0,0);
    glVertex3f(-hw,-hh,-hd);
    glVertex3f(-hw,-hh, hd);
    glVertex3f(-hw, hh, hd);
    glVertex3f(-hw, hh,-hd);

    /* kanan */
    glNormal3f(1,0,0);
    glVertex3f(hw,-hh,-hd);
    glVertex3f(hw, hh,-hd);
    glVertex3f(hw, hh, hd);
    glVertex3f(hw,-hh, hd);

    /* atas */
    glNormal3f(0,1,0);
    glVertex3f(-hw,hh,-hd);
    glVertex3f( hw,hh,-hd);
    glVertex3f( hw,hh, hd);
    glVertex3f(-hw,hh, hd);

    /* bawah */
    glNormal3f(0,-1,0);
    glVertex3f(-hw,-hh,-hd);
    glVertex3f(-hw,-hh, hd);
    glVertex3f( hw,-hh, hd);
    glVertex3f( hw,-hh,-hd);

    glEnd();
}

void drawBoxTextured(float w, float h, float d,
                     float uTile, float vTile) {

    float hw = w/2.0f;
    float hh = h/2.0f;
    float hd = d/2.0f;

    glBegin(GL_QUADS);

    /* Atas */
    glNormal3f(0,1,0);
    glTexCoord2f(0,0);             glVertex3f(-hw, hh,-hd);
    glTexCoord2f(uTile,0);         glVertex3f( hw, hh,-hd);
    glTexCoord2f(uTile,vTile);     glVertex3f( hw, hh, hd);
    glTexCoord2f(0,vTile);         glVertex3f(-hw, hh, hd);

    /* Bawah */
    glNormal3f(0,-1,0);
    glTexCoord2f(0,0);             glVertex3f(-hw,-hh,-hd);
    glTexCoord2f(0,vTile);         glVertex3f(-hw,-hh, hd);
    glTexCoord2f(uTile,vTile);     glVertex3f( hw,-hh, hd);
    glTexCoord2f(uTile,0);         glVertex3f( hw,-hh,-hd);

    /* Depan */
    glNormal3f(0,0,1);
    glTexCoord2f(0,0);             glVertex3f(-hw,-hh, hd);
    glTexCoord2f(uTile,0);         glVertex3f( hw,-hh, hd);
    glTexCoord2f(uTile,vTile);     glVertex3f( hw, hh, hd);
    glTexCoord2f(0,vTile);         glVertex3f(-hw, hh, hd);

    /* Belakang */
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);             glVertex3f(-hw,-hh,-hd);
    glTexCoord2f(0,vTile);         glVertex3f(-hw, hh,-hd);
    glTexCoord2f(uTile,vTile);     glVertex3f( hw, hh,-hd);
    glTexCoord2f(uTile,0);         glVertex3f( hw,-hh,-hd);

    /* Kiri */
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);             glVertex3f(-hw,-hh,-hd);
    glTexCoord2f(uTile,0);         glVertex3f(-hw,-hh, hd);
    glTexCoord2f(uTile,vTile);     glVertex3f(-hw, hh, hd);
    glTexCoord2f(0,vTile);         glVertex3f(-hw, hh,-hd);

    /* Kanan */
    glNormal3f(1,0,0);
    glTexCoord2f(0,0);             glVertex3f( hw,-hh,-hd);
    glTexCoord2f(0,vTile);         glVertex3f( hw, hh,-hd);
    glTexCoord2f(uTile,vTile);     glVertex3f( hw, hh, hd);
    glTexCoord2f(uTile,0);         glVertex3f( hw,-hh, hd);

    glEnd();
}

void drawShadow(float radiusX, float radiusZ) {

    int i;
    int segments = 40;

    glDisable(GL_LIGHTING);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.0f, 0.0f, 0.35f);

    glBegin(GL_TRIANGLE_FAN);

    glVertex3f(0.0f, 0.01f, 0.0f);

    for (i = 0; i <= segments; i++) {

        float angle =
            2.0f * 3.1415926f * i / segments;

        float x = cos(angle) * radiusX;
        float z = sin(angle) * radiusZ;

        glVertex3f(x, 0.01f, z);
    }

    glEnd();

    glDisable(GL_BLEND);

    glEnable(GL_LIGHTING);
}

// Di shapes.h — tambahkan setelah include guards
static void glShadowProjection(float *l, float *e, float *n)
{
    float d, c;
    float mat[16];

    d = n[0]*l[0] + n[1]*l[1] + n[2]*l[2];
    c = e[0]*n[0] + e[1]*n[1] + e[2]*n[2] - d;

    mat[0]  = l[0]*n[0]+c;  mat[4]  = n[1]*l[0];
    mat[8]  = n[2]*l[0];    mat[12] = -l[0]*c-l[0]*d;

    mat[1]  = n[0]*l[1];    mat[5]  = l[1]*n[1]+c;
    mat[9]  = n[2]*l[1];    mat[13] = -l[1]*c-l[1]*d;

    mat[2]  = n[0]*l[2];    mat[6]  = n[1]*l[2];
    mat[10] = l[2]*n[2]+c;  mat[14] = -l[2]*c-l[2]*d;

    mat[3]  = n[0];         mat[7]  = n[1];
    mat[11] = n[2];         mat[15] = -d;

    glMultMatrixf(mat);
}