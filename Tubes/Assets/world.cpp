#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/freeglut.h>

#include "world.h"
#include "shapes.h"
#include "tree.h"
#include "shadow.h"

#define LANE_WIDTH 2.0f
#define NUM_LANES 8

static void drawGroundTextured(float x1, float z1,
                               float x2, float z2,
                               float tileU, float tileV,
                               GLuint texGrass)
{
    if (texGrass != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texGrass);
        glColor3f(1.0f, 1.0f, 1.0f);
    } else {
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.2f, 0.55f, 0.15f);
    }

    glBegin(GL_QUADS);

    glNormal3f(0,1,0);

    glTexCoord2f(0,0);
    glVertex3f(x1, -0.01f, z1);

    glTexCoord2f(tileU,0);
    glVertex3f(x2, -0.01f, z1);

    glTexCoord2f(tileU,tileV);
    glVertex3f(x2, -0.01f, z2);

    glTexCoord2f(0,tileV);
    glVertex3f(x1, -0.01f, z2);

    glEnd();

    glDisable(GL_TEXTURE_2D);
}

/* helper internal — gambar tiang (body tiang + papan atas) */
static void drawGate(float posX, float posY, float posZ,
                     float r, float g, float b,
                     int isShadow)
{
    glPushMatrix();
    glTranslatef(posX, posY, posZ);

    if (isShadow)
        glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    else
        glColor3f(r, g, b);

    drawBox(0.3f, 2.5f, 0.3f);

    glTranslatef(0, 1.4f, 0);
    drawBox(2.0f, 0.7f, 0.2f);

    glPopMatrix();
}

static void renderGateWithShadow(float posX, float posY, float posZ,
                                 float r, float g, float b,
                                 float *shadowMat)
{
    /* Shadow */
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
    glTranslatef(0.0f, 0.01f, 0.0f);
    drawGate(posX, posY, posZ, r, g, b, 1);
    glPopMatrix();

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopAttrib();

    /* Gate asli */
    glEnable(GL_LIGHTING);
    drawGate(posX, posY, posZ, r, g, b, 0);
}

static void renderTreeWithShadow(float tx, float tz,
                                 GLuint texBark, GLuint texLeaf,
                                 float *shadowMat)
{
    /* Shadow */
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
    glTranslatef(0.0f, 0.01f, 0.0f);   // sedikit diangkat biar gak z-fighting
    drawTree(tx, tz, 0, 0, 1);
    glPopMatrix();

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopAttrib();

    /* Asli */
    glEnable(GL_LIGHTING);
    drawTree(tx, tz, texBark, texLeaf, 0);
}

void drawWorld(float roadLen,
               GLuint texGrass,
               GLuint texBark,
               GLuint texLeaf,
               float *lightPos)   // lightPos = float[4]
{
    int i, t;

    float startZ = -roadLen / 2.0f;
    float finZ   =  roadLen / 2.0f - LANE_WIDTH / 2.0f;

    float planePoint[3]  = {0.0f, 0.0f, 0.0f};
    float planeNormal[3] = {0.0f, 1.0f, 0.0f};

    float shadowMat[16];
    buildShadowMatrix(shadowMat, lightPos, planePoint, planeNormal);

    float treeX[] = { -10.5f, -11.2f,  10.5f, 11.3f, -10.8f, 10.7f };
    float treeZ[] = {  -6.0f,   0.0f,  -4.0f,  3.0f,   5.0f, -1.0f };

    /* ── Tanah luar ───────────────────────────────────────────── */
    drawGroundTextured(-50, -20, 50, 20, 20.0f, 20.0f, texGrass);

    /* ── Lane + marka (tidak berubah) ────────────────────────── */
    for (i = 0; i < NUM_LANES; i++) {
        float z0 = startZ + i * LANE_WIDTH;
        float z1 = z0 + LANE_WIDTH;
        float zc = (z0 + z1) / 2.0f;
        float xi;

        if (i == 0 || i == NUM_LANES - 1)
            glColor3f(0.25f, 0.65f, 0.2f);
        else
            glColor3f(0.35f, 0.35f, 0.35f);

        glBegin(GL_QUADS);
        glNormal3f(0,1,0);
        glVertex3f(-30, 0, z0); glVertex3f( 30, 0, z0);
        glVertex3f( 30, 0, z1); glVertex3f(-30, 0, z1);
        glEnd();

        if (i > 0 && i < NUM_LANES - 1) {
            glColor3f(0.9f, 0.9f, 0.9f);
            glBegin(GL_QUADS);
            glVertex3f(-30,0.01f,zc-0.03f); glVertex3f( 30,0.01f,zc-0.03f);
            glVertex3f( 30,0.01f,zc+0.03f); glVertex3f(-30,0.01f,zc+0.03f);
            glEnd();

            glColor3f(0.95f, 0.85f, 0.0f);
            for (xi = -9.0f; xi < 9.0f; xi += 2.5f) {
                glBegin(GL_QUADS);
                glVertex3f(xi,      0.01f,zc-0.05f); glVertex3f(xi+1.2f,0.01f,zc-0.05f);
                glVertex3f(xi+1.2f, 0.01f,zc+0.05f); glVertex3f(xi,     0.01f,zc+0.05f);
                glEnd();
            }
        }
    }

    /* ── Tiang FINISH + START (shadow + asli) ────────────────── */
    renderGateWithShadow(-9.5f, 1.25f, finZ,
                        0.0f, 0.8f, 0.1f, shadowMat);

    renderGateWithShadow(-9.5f, 1.25f, startZ + LANE_WIDTH / 2.0f,
                        0.9f, 0.5f, 0.0f, shadowMat);

    /* ── Pohon (shadow + asli) ────────────────────────────────── */
    for (t = 0; t < 6; t++) {
        renderTreeWithShadow(treeX[t], treeZ[t],
                              texBark, texLeaf, shadowMat);
    }
}