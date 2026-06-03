#include "tree.h"
#include "shapes.h"

void drawTree(float px, float pz, GLuint texBark, GLuint texLeaf, int isShadow) {
    glPushMatrix();
    glTranslatef(px, 0.0f, pz);

    /* ================= BATANG ================= */
    glPushMatrix();
    glTranslatef(0.0f, 0.6f, 0.0f);   // supaya alas batang pas di tanah

    if (isShadow) {
        glDisable(GL_TEXTURE_2D);
        glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
        drawBox(0.2f, 1.2f, 0.2f);
    } else {
        if (texBark != 0) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texBark);
            glColor3f(1.0f, 1.0f, 1.0f);
        } else {
            glDisable(GL_TEXTURE_2D);
            glColor3f(0.45f, 0.28f, 0.10f);
        }

        drawBoxTextured(0.2f, 1.2f, 0.2f, 1.0f, 3.0f);
    }
    glPopMatrix();

    /* ================= DAUN ================= */
    glPushMatrix();
    glTranslatef(0.0f, 1.7f, 0.0f);

    if (isShadow) {
        glDisable(GL_TEXTURE_2D);
        glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
        drawBox(1.0f, 1.4f, 1.0f);
    } else {
        if (texLeaf != 0) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texLeaf);
            glColor3f(1.0f, 1.0f, 1.0f);
        } else {
            glDisable(GL_TEXTURE_2D);
            glColor3f(0.15f, 0.65f, 0.15f);
        }

        drawBoxTextured(1.0f, 1.4f, 1.0f, 1.0f, 1.0f);
    }
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}