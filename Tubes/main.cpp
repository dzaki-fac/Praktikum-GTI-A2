// g++ main.cpp Assets/*.cpp -o main -lfreeglut -lopengl32 -lglu32; .\main.exe

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
#include "Assets/shapes.h"
#include "Assets/chicken.h"
#include "Assets/car.h"
#include "Assets/tree.h"
#include "Assets/texture.h"
#include "Assets/world.h"
#include "Assets/ui.h"
#include "Assets/constants.h"
#include "Assets/game.h"
#include "Assets/shadow.h"

/* -- State Global ------------------------------------------------ */
static Chicken chicken;
static World   world;

static int   gameOver = 0;
static int   score    = 0;
static float maxZ     = 0.0f;   /* posisi Z terjauh yang pernah dicapai ayam */

/* -- Texture IDs ------------------------------------------------- */
static GLuint texLeaf  = 0;
static GLuint texBark  = 0;
static GLuint texGrass = 0;
static GLuint texRock  = 0;

/* -- Callback Display -------------------------------------------- */
void display(void) {
    char  buf[64];
    int   i;

    /* Cahaya selalu di depan ayam (offset +20 ke depan, tinggi 14) */
    float lightPos[4]    = {0.0f, 14.0f, chicken.z + 20.0f, 1.0f};
    float planePoint[3]  = {0.0f, 0.0f, chicken.z};
    float planeNormal[3] = {0.0f, 1.0f, 0.0f};

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();

    /* Kamera mengikuti ayam */
    gluLookAt(
        chicken.x, 4.0f, chicken.z - 7.0f,
        chicken.x, 0.0f, chicken.z + 8.0f,
        0, 1, 0
    );

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    /* Dunia dinamis */
    drawWorld(&world, chicken.z, texGrass, texBark, texLeaf, texRock, lightPos);

    /* Mobil */
    for (i = 0; i < world.numCars; i++)
        renderCarWithShadow(lightPos, planePoint, planeNormal, &world.cars[i]);

    /* Ayam */
    if (chicken.alive)
        renderChickenWithShadow(
            lightPos,
            planePoint,
            planeNormal,
            chicken.bobAngle,
            chicken.x,
            chicken.z
        );

    /* HUD */
    sprintf(buf, "Score: %d", score);
    drawText2D(20, WINDOW_H - 30, buf, 1.0f, 1.0f, 0.0f);
    drawText2D(20, WINDOW_H - 55,
               "W/S/A/D = Gerak  |  R = Restart  |  ESC = Keluar",
               0.8f, 0.8f, 0.8f);

    /* Overlay Game Over */
    if (gameOver) {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        drawOverlay(0.5f, WINDOW_W, WINDOW_H);
        drawText2D(WINDOW_W/2 - 120, WINDOW_H/2 + 30,
                   "GAME OVER!", 1.0f, 0.0f, 0.0f);
        drawText2D(WINDOW_W/2 - 110, WINDOW_H/2,
                   "Ayammu ketabrak!", 1.0f, 0.6f, 0.6f);
        sprintf(buf, "Score akhir: %d", score);
        drawText2D(WINDOW_W/2 - 110, WINDOW_H/2 - 30,
                   buf, 1.0f, 1.0f, 0.5f);
        drawText2D(WINDOW_W/2 - 110, WINDOW_H/2 - 60,
                   "Tekan R untuk mulai lagi", 0.8f, 0.8f, 0.8f);

        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);
    }

    glutSwapBuffers();
}

/* -- Timer / Update ---------------------------------------------- */
void update(int val) {
    (void)val;
    glutTimerFunc(16, update, 0);

    if (!gameOver) {
        chicken.bobAngle += 0.15f;

        /* Update posisi terjauh */
        if (chicken.z > maxZ) maxZ = chicken.z;

        updateCars(&world);

        if (checkCollision(chicken, &world)) {
            chicken.alive = 0;
            gameOver      = 1;
        }

        /* Generate lane baru + hitung score */
        updateWorld(&world, &chicken, &score);
    }

    glutPostRedisplay();
}

/* -- Keyboard ---------------------------------------------------- */
void keyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;
    if (key == 27) exit(0);

    if (key == 'r' || key == 'R') {
        resetGame(&chicken, &gameOver, &score, &world);
        maxZ = chicken.z;
        return;
    }

    if (gameOver) return;

    switch (key) {
        case 'w': case 'W': chicken.z += LANE_WIDTH; break;
        case 's': case 'S': chicken.z -= LANE_WIDTH; break;
        case 'a': case 'A': chicken.x += 0.5f;       break;
        case 'd': case 'D': chicken.x -= 0.5f;       break;
    }

    if (chicken.x < -9.5f) chicken.x = -9.5f;
    if (chicken.x >  9.5f) chicken.x =  9.5f;
    if (chicken.z < maxZ - 3.0f * LANE_WIDTH)
        chicken.z = maxZ - 3.0f * LANE_WIDTH;
}

void specialKey(int key, int x, int y) {
    (void)x; (void)y;
    if (gameOver) return;

    switch (key) {
        case GLUT_KEY_UP:    chicken.z += LANE_WIDTH; break;
        case GLUT_KEY_DOWN:  chicken.z -= LANE_WIDTH; break;
        case GLUT_KEY_LEFT:  chicken.x -= 0.5f;       break;
        case GLUT_KEY_RIGHT: chicken.x += 0.5f;       break;
    }

    if (chicken.x < -9.5f) chicken.x = -9.5f;
    if (chicken.x >  9.5f) chicken.x =  9.5f;
    if (chicken.z < maxZ - 3.0f * LANE_WIDTH)
        chicken.z = maxZ - 3.0f * LANE_WIDTH;
}
void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, (double)w / h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

/* -- Main -------------------------------------------------------- */
int main(int argc, char **argv) {
    GLfloat ambient[] = {0.50f, 0.50f, 0.50f, 1.0f};
    GLfloat diffuse[] = {0.85f, 0.85f, 0.78f, 1.0f};

    srand((unsigned)time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
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
    /* Warna background = warna horizon fog, diset sekali di sini
       (drawWorld akan update tiap frame via glClearColor juga) */
    glClearColor(0.85f, 0.92f, 1.0f, 1.0f);

    texBark  = loadBMP("texture/bark.bmp");
    texLeaf  = loadBMP("texture/leaf.bmp");
    texGrass = loadBMP("texture/grass.bmp");
    texRock  = loadBMP("texture/rock.bmp");

    resetGame(&chicken, &gameOver, &score, &world);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}
