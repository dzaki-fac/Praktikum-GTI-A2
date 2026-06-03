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
#include "Assets\shapes.h"
#include "Assets\chicken.h"
#include "Assets\car.h"
#include "Assets\tree.h"
#include "Assets\texture.h"
#include "Assets/world.h"
#include "Assets/ui.h"
#include "Assets/constants.h"
#include "Assets/game.h"
#include "Assets/shadow.h"

/* -- State Global ------------------------------------------------ */
static Chicken chicken;
static Car     cars[MAX_CARS];
static int     numCars = 0;

static int   gameOver = 0;
static int   gameWon  = 0;
static int   score    = 0;
static float ROAD_LEN;

/* -- Texture IDs ------------------------------------------------- */
static GLuint texLeaf  = 0;
static GLuint texBark  = 0;
static GLuint texGrass = 0;

/* -- Callback Display -------------------------------------------- */
void display(void) {
    char    buf[64];
    int     i;

    /* Posisi lampu — float[4], w=1 untuk positional light */
    float lightPos[4] = {0.0f, 12.0f, 30.0f, 1.0f};
    float planePoint[3]  = {0.0f,  0.0f, 0.0f};
    float planeNormal[3] = {0.0f,  1.0f, 0.0f};

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();

    

    /* Kamera mengikuti ayam */
    gluLookAt(
        chicken.x, 4.0f, chicken.z - 7.0f,
        chicken.x, 0.0f, chicken.z + 8.0f,
        0, 1, 0
    );

    /* Set posisi lampu SEBELUM gluLookAt agar tetap di world space */
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    /* Dunia (tanah, lane, tiang, pohon) */
    drawWorld(ROAD_LEN, texGrass, texBark, texLeaf, lightPos);

    /* Mobil — shadow + asli */
    for (i = 0; i < numCars; i++)
        renderCarWithShadow(lightPos, planePoint, planeNormal, &cars[i]);

    /* Ayam — shadow + asli */
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
        drawText2D(WINDOW_W/2 - 110, WINDOW_H/2 - 35,
                   "Tekan R untuk mulai lagi", 0.8f, 0.8f, 0.8f);

        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);

    /* Overlay Game Won */
    } else if (gameWon) {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        drawOverlay(0.5f, WINDOW_W, WINDOW_H);
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

/* -- Timer / Update ---------------------------------------------- */
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

        if (checkCollision(chicken, cars, numCars)) {
            chicken.alive = 0;
            gameOver      = 1;
        }

        if (chicken.z >= ROAD_LEN / 2.0f - LANE_WIDTH / 2.0f - 0.3f) {
            score  += 500;
            gameWon = 1;
        }
    }

    glutPostRedisplay();
}

/* -- Keyboard ---------------------------------------------------- */
void keyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;
    if (key == 27) exit(0);

    if (key == 'r' || key == 'R') {
        if (gameOver) score = 0;
        resetGame(&chicken, &gameOver, &gameWon, &score, &ROAD_LEN, cars, &numCars);
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

/* -- Reshape ----------------------------------------------------- */
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
    /* GLUT_STENCIL wajib ada untuk shadow stencil buffer */
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
    glClearColor(0.52f, 0.80f, 0.98f, 1.0f);

    texBark  = loadBMP("texture/bark.bmp");
    texLeaf  = loadBMP("texture/leaf.bmp");
    texGrass = loadBMP("texture/grass.bmp");

    resetGame(&chicken, &gameOver, &gameWon, &score, &ROAD_LEN, cars, &numCars);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}