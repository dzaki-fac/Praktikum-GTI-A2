#ifdef _WIN32
  #include <windows.h>
  #include <GL/freeglut.h>
#else
  #include <GL/glut.h>
#endif

#include <math.h>

/* ================= GLOBAL ================= */
float angle = 0;

/* ================= DRAW BOX ================= */
void drawBox(float w, float h, float d) {
    float hw = w/2, hh = h/2, hd = d/2;

    glBegin(GL_QUADS);

    glNormal3f(0,1,0);
    glVertex3f(-hw, hh,-hd); glVertex3f(hw, hh,-hd);
    glVertex3f(hw, hh, hd);  glVertex3f(-hw, hh, hd);

    glNormal3f(0,-1,0);
    glVertex3f(-hw,-hh,-hd); glVertex3f(-hw,-hh, hd);
    glVertex3f(hw,-hh, hd);  glVertex3f(hw,-hh,-hd);

    glNormal3f(0,0,1);
    glVertex3f(-hw,-hh, hd); glVertex3f(hw,-hh, hd);
    glVertex3f(hw, hh, hd);  glVertex3f(-hw, hh, hd);

    glNormal3f(0,0,-1);
    glVertex3f(-hw,-hh,-hd); glVertex3f(-hw, hh,-hd);
    glVertex3f(hw, hh,-hd);  glVertex3f(hw,-hh,-hd);

    glNormal3f(-1,0,0);
    glVertex3f(-hw,-hh,-hd); glVertex3f(-hw,-hh, hd);
    glVertex3f(-hw, hh, hd); glVertex3f(-hw, hh,-hd);

    glNormal3f(1,0,0);
    glVertex3f(hw,-hh,-hd); glVertex3f(hw, hh,-hd);
    glVertex3f(hw, hh, hd); glVertex3f(hw,-hh, hd);

    glEnd();
}

/* ================= DRAW CHICKEN ================= */
void drawChicken(float bobAngle) {
    // float bob = sin(bobAngle) * 0.08f;
    float bob = 0;

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

    /* Mata */
    glPushMatrix();
    glTranslatef(-0.17f, 0.09f, 0.1f);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawBox(0.08f, 0.08f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.17f, 0.09f, 0.1f);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawBox(0.08f, 0.08f, 0.05f);
    glPopMatrix();

    glPopMatrix();

    /* Jengger */
    glPushMatrix();
    glTranslatef(0, 0.85f + bob, 0.3f);
    glColor3f(1.0f, 0.1f, 0.1f);
    drawBox(0.12f, 0.2f, 0.12f);
    glPopMatrix();

    /* Sayap */
    glPushMatrix();
    glTranslatef(-0.42f, 0.05f, 0);
    glRotatef(-20, 0, 0, 1);
    glColor3f(0.85f, 0.75f, 0.4f);
    drawBox(0.15f, 0.45f, 0.7f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.42f, 0.05f, 0);
    glRotatef(20, 0, 0, 1);
    glColor3f(0.85f, 0.75f, 0.4f);
    drawBox(0.15f, 0.45f, 0.7f);
    glPopMatrix();

    /* Kaki */
    glPushMatrix();
    glTranslatef(-0.15f, -0.38f, 0);
    glColor3f(1.0f, 0.55f, 0.0f);
    drawBox(0.1f, 0.25f, 0.1f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.15f, -0.38f, 0);
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

/* ================= DISPLAY ================= */
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(3,3,5, 0,0,0, 0,1,0);

    GLfloat lightPos[] = {5,10,5,1};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    drawChicken(angle);

    glutSwapBuffers();
}

/* ================= UPDATE ================= */
void update() {
    angle += 0.1f;
    glutPostRedisplay();
}

/* ================= INIT ================= */
void init() {
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.6f,0.6f,0.6f,1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 800.0/600.0, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    GLfloat ambient[] = {0.5,0.5,0.5,1};
    GLfloat diffuse[] = {1,1,1,1};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
}

/* ================= MAIN ================= */
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800,600);
    glutCreateWindow("Render Ayam 3D");

    init();

    glutDisplayFunc(display);
    glutIdleFunc(update);

    glutMainLoop();
    return 0;
}