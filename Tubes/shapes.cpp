#ifdef _WIN32
  #include <windows.h>
  #include <GL/freeglut.h>
#else
  #include <GL/glut.h>
#endif

#include "shapes.h"

void drawBox(float w, float h, float d) {
    float hw = w/2.0f, hh = h/2.0f, hd = d/2.0f;

    glBegin(GL_QUADS);

        /* Atas */
        glNormal3f(0,1,0);
        glVertex3f(-hw, hh,-hd); glVertex3f( hw, hh,-hd);
        glVertex3f( hw, hh, hd); glVertex3f(-hw, hh, hd);

        /* Bawah */
        glNormal3f(0,-1,0);
        glVertex3f(-hw,-hh,-hd); glVertex3f(-hw,-hh, hd);
        glVertex3f( hw,-hh, hd); glVertex3f( hw,-hh,-hd);

        /* Depan */
        glNormal3f(0,0,1);
        glVertex3f(-hw,-hh, hd); glVertex3f( hw,-hh, hd);
        glVertex3f( hw, hh, hd); glVertex3f(-hw, hh, hd);

        /* Belakang */
        glNormal3f(0,0,-1);
        glVertex3f(-hw,-hh,-hd); glVertex3f(-hw, hh,-hd);
        glVertex3f( hw, hh,-hd); glVertex3f( hw,-hh,-hd);

        /* Kiri */
        glNormal3f(-1,0,0);
        glVertex3f(-hw,-hh,-hd); glVertex3f(-hw,-hh, hd);
        glVertex3f(-hw, hh, hd); glVertex3f(-hw, hh,-hd);

        /* Kanan */
        glNormal3f(1,0,0);
        glVertex3f( hw,-hh,-hd); glVertex3f( hw, hh,-hd);
        glVertex3f( hw, hh, hd); glVertex3f( hw,-hh, hd);

    glEnd();
}