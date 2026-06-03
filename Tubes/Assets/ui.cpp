#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/freeglut.h>

#include "constants.h"
#include "ui.h"

void drawOverlay(float alpha,
                 int windowW,
                 int windowH)
{
    glDisable(GL_LIGHTING);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,
                GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);

    glPushMatrix();
    glLoadIdentity();

    gluOrtho2D(0, windowW,
               0, windowH);

    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    glLoadIdentity();

    glColor4f(0.0f, 0.0f,
              0.0f, alpha);

    glBegin(GL_QUADS);

        glVertex2f(0, 0);
        glVertex2f(windowW, 0);
        glVertex2f(windowW, windowH);
        glVertex2f(0, windowH);

    glEnd();

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_BLEND);

    glEnable(GL_LIGHTING);
}

void drawText2D(float x, float y,
                const char *text,
                float r, float g, float b)
{
    const char *p;

    glPushAttrib(GL_ENABLE_BIT |
                 GL_CURRENT_BIT);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);

    glPushMatrix();
    glLoadIdentity();

    gluOrtho2D(0, 900,
               0, 700);

    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    glLoadIdentity();

    glColor3f(r, g, b);

    glRasterPos2f(x, y);

    for (p = text; *p; p++) {
        glutBitmapCharacter(
            GLUT_BITMAP_HELVETICA_18,
            *p
        );
    }

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);

    glPopAttrib();
}