/*
 * CHICKEN CROSS THE ROAD 3D
 * Pure C + FreeGLUT
 *
 * Compile (Linux/Mac):
 *   gcc chicken_cross.c -o chicken_cross -lGL -lGLU -lglut -lm
 *
 * Compile (Windows MinGW):
 *   gcc chicken_cross.c -o Project1.exe -mwindows -lfreeglut -lopengl32 -lglu32 -lm
 *
 * Kontrol:
 *   W / Arrow Up    : Maju
 *   S / Arrow Down  : Mundur
 *   A / Arrow Left  : Kiri
 *   D / Arrow Right : Kanan
 *   R               : Restart
 *   ESC             : Keluar
 */

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

/* -- Konstanta --------------------------------------------------- */
#define WINDOW_W      900
#define WINDOW_H      700
#define LANE_WIDTH    2.0f
#define NUM_LANES     8
#define MAX_CARS      30
#define CAR_SPEED_BASE 0.03f
#define PI 3.14159265358979f

/* -- Struct ------------------------------------------------------ */
typedef struct {
    float x, z;
    float speed;
    float r, g, b;
    float width, panjang;
} Car;

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

    glColor4f(0.0f, 0.0f, 0.0f, alpha); // hitam transparan

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
//            c->panjang = 0.8f;
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

