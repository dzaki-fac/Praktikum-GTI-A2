#ifdef _WIN32
  #include <windows.h>
  #include <GL/freeglut.h>
#else
  #include <GL/glut.h>
#endif

#include <stdlib.h>

/* ================= STRUCT ================= */
typedef struct {
    float x, z;
    float speed;
    float r, g, b;
    float panjang;
} Car;

Car car;

/* ================= DRAW BOX ================= */
void drawBox(float w, float h, float d) {
    float hw = w/2, hh = h/2, hd = d/2;

    glBegin(GL_QUADS);

    // ATAS
    glNormal3f(0,1,0);
    glVertex3f(-hw, hh,-hd); glVertex3f(hw, hh,-hd);
    glVertex3f(hw, hh, hd);  glVertex3f(-hw, hh, hd);

    // BAWAH
    glNormal3f(0,-1,0);
    glVertex3f(-hw,-hh,-hd); glVertex3f(-hw,-hh, hd);
    glVertex3f(hw,-hh, hd);  glVertex3f(hw,-hh,-hd);

    // DEPAN
    glNormal3f(0,0,1);
    glVertex3f(-hw,-hh, hd); glVertex3f(hw,-hh, hd);
    glVertex3f(hw, hh, hd);  glVertex3f(-hw, hh, hd);

    // BELAKANG
    glNormal3f(0,0,-1);
    glVertex3f(-hw,-hh,-hd); glVertex3f(-hw, hh,-hd);
    glVertex3f(hw, hh,-hd);  glVertex3f(hw,-hh,-hd);

    // KIRI
    glNormal3f(-1,0,0);
    glVertex3f(-hw,-hh,-hd); glVertex3f(-hw,-hh, hd);
    glVertex3f(-hw, hh, hd); glVertex3f(-hw, hh,-hd);

    // KANAN
    glNormal3f(1,0,0);
    glVertex3f(hw,-hh,-hd); glVertex3f(hw, hh,-hd);
    glVertex3f(hw, hh, hd); glVertex3f(hw,-hh, hd);

    glEnd();
}

/* ================= DRAW CAR ================= */
void drawCar(const Car *c) {
    float halfLength = c->panjang / 2.0f;
    float halfWidth  = 0.9f / 2.0f;
    float front = (c->speed > 0) ? 1.0f : -1.0f;

    float wx[2] = { halfLength - 0.2f, -(halfLength - 0.2f) };
    float wz = halfWidth + 0.05f;

    glPushMatrix();
    glTranslatef(c->x, 0.35f, c->z);

    /* BODY */
    glColor3f(c->r, c->g, c->b);
    drawBox(c->panjang, 0.35f, 0.9f);

    /* KABIN */
    glPushMatrix();
        glTranslatef(-0.1f, 0.28f, 0);
        glColor3f(c->r*0.7f, c->g*0.7f, c->b*0.7f);
        drawBox(c->panjang*0.6f, 0.28f, 0.7f);
    glPopMatrix();

    /* KACA */
    glPushMatrix();
        glTranslatef(halfLength*0.3f, 0.3f, 0);
        glColor3f(0.6f, 0.8f, 1.0f);
        drawBox(0.1f, 0.22f, 0.6f);
    glPopMatrix();

    /* RODA */
    glColor3f(0.1f, 0.1f, 0.1f);
    for (int i = 0; i < 2; i++) {
        glPushMatrix();
            glTranslatef(wx[i], -0.2f, -wz);
            drawBox(0.22f, 0.22f, 0.12f);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(wx[i], -0.2f, wz);
            drawBox(0.22f, 0.22f, 0.12f);
        glPopMatrix();
    }

    /* LAMPU DEPAN (EMISSIVE) */
    GLfloat emitOn[] = {1,1,0.7f,1};
    GLfloat emitOff[] = {0,0,0,1};

    glMaterialfv(GL_FRONT, GL_EMISSION, emitOn);
    glColor3f(1,1,0.7f);

    glPushMatrix();
        glTranslatef(front*halfLength, 0.1f, -0.3f);
        drawBox(0.05f, 0.1f, 0.15f);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(front*halfLength, 0.1f, 0.3f);
        drawBox(0.05f, 0.1f, 0.15f);
    glPopMatrix();

    glMaterialfv(GL_FRONT, GL_EMISSION, emitOff);

    /* LAMPU BELAKANG */
    glColor3f(1,0.2f,0.2f);

    glPushMatrix();
        glTranslatef(-front*halfLength, 0.1f, -0.3f);
        drawBox(0.05f, 0.1f, 0.15f);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-front*halfLength, 0.1f, 0.3f);
        drawBox(0.05f, 0.1f, 0.15f);
    glPopMatrix();

    glPopMatrix();
}

/* ================= DISPLAY ================= */
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(3,3,5, 0,0,0, 0,1,0);

    /* LIGHT POSITION (WAJIB DI SINI) */
    GLfloat lightPos[] = {5.0f, 10.0f, 5.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    drawCar(&car);

    glutSwapBuffers();
}

/* ================= INIT ================= */
void init() {
    glEnable(GL_DEPTH_TEST);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    /* PROJECTION */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 800.0/600.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

    /* LIGHTING */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    GLfloat ambient[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    glShadeModel(GL_SMOOTH);

    /* DATA MOBIL */
    car.x = 0;
    car.z = 0;
    car.speed = 1;
    car.r = 0.2f;
    car.g = 0.6f;
    car.b = 1.0f;
    car.panjang = 1.5f;
}

/* ================= MAIN ================= */
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800,600);
    glutCreateWindow("Mobil 3D Lighting");

    init();

    glutDisplayFunc(display);
    glutIdleFunc(display);

    glutMainLoop();
    return 0;
}
