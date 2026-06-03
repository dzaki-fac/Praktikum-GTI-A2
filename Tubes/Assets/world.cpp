#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/freeglut.h>
#include <math.h>

#include "world.h"
#include "game.h"
#include "shapes.h"
#include "tree.h"
#include "shadow.h"
#include "constants.h"

/* ── Skybox ─────────────────────────────────────────────────────── */
/*
 * Kubus besar mengelilingi scene. Depth test dimatikan saat render
 * agar selalu jadi background, lalu dinyalakan lagi.
 * Warna tiap sisi dibuat berbeda agar terlihat seperti langit bertahap.
 *   atas  : biru tua   (zenith)
 *   bawah : tidak digambar (tertutup tanah)
 *   sisi  : biru muda ke putih kekuningan (horizon)
 */
static void drawSkybox(float cx, float cy, float cz, float size) {
    float h = size / 2.0f;

    /* Warna horizon (bawah sisi) */
    float hr = 0.85f, hg = 0.92f, hb = 1.0f;
    /* Warna zenith (atas sisi & face atas) */
    float zr = 0.25f, zg = 0.52f, zb = 0.95f;

    glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);

    glPushMatrix();
    glTranslatef(cx, cy, cz);

    glBegin(GL_QUADS);

    /* Sisi depan (Z+) */
    glColor3f(hr, hg, hb); glVertex3f(-h, -h,  h);
    glColor3f(hr, hg, hb); glVertex3f( h, -h,  h);
    glColor3f(zr, zg, zb); glVertex3f( h,  h,  h);
    glColor3f(zr, zg, zb); glVertex3f(-h,  h,  h);

    /* Sisi belakang (Z-) */
    glColor3f(hr, hg, hb); glVertex3f( h, -h, -h);
    glColor3f(hr, hg, hb); glVertex3f(-h, -h, -h);
    glColor3f(zr, zg, zb); glVertex3f(-h,  h, -h);
    glColor3f(zr, zg, zb); glVertex3f( h,  h, -h);

    /* Sisi kiri (X-) */
    glColor3f(hr, hg, hb); glVertex3f(-h, -h, -h);
    glColor3f(hr, hg, hb); glVertex3f(-h, -h,  h);
    glColor3f(zr, zg, zb); glVertex3f(-h,  h,  h);
    glColor3f(zr, zg, zb); glVertex3f(-h,  h, -h);

    /* Sisi kanan (X+) */
    glColor3f(hr, hg, hb); glVertex3f( h, -h,  h);
    glColor3f(hr, hg, hb); glVertex3f( h, -h, -h);
    glColor3f(zr, zg, zb); glVertex3f( h,  h, -h);
    glColor3f(zr, zg, zb); glVertex3f( h,  h,  h);

    /* Face atas (zenith penuh) */
    glColor3f(zr, zg, zb);
    glVertex3f(-h,  h,  h);
    glVertex3f( h,  h,  h);
    glVertex3f( h,  h, -h);
    glVertex3f(-h,  h, -h);

    glEnd();

    glPopMatrix();
    glPopAttrib();
}

/* ── Pegunungan ─────────────────────────────────────────────────── */
/*
 * Deretan "puncak" segitiga di kiri, kanan, dan jauh depan/belakang.
 * Digambar sebelum lane agar tertutup fog dan geometry jalan.
 */
