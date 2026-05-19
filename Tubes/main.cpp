// g++ main.cpp -o main -lfreeglut -lopengl32 -lglu32; .\main.exe

#ifdef _WIN32
  #include <windows.h>
  #include <GL/freeglut.h>
#else
  #include <GL/glut.h>
#endif


#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "shapes.h"
#include "chicken.h"
#include "car.h"

/* -- Konstanta --------------------------------------------------- */
#define WINDOW_W      900
#define WINDOW_H      700
#define LANE_WIDTH    2.0f
#define NUM_LANES     8
#define MAX_CARS      30
#define CAR_SPEED_BASE 0.03f
#define PI 3.14159265358979f



typedef struct {
    float x, z;
    float bobAngle;
    int   alive;
} Chicken;

/* -- State Global ------------------------------------------------ */
static Chicken chicken;
static Car     cars[MAX_CARS];
static int     numCars = 0;

static int   gameOver = 0;
static int   gameWon  = 0;
static int   score    = 0;
static float ROAD_LEN;

/* -- Texture IDs ------------------------------------------------- */
static GLuint texLeaf = 0;
static GLuint texBark = 0;
static GLuint texGrass = 0;

/* ================================================================
   BMP Loader sederhana (24-bit uncompressed BMP)
   ================================================================ */
static GLuint loadBMP(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "[WARN] Tidak bisa buka %s, pohon tanpa tekstur.\n", filename);
        return 0;
    }

    unsigned char header[54];
    if (fread(header, 1, 54, f) != 54 || header[0] != 'B' || header[1] != 'M') {
        fprintf(stderr, "[WARN] %s bukan BMP valid.\n", filename);
        fclose(f);
        return 0;
    }

    /* Baca metadata dari header BMP */
    int dataOffset  = *(int*)&header[10];
    int width       = *(int*)&header[18];
    int height      = *(int*)&header[22];
    short bpp       = *(short*)&header[28];

    if (bpp != 24) {
        fprintf(stderr, "[WARN] %s harus 24-bit BMP (sekarang %d-bit).\n", filename, bpp);
        fclose(f);
        return 0;
    }

    /* Row BMP selalu di-pad ke kelipatan 4 byte */
    int rowSize   = ((width * 3 + 3) / 4) * 4;
    int imageSize = rowSize * height;

    unsigned char *rawData = (unsigned char*)malloc(imageSize);
    if (!rawData) { fclose(f); return 0; }

    fseek(f, dataOffset, SEEK_SET);
    fread(rawData, 1, imageSize, f);
    fclose(f);

    /* BMP disimpan BGR, kita perlu RGB untuk OpenGL */
    /* Sekaligus flip vertikal (BMP bottom-up) */
    unsigned char *rgbData = (unsigned char*)malloc(width * height * 3);
    if (!rgbData) { free(rawData); return 0; }

    int row, col;
    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            int srcIdx = (height - 1 - row) * rowSize + col * 3; /* flip vertikal */
            int dstIdx = row * width * 3 + col * 3;
            rgbData[dstIdx + 0] = rawData[srcIdx + 2]; /* R */
            rgbData[dstIdx + 1] = rawData[srcIdx + 1]; /* G */
            rgbData[dstIdx + 2] = rawData[srcIdx + 0]; /* B */
        }
    }
    free(rawData);

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB,
                      GL_UNSIGNED_BYTE, rgbData);
    free(rgbData);

    printf("[INFO] Loaded %s (%dx%d) -> texID=%u\n", filename, width, height, texID);
    return texID;
}


/* ================================================================
   drawBoxTextured — kotak dengan UV mapping per sisi
   ================================================================ */