/* -- Gambar Kotak ------------------------------------------------ */
static void drawBox(float w, float h, float d) {
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

/* -- Gambar Ayam ------------------------------------------------- */
static void drawChicken(float bobAngle) {
    float bob = (float)sin(bobAngle) * 0.08f;

    glPushMatrix();

    /* Badan */
    glColor3f(0.95f, 0.9f, 0.6f);
    drawBox(0.7f, 0.6f, 0.9f);

    /* Kepala */
    glPushMatrix();
    glTranslatef(0, 0.55f + bob, 0.3f);
    glColor3f(0.95f, 0.85f, 0.5f);
    drawBox(0.45f, 0.4f, 0.45f);

    /* Paruh */
    glPushMatrix();
    glTranslatef(0, -0.05f, 0.28f);
    glColor3f(1.0f, 0.55f, 0.0f);
    drawBox(0.15f, 0.1f, 0.15f);
    glPopMatrix();

    /* Mata kiri */
    glPushMatrix();
    glTranslatef(-0.17f, 0.09f, 0.1f);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawBox(0.08f, 0.08f, 0.05f);
    glPopMatrix();

    /* Mata kanan */
    glPushMatrix();
    glTranslatef(0.17f, 0.09f, 0.1f);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawBox(0.08f, 0.08f, 0.05f);
    glPopMatrix();

    glPopMatrix(); /* kepala */

    /* Jengger */
    glPushMatrix();
    glTranslatef(0, 0.85f + bob, 0.3f);
    glColor3f(1.0f, 0.1f, 0.1f);
    drawBox(0.12f, 0.2f, 0.12f);
    glPopMatrix();

    /* Sayap kiri */
    glPushMatrix();
    glTranslatef(-0.42f, 0.05f, 0.0f);
    glRotatef(-20, 0, 0, 1);
    glColor3f(0.85f, 0.75f, 0.4f);
    drawBox(0.15f, 0.45f, 0.7f);
    glPopMatrix();

    /* Sayap kanan */
    glPushMatrix();
    glTranslatef(0.42f, 0.05f, 0.0f);
    glRotatef(20, 0, 0, 1);
    glColor3f(0.85f, 0.75f, 0.4f);
    drawBox(0.15f, 0.45f, 0.7f);
    glPopMatrix();

    /* Kaki kiri */
    glPushMatrix();
    glTranslatef(-0.15f, -0.38f, 0.0f);
    glColor3f(1.0f, 0.55f, 0.0f);
    drawBox(0.1f, 0.25f, 0.1f);
    glPopMatrix();

    /* Kaki kanan */
    glPushMatrix();
    glTranslatef(0.15f, -0.38f, 0.0f);
    glColor3f(1.0f, 0.55f, 0.0f);
    drawBox(0.1f, 0.25f, 0.1f);
    glPopMatrix();

    /* Ekor */
    glPushMatrix();
    glTranslatef(0, 0.2f, -0.52f);
    glRotatef(35, 1, 0, 0);
    glColor3f(0.7f, 0.6f, 0.3f);
    drawBox(0.5f, 0.35f, 0.25f);
    glPopMatrix();

    glPopMatrix();
}

static void drawCar(const Car *c) {
    int i;

    float halfLength = c->panjang / 2.0f;
    float halfWidth  = 0.9f / 2.0f;

    /* posisi roda (depan-belakang di X) */
    float wx[2] = { halfLength - 0.2f, -(halfLength - 0.2f) };

    /* posisi kiri-kanan (Z) */
    float wz = halfWidth + 0.05f;

    glPushMatrix();
    glTranslatef(c->x, 0.35f, c->z);

    /* ================= BODY ================= */
    glColor3f(c->r, c->g, c->b);
    drawBox(c->panjang, 0.35f, 0.9f);

    /* ================= KABIN ================= */
    glPushMatrix();
        glTranslatef(-0.1f, 0.28f, 0.0f);
        glColor3f(c->r * 0.7f, c->g * 0.7f, c->b * 0.7f);
        drawBox(c->panjang * 0.6f, 0.28f, 0.7f);
    glPopMatrix();

    /* ================= KACA ================= */
    glPushMatrix();
        glTranslatef(halfLength * 0.3f, 0.3f, 0.0f);
        glColor3f(0.6f, 0.8f, 1.0f);
        drawBox(0.1f, 0.22f, 0.6f);
    glPopMatrix();

    /* ================= RODA ================= */
    glColor3f(0.15f, 0.15f, 0.15f);
    for (i = 0; i < 2; i++) {
        // kiri
        glPushMatrix();
            glTranslatef(wx[i], -0.2f, -wz);
            drawBox(0.22f, 0.22f, 0.12f);
        glPopMatrix();

        // kanan
        glPushMatrix();
            glTranslatef(wx[i], -0.2f, wz);
            drawBox(0.22f, 0.22f, 0.12f);
        glPopMatrix();
    }

	/* arah depan berdasarkan speed */
	float front = (c->speed > 0) ? 1.0f : -1.0f;
	
	/* ================= LAMPU DEPAN ================= */
	glColor3f(1.0f, 1.0f, 0.7f);
	
	glPushMatrix();
	    glTranslatef(front * halfLength, 0.1f, -0.3f);
	    drawBox(0.05f, 0.1f, 0.15f);
	glPopMatrix();
	
	glPushMatrix();
	    glTranslatef(front * halfLength, 0.1f, 0.3f);
	    drawBox(0.05f, 0.1f, 0.15f);
	glPopMatrix();
	
	/* ================= LAMPU BELAKANG ================= */
	glColor3f(1.0f, 0.2f, 0.2f);
	
	glPushMatrix();
	    glTranslatef(-front * halfLength, 0.1f, -0.3f);
	    drawBox(0.05f, 0.1f, 0.15f);
	glPopMatrix();
	
	glPushMatrix();
	    glTranslatef(-front * halfLength, 0.1f, 0.3f);
	    drawBox(0.05f, 0.1f, 0.15f);
	glPopMatrix();
	
    glPopMatrix();
}

/* -- Gambar Dunia ------------------------------------------------ */
static void drawWorld(void) {
    int i;
    float startZ = -ROAD_LEN / 2.0f;
    float finZ   =  ROAD_LEN / 2.0f - LANE_WIDTH / 2.0f;

    /* Tanah luar */
    glColor3f(0.2f, 0.55f, 0.15f);
    glBegin(GL_QUADS);
    glVertex3f(-50,-0.01f,-20); glVertex3f(50,-0.01f,-20);
    glVertex3f( 50,-0.01f, 20); glVertex3f(-50,-0.01f, 20);
    glEnd();

    /* Lane per lane */
    for (i = 0; i < NUM_LANES; i++) {
        float z0 = startZ + i * LANE_WIDTH;
        float z1 = 1 * z0 + LANE_WIDTH;
        float zc = 1 * (z0 + z1) / 2.0f;
        float xi;

        if (i == 0 || i == NUM_LANES - 1)
            glColor3f(0.25f, 0.65f, 0.2f);   /* rumput */
        else
            glColor3f(0.35f, 0.35f, 0.35f);  /* aspal */

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

    /* Pohon */
    {
        float treeX[] = {-10.5f,-11.2f, 10.5f,11.3f,-10.8f,10.7f};
        float treeZ[] = { -6.0f,  0.0f, -4.0f, 3.0f,  5.0f,-1.0f};
        int t;
        for (t = 0; t < 6; t++) {
            glPushMatrix();
            glTranslatef(treeX[t], 0, treeZ[t]);
            glColor3f(0.4f, 0.25f, 0.1f);
            drawBox(0.2f, 1.2f, 0.2f);
            glTranslatef(0, 1.0f, 0);
            glColor3f(0.15f, 0.55f, 0.1f);
            drawBox(1.0f, 1.4f, 1.0f);
            glPopMatrix();
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

    glPopAttrib(); // ?? balikin semua state (lighting nyala lagi)
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

    /* Kamera isometrik ala Crossy Road mobile:
       ikuti ayam, sudut ~45deg dari belakang-kiri-atas */
//    eyeX = chicken.x - 8.0f;   /* sedikit ke kiri-belakang */
//    eyeY = 10.0f;               /* tinggi */
//    eyeZ = chicken.z - 8.0f;   /* mundur dari ayam */
//    gluLookAt(eyeX, eyeY, eyeZ,
//              chicken.x, 0.3f, chicken.z,  /* fokus ke ayam */
//              0, 1, 0);
              
	eyeX = chicken.x;
	 eyeY = 4.0f;          // lebih rendah dari sebelumnya
	 eyeZ = chicken.z - 7.0f; // lebih jauh ke belakang
	
	gluLookAt(
	    eyeX, eyeY, eyeZ,
	    chicken.x, 0.0f, chicken.z + 8.0f, // LIHAT JAUH KE DEPAN
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

    resetGame();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}