static void drawMountainRange(float baseZ, float side,
                              float r, float g, float b,
                              int count, float spacing,
                              float baseW, float peakH,
                              GLuint tex)
{
    int i;
    glDisable(GL_LIGHTING);

    int useTexture = (tex != 0);
    if (useTexture) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex);
        glColor3f(1.0f, 1.0f, 1.0f);
    } else {
        glDisable(GL_TEXTURE_2D);
    }

    for (i = 0; i < count; i++) {
        float z  = baseZ + i * spacing;
        float bw = baseW * (0.7f + (float)(i % 3) * 0.2f);
        float ph = peakH * (0.8f + (float)(i % 4) * 0.15f);

        /* Warna variasi — hanya dipakai jika tidak ada tekstur */
        float dr = r * (0.85f + (float)(i % 5) * 0.03f);
        float dg = g * (0.85f + (float)(i % 5) * 0.03f);
        float db = b * (0.85f + (float)(i % 5) * 0.03f);

        /* Muka utama (terang) */
        if (!useTexture) glColor3f(dr, dg, db);
        glBegin(GL_TRIANGLES);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(side - bw, 0.0f, z);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(side + bw, 0.0f, z);
        glTexCoord2f(0.5f, 1.0f); glVertex3f(side,      ph,   z);
        glEnd();

        /* Sisi gelap (kedalaman) */
        if (!useTexture) glColor3f(dr * 0.6f, dg * 0.6f, db * 0.6f);
        else             glColor3f(0.6f, 0.6f, 0.6f); /* tint gelap di atas tekstur */
        glBegin(GL_TRIANGLES);
        glTexCoord2f(0.5f, 1.0f); glVertex3f(side,      ph,                  z);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(side + bw, 0.0f,               z);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(side + bw, 0.0f, z + spacing * 0.3f);
        glEnd();
        if (useTexture) glColor3f(1.0f, 1.0f, 1.0f); /* reset tint */
    }

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

/* ── Tanah bertekstur ───────────────────────────────────────────── */
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
    glNormal3f(0, 1, 0);
    glTexCoord2f(0,      0);      glVertex3f(x1, -0.01f, z1);
    glTexCoord2f(tileU,  0);      glVertex3f(x2, -0.01f, z1);
    glTexCoord2f(tileU,  tileV);  glVertex3f(x2, -0.01f, z2);
    glTexCoord2f(0,      tileV);  glVertex3f(x1, -0.01f, z2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

/* ── Tiang gerbang ──────────────────────────────────────────────── */
static void drawGate(float posX, float posY, float posZ,
                     float r, float g, float b,
                     int isShadow)
{
    glPushMatrix();
    glTranslatef(posX, posY, posZ);

    if (isShadow) glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    else          glColor3f(r, g, b);

    drawBox(0.3f, 2.5f, 0.3f);
    glTranslatef(0, 1.4f, 0);
    drawBox(2.0f, 0.7f, 0.2f);

    glPopMatrix();
}

static void renderGateWithShadow(float posX, float posY, float posZ,
                                 float r, float g, float b,
                                 float *shadowMat)
{
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT |
                 GL_LIGHTING_BIT | GL_STENCIL_BUFFER_BIT);

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

    glEnable(GL_LIGHTING);
    drawGate(posX, posY, posZ, r, g, b, 0);
}

static void renderTreeWithShadow(float tx, float tz,
                                 GLuint texBark, GLuint texLeaf,
                                 float *shadowMat)
{
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT |
                 GL_LIGHTING_BIT | GL_STENCIL_BUFFER_BIT);

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
    drawTree(tx, tz, 0, 0, 1);
    glPopMatrix();

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopAttrib();

    glEnable(GL_LIGHTING);
    drawTree(tx, tz, texBark, texLeaf, 0);
}

/* ── Render satu lane ───────────────────────────────────────────── */
static void drawLane(const Lane *lane) {
    float z0 = lane->z - LANE_WIDTH / 2.0f;
    float z1 = lane->z + LANE_WIDTH / 2.0f;
    float zc = lane->z;
    float xi;

    if (lane->type == LANE_GRASS) {
        glColor3f(0.25f, 0.65f, 0.2f);
        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(-30, 0, z0); glVertex3f( 30, 0, z0);
        glVertex3f( 30, 0, z1); glVertex3f(-30, 0, z1);
        glEnd();
    } else {
        /* Aspal */
        glColor3f(0.35f, 0.35f, 0.35f);
        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(-30, 0, z0); glVertex3f( 30, 0, z0);
        glVertex3f( 30, 0, z1); glVertex3f(-30, 0, z1);
        glEnd();

        /* Marka kuning putus-putus — posisi awal random per lane */
        glColor3f(0.95f, 0.85f, 0.0f);
        for (xi = -9.0f + lane->markOffset; xi < 9.0f; xi += 2.5f) {
            glBegin(GL_QUADS);
            glVertex3f(xi,       0.01f, zc - 0.05f);
            glVertex3f(xi + 1.2f,0.01f, zc - 0.05f);
            glVertex3f(xi + 1.2f,0.01f, zc + 0.05f);
            glVertex3f(xi,       0.01f, zc + 0.05f);
            glEnd();
        }
    }
}