static void drawBoxTextured(float w, float h, float d,
                            float uTile, float vTile) {
    float hw = w/2.0f, hh = h/2.0f, hd = d/2.0f;

    glBegin(GL_QUADS);
        /* Atas */
        glNormal3f(0,1,0);
        glTexCoord2f(0,       0      ); glVertex3f(-hw, hh,-hd);
        glTexCoord2f(uTile,   0      ); glVertex3f( hw, hh,-hd);
        glTexCoord2f(uTile,   vTile  ); glVertex3f( hw, hh, hd);
        glTexCoord2f(0,       vTile  ); glVertex3f(-hw, hh, hd);
        /* Bawah */
        glNormal3f(0,-1,0);
        glTexCoord2f(0,       0      ); glVertex3f(-hw,-hh,-hd);
        glTexCoord2f(0,       vTile  ); glVertex3f(-hw,-hh, hd);
        glTexCoord2f(uTile,   vTile  ); glVertex3f( hw,-hh, hd);
        glTexCoord2f(uTile,   0      ); glVertex3f( hw,-hh,-hd);
        /* Depan */
        glNormal3f(0,0,1);
        glTexCoord2f(0,       0      ); glVertex3f(-hw,-hh, hd);
        glTexCoord2f(uTile,   0      ); glVertex3f( hw,-hh, hd);
        glTexCoord2f(uTile,   vTile  ); glVertex3f( hw, hh, hd);
        glTexCoord2f(0,       vTile  ); glVertex3f(-hw, hh, hd);
        /* Belakang */
        glNormal3f(0,0,-1);
        glTexCoord2f(0,       0      ); glVertex3f(-hw,-hh,-hd);
        glTexCoord2f(0,       vTile  ); glVertex3f(-hw, hh,-hd);
        glTexCoord2f(uTile,   vTile  ); glVertex3f( hw, hh,-hd);
        glTexCoord2f(uTile,   0      ); glVertex3f( hw,-hh,-hd);
        /* Kiri */
        glNormal3f(-1,0,0);
        glTexCoord2f(0,       0      ); glVertex3f(-hw,-hh,-hd);
        glTexCoord2f(uTile,   0      ); glVertex3f(-hw,-hh, hd);
        glTexCoord2f(uTile,   vTile  ); glVertex3f(-hw, hh, hd);
        glTexCoord2f(0,       vTile  ); glVertex3f(-hw, hh,-hd);
        /* Kanan */
        glNormal3f(1,0,0);
        glTexCoord2f(0,       0      ); glVertex3f( hw,-hh,-hd);
        glTexCoord2f(0,       vTile  ); glVertex3f( hw, hh,-hd);
        glTexCoord2f(uTile,   vTile  ); glVertex3f( hw, hh, hd);
        glTexCoord2f(uTile,   0      ); glVertex3f( hw,-hh, hd);
    glEnd();
}

/* ================================================================
   drawTree — batang pakai bark.bmp, kanopi pakai leaf.bmp
   ================================================================ */
static void drawTree(float px, float pz) {
    glPushMatrix();
    glTranslatef(px, 0, pz);

    /* --- BATANG (bark texture) --- */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texBark);
    glColor3f(1.0f, 1.0f, 1.0f); /* warna putih agar tekstur tampil asli */

    if (texBark == 0) {
        /* Fallback warna jika tekstur tidak berhasil load */
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.4f, 0.25f, 0.1f);
    }

    drawBoxTextured(0.2f, 1.2f, 0.2f, 1.0f, 3.0f); /* uTile=1, vTile=3 (repeat vertikal) */

    /* --- KANOPI (leaf texture) --- */
    glTranslatef(0, 1.0f, 0);

    if (texLeaf != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texLeaf);
        glColor3f(1.0f, 1.0f, 1.0f);
    } else {
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.15f, 0.55f, 0.1f);
    }

    drawBoxTextured(1.0f, 1.4f, 1.0f, 1.0f, 1.0f);

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawOverlay(float alpha) {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_W, 0, WINDOW_H);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor4f(0.0f, 0.0f, 0.0f, alpha);

    glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(WINDOW_W, 0);
        glVertex2f(WINDOW_W, WINDOW_H);
        glVertex2f(0, WINDOW_H);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

/* -- Spawn Mobil ------------------------------------------------- */
static float frandRange(float lo, float hi) {
    return lo + (float)rand() / RAND_MAX * (hi - lo);
}

static void spawnCars(void) {
    int lane, i;
    numCars = 0;
    for (lane = 1; lane <= 6 && numCars < MAX_CARS; lane++) {
        float laneZ = -ROAD_LEN / 2.0f + lane * LANE_WIDTH + LANE_WIDTH / 2.0f;
        int   n     = 2 + rand() % 3;
        float dir   = (lane % 2 == 0) ? 1.0f : -1.0f;
        float spd   = CAR_SPEED_BASE * (0.8f + (rand() % 60) / 100.0f)
                      * (1.0f + score * 0.05f);
        for (i = 0; i < n && numCars < MAX_CARS; i++) {
            Car *c = &cars[numCars++];
            c->z     = laneZ;
            c->x     = -8.0f + i * (16.0f / n) + frandRange(0, 2.0f);
            c->speed = spd * dir;
            c->r     = 0.3f + frandRange(0, 0.7f);
            c->g     = 0.3f + frandRange(0, 0.7f);
            c->b     = 0.3f + frandRange(0, 0.7f);
            c->panjang = 1.2f + (rand() % 4) / 10.0f;
        }
    }
}

static void resetGame(void) {
    ROAD_LEN       = NUM_LANES * LANE_WIDTH;
    chicken.x      = 0.0f;
    chicken.z      = -ROAD_LEN / 2.0f + LANE_WIDTH / 2.0f;
    chicken.bobAngle = 0.0f;
    chicken.alive  = 1;
    gameOver       = 0;
    gameWon        = 0;
    spawnCars();
}


static void drawGroundTextured(float x1, float z1,
                               float x2, float z2,
                               float tileU, float tileV) {

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

    glTexCoord2f(0,      0);      glVertex3f(x1, -0.01f, z1);
    glTexCoord2f(tileU,  0);      glVertex3f(x2, -0.01f, z1);
    glTexCoord2f(tileU,  tileV);  glVertex3f(x2, -0.01f, z2);
    glTexCoord2f(0,      tileV);  glVertex3f(x1, -0.01f, z2);

    glEnd();

    glDisable(GL_TEXTURE_2D);
}

/* -- Gambar Dunia ------------------------------------------------ */
static void drawWorld(void) {
    int i;
    float startZ = -ROAD_LEN / 2.0f;
    float finZ   =  ROAD_LEN / 2.0f - LANE_WIDTH / 2.0f;

    /* Tanah luar */
    drawGroundTextured(
    -50, -20,
     50,  20,
     20.0f, 20.0f
    );  

    /* Lane per lane */
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
        glVertex3f(-30,0,z0); glVertex3f(30,0,z0);
        glVertex3f( 30,0,z1); glVertex3f(-30,0,z1);
        glEnd();

        /* Marka tengah */
        if (i > 0 && i < NUM_LANES - 1) {
            glColor3f(0.9f, 0.9f, 0.9f);
            glBegin(GL_QUADS);
            glVertex3f(-30,0.01f,zc-0.03f); glVertex3f(30,0.01f,zc-0.03f);
            glVertex3f( 30,0.01f,zc+0.03f); glVertex3f(-30,0.01f,zc+0.03f);
            glEnd();

            glColor3f(0.95f, 0.85f, 0.0f);
            for (xi = -9.0f; xi < 9.0f; xi += 2.5f) {
                glBegin(GL_QUADS);
                glVertex3f(xi,       0.01f, zc-0.05f);
                glVertex3f(xi+1.2f,  0.01f, zc-0.05f);
                glVertex3f(xi+1.2f,  0.01f, zc+0.05f);
                glVertex3f(xi,       0.01f, zc+0.05f);
                glEnd();
            }
        }
    }

    /* Papan FINISH */
    glPushMatrix();
    glTranslatef(-9.5f, 1.5f, finZ);
    glColor3f(0.0f, 0.8f, 0.1f);
    drawBox(0.3f, 2.5f, 0.3f);
    glTranslatef(0, 1.4f, 0);
    drawBox(2.0f, 0.7f, 0.2f);
    glPopMatrix();

    /* Papan START */
    glPushMatrix();
    glTranslatef(-9.5f, 1.5f, startZ + LANE_WIDTH / 2.0f);
    glColor3f(0.9f, 0.5f, 0.0f);
    drawBox(0.3f, 2.5f, 0.3f);
    glTranslatef(0, 1.4f, 0);
    drawBox(2.0f, 0.7f, 0.2f);
    glPopMatrix();

    /* =========================================================
       POHON — sekarang pakai tekstur bark.bmp & leaf.bmp
       ========================================================= */
    {
        float treeX[] = {-10.5f,-11.2f, 10.5f,11.3f,-10.8f,10.7f};
        float treeZ[] = { -6.0f,  0.0f, -4.0f, 3.0f,  5.0f,-1.0f};
        int t;
        for (t = 0; t < 6; t++) {
            drawTree(treeX[t], treeZ[t]);
        }
    }
}

/* -- Teks 2D ------------------------------------------------------ */
static void drawText2D(float x, float y, const char *text,
                       float r, float g, float b) {
    const char *p;

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_W, 0, WINDOW_H);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(r, g, b);
    glRasterPos2f(x, y);

    for (p = text; *p; p++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);

    glPopAttrib();
}

/* -- Deteksi Tabrakan --------------------------------------------- */
static int checkCollision(void) {
    int i;
    for (i = 0; i < numCars; i++) {
        float dx = (float)fabs(chicken.x - cars[i].x);
        float dz = (float)fabs(chicken.z - cars[i].z);
        if (dx < (cars[i].width / 2.0f + 0.35f) && dz < 0.75f)
            return 1;
    }
    return 0;
}