/* ── drawWorld ──────────────────────────────────────────────────── */
void drawWorld(World *w,
               float chickenZ,
               GLuint texGrass,
               GLuint texBark,
               GLuint texLeaf,
               GLuint texRock,
               float *lightPos)
{
    int i;

    float planePoint[3]  = {0.0f, 0.0f, chickenZ};
    float planeNormal[3] = {0.0f, 1.0f, 0.0f};
    float shadowMat[16];
    buildShadowMatrix(shadowMat, lightPos, planePoint, planeNormal);

    float zMin = chickenZ - LANES_BEHIND * LANE_WIDTH - LANE_WIDTH;
    float zMax = chickenZ + LANES_AHEAD  * LANE_WIDTH + LANE_WIDTH;

    /* ── FOG ────────────────────────────────────────────────────── */
    /* Warna fog cocok dengan horizon skybox agar blend mulus */
    GLfloat fogColor[] = { 0.85f, 0.92f, 1.0f, 1.0f };
    glFogi(GL_FOG_MODE,    GL_LINEAR);
    glFogfv(GL_FOG_COLOR,  fogColor);
    glFogf(GL_FOG_START,   12.0f);   /* mulai kabur 12 unit dari kamera */
    glFogf(GL_FOG_END,     32.0f);   /* penuh kabut di 32 unit */
    glEnable(GL_FOG);

    /* glClearColor disesuaikan warna fog agar background = horizon */
    glClearColor(fogColor[0], fogColor[1], fogColor[2], 1.0f);

    /* ── Skybox (selalu digambar duluan, depth test off) ─────────── */
    /* Skybox ikut posisi kamera (chickenX ~0, Y ~2 = posisi mata kamera) */
    drawSkybox(0.0f, 2.0f, chickenZ + 5.0f, 90.0f);

    /* ── Pegunungan kiri & kanan ─────────────────────────────────── */
    /* Snap ke grid 6.0 agar tidak "meluncur" saat kamera maju */
    float mzBase = (float)((int)(chickenZ / 6.0f)) * 6.0f;

    /* Kiri — abu-abu kebiruan */
    drawMountainRange(mzBase - 30.0f, -22.0f,
                      0.45f, 0.50f, 0.58f,
                      12, 6.0f, 5.0f, 12.0f, texRock);
    /* Kanan — sedikit lebih terang */
    drawMountainRange(mzBase - 30.0f,  22.0f,
                      0.50f, 0.54f, 0.62f,
                      12, 6.0f, 4.5f, 10.0f, texRock);

    /* ── Tanah luar ─────────────────────────────────────────────── */
    drawGroundTextured(-50, zMin, -10, zMax, 10.0f, (zMax - zMin) / 2.0f, texGrass);
    drawGroundTextured( 10, zMin,  50, zMax, 10.0f, (zMax - zMin) / 2.0f, texGrass);

    /* ── Lane-lane ──────────────────────────────────────────────── */
    for (i = 0; i < w->laneCount; i++) {
        const Lane *lane = &w->lanes[i];
        if (lane->z < zMin || lane->z > zMax) continue;
        drawLane(lane);
    }

    /* ── Pohon di samping jalan ──────────────────────────────────── */
    {
        float treeXSides[] = { -11.2f, -10.5f, 11.3f, 10.5f };
        int   tSide;
        float zBase = (float)((int)(chickenZ / 4.0f)) * 4.0f;

        for (tSide = 0; tSide < 4; tSide++) {
            int k;
            for (k = -LANES_BEHIND; k <= LANES_AHEAD; k += 2) {
                float tz = zBase + k * LANE_WIDTH;
                renderTreeWithShadow(treeXSides[tSide], tz,
                                     texBark, texLeaf, shadowMat);
            }
        }
    }

    /* ── Tanda START di lane pertama ────────────────────────────── */
    if (w->laneCount > 0) {
        float startZ = w->lanes[0].z;
        if (startZ >= zMin && startZ <= zMax) {
            renderGateWithShadow(-9.5f, 1.25f, startZ,
                                 0.9f, 0.5f, 0.0f, shadowMat);
            renderGateWithShadow( 9.5f, 1.25f, startZ,
                                 0.9f, 0.5f, 0.0f, shadowMat);
        }
    }

    glDisable(GL_FOG);
}