/* -- Callback Display --------------------------------------------- */
void display(void) {
    char buf[64];
    float eyeX, eyeY, eyeZ;
    int i;
    GLfloat lightPos[] = {5.0f, 10.0f, 5.0f, 1.0f};

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    eyeX = chicken.x;
    eyeY = 4.0f;
    eyeZ = chicken.z - 7.0f;

    gluLookAt(
        eyeX, eyeY, eyeZ,
        chicken.x, 0.0f, chicken.z + 8.0f,
        0, 1, 0
    );

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    drawWorld();

    for (i = 0; i < numCars; i++)
        drawCar(&cars[i]);

    if (chicken.alive) {
        glPushMatrix();
        glTranslatef(chicken.x, 0.5f, chicken.z);
        drawChicken(chicken.bobAngle);
        glPopMatrix();
    }

    /* HUD */
    sprintf(buf, "Score: %d", score);
    drawText2D(20, WINDOW_H - 30, buf, 1.0f, 1.0f, 0.0f);
    drawText2D(20, WINDOW_H - 55,
               "W/S/A/D = Gerak  |  R = Restart  |  ESC = Keluar",
               0.8f, 0.8f, 0.8f);

    if (gameOver) {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        drawOverlay(0.5f);

        drawText2D(WINDOW_W/2 - 120, WINDOW_H/2 + 30,
                   "GAME OVER!", 1.0f, 0.0f, 0.0f);
        drawText2D(WINDOW_W/2 - 110, WINDOW_H/2,
                   "Ayammu ketabrak!", 1.0f, 0.6f, 0.6f);
        drawText2D(WINDOW_W/2 - 110, WINDOW_H/2 - 35,
                   "Tekan R untuk mulai lagi", 0.8f, 0.8f, 0.8f);

        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);

    } else if (gameWon) {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        drawOverlay(0.5f);

        drawText2D(WINDOW_W/2 - 140, WINDOW_H/2 + 30,
                   "SELAMAT! AYAM BERHASIL!", 0.2f, 1.0f, 0.3f);
        sprintf(buf, "Score: %d  |  Tekan R untuk lanjut", score);
        drawText2D(WINDOW_W/2 - 150, WINDOW_H/2 - 10,
                   buf, 1.0f, 1.0f, 0.5f);

        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);
    }

    glutSwapBuffers();
}

/* -- Timer / Update ----------------------------------------------- */
void update(int val) {
    int i;
    (void)val;
    glutTimerFunc(16, update, 0);

    if (!gameOver && !gameWon) {
        chicken.bobAngle += 0.15f;

        for (i = 0; i < numCars; i++) {
            cars[i].x += cars[i].speed;
            if (cars[i].x >  11.0f) cars[i].x = -11.0f;
            if (cars[i].x < -11.0f) cars[i].x =  11.0f;
        }

        if (checkCollision()) {
            chicken.alive = 0;
            gameOver      = 1;
        }

        if (chicken.z >= ROAD_LEN / 2.0f - LANE_WIDTH / 2.0f - 0.3f) {
            score = score + 500;
            gameWon = 1;
        }
    }

    glutPostRedisplay();
}

/* -- Keyboard ----------------------------------------------------- */
void keyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;
    if (key == 27) exit(0);

    if (key == 'r' || key == 'R') {
        if (gameOver) score = 0;
        resetGame();
        return;
    }

    if (gameOver || gameWon) return;

    switch (key) {
        case 'w': case 'W': chicken.z += LANE_WIDTH; break;
        case 's': case 'S': chicken.z -= LANE_WIDTH; break;
        case 'a': case 'A': chicken.x += 0.5f;       break;
        case 'd': case 'D': chicken.x -= 0.5f;       break;
    }

    if (chicken.x < -9.5f) chicken.x = -9.5f;
    if (chicken.x >  9.5f) chicken.x =  9.5f;
    if (chicken.z < -ROAD_LEN / 2.0f + 0.1f)
        chicken.z = -ROAD_LEN / 2.0f + 0.1f;
}

void specialKey(int key, int x, int y) {
    (void)x; (void)y;
    if (gameOver || gameWon) return;

    switch (key) {
        case GLUT_KEY_UP:    chicken.z += LANE_WIDTH; break;
        case GLUT_KEY_DOWN:  chicken.z -= LANE_WIDTH; break;
        case GLUT_KEY_LEFT:  chicken.x -= 0.5f;       break;
        case GLUT_KEY_RIGHT: chicken.x += 0.5f;       break;
    }

    if (chicken.x < -9.5f) chicken.x = -9.5f;
    if (chicken.x >  9.5f) chicken.x =  9.5f;
}

/* -- Reshape ------------------------------------------------------ */
void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, (double)w / h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

/* -- Main --------------------------------------------------------- */
int main(int argc, char **argv) {
    GLfloat ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat diffuse[] = {0.9f, 0.9f, 0.8f, 1.0f};

    srand((unsigned)time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_W, WINDOW_H);
    glutInitWindowPosition(100, 80);
    glutCreateWindow("Chicken Cross the Road 3D");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glClearColor(0.52f, 0.80f, 0.98f, 1.0f);

    /* Load tekstur — file harus ada di direktori yang sama dengan .exe */
    texBark = loadBMP("bark.bmp");
    texLeaf = loadBMP("leaf.bmp");
    texGrass = loadBMP("grass.bmp");

    resetGame();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}