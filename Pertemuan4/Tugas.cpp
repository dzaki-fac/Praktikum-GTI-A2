// CarLighting.cpp
// Simulasi mobil 3D dengan lighting, depth test, dan kontrol kamera
// Kontrol: Arrow keys = gerak kamera, W/S = maju/mundur kamera
// Compile: g++ CarLighting.cpp -o CarLighting -lGL -lGLU -lglut -lm

#include <math.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ===================== KAMERA =====================
float angle = 0.0f, deltaAngle = 0.0f;
float x = 0.0f, y = 3.0f, z = 15.0f;
float lx = 0.0f, ly = -0.1f, lz = -1.0f;
int deltaMove = 0;
float ratio;
int W, H;

// ===================== PENCAHAYAAN =====================
const GLfloat light_ambient[]   = { 0.3f, 0.3f, 0.3f, 1.0f };
const GLfloat light_diffuse[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[]  = { 5.0f, 20.0f, 10.0f, 1.0f };

const GLfloat mat_ambient[]     = { 0.6f, 0.6f, 0.6f, 1.0f };
const GLfloat mat_diffuse[]     = { 0.9f, 0.9f, 0.9f, 1.0f };
const GLfloat mat_specular[]    = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[]  = { 80.0f };
const GLfloat emit_off[]        = { 0.0f, 0.0f, 0.0f, 1.0f };

// ===================== STRUCT MOBIL =====================
typedef struct {
    float x, z;
    float speed;
    float panjang;
    float r, g, b;
    float angle; // rotasi Y mobil (derajat)
} Car;

// Dua mobil contoh
Car mobil1 = { -2.0f, 0.0f, 0.0f, 2.5f, 0.8f, 0.1f, 0.1f, 0.0f }; // merah
Car mobil2 = {  3.0f, 0.0f, 0.0f, 2.5f, 0.1f, 0.3f, 0.9f, 0.0f }; // biru

// ===================== FUNGSI KAMERA =====================
void orientMe(float ang)
{
    lx = sin(ang);
    lz = -cos(ang);
    glLoadIdentity();
    gluLookAt(x, y, z,
              x + lx, y + ly, z + lz,
              0.0f, 1.0f, 0.0f);
}

void moveMeFlat(int i)
{
    x = x + i * (lx) * 0.1f;
    z = z + i * (lz) * 0.1f;
    glLoadIdentity();
    gluLookAt(x, y, z,
              x + lx, y + ly, z + lz,
              0.0f, 1.0f, 0.0f);
}

// ===================== RESHAPE =====================
void Reshape(int w1, int h1)
{
    if (h1 == 0) h1 = 1;
    W = w1; H = h1;
    ratio = 1.0f * W / H;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, W, H);
    gluPerspective(45.0, ratio, 0.1, 500.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x, y, z,
              x + lx, y + ly, z + lz,
              0.0f, 1.0f, 0.0f);
}

// ===================== GRID =====================
void Grid()
{
    // Nonaktifkan lighting sementara agar grid abu-abu flat
    glDisable(GL_LIGHTING);
    double i;
    const float Z_MIN = -50, Z_MAX = 50;
    const float X_MIN = -50, X_MAX = 50;
    const float gap = 1.5f;
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_LINES);
    for (i = Z_MIN; i <= Z_MAX; i += gap) {
        glVertex3f(i, 0, Z_MIN);
        glVertex3f(i, 0, Z_MAX);
    }
    for (i = X_MIN; i <= X_MAX; i += gap) {
        glVertex3f(X_MIN, 0, i);
        glVertex3f(X_MAX, 0, i);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

// ===================== DRAW BOX =====================
void drawBox(float w, float h, float d)
{
    float hw = w / 2, hh = h / 2, hd = d / 2;

    glBegin(GL_QUADS);

    // ATAS
    glNormal3f(0, 1, 0);
    glVertex3f(-hw,  hh, -hd); glVertex3f( hw,  hh, -hd);
    glVertex3f( hw,  hh,  hd); glVertex3f(-hw,  hh,  hd);

    // BAWAH
    glNormal3f(0, -1, 0);
    glVertex3f(-hw, -hh, -hd); glVertex3f(-hw, -hh,  hd);
    glVertex3f( hw, -hh,  hd); glVertex3f( hw, -hh, -hd);

    // DEPAN
    glNormal3f(0, 0, 1);
    glVertex3f(-hw, -hh,  hd); glVertex3f( hw, -hh,  hd);
    glVertex3f( hw,  hh,  hd); glVertex3f(-hw,  hh,  hd);

    // BELAKANG
    glNormal3f(0, 0, -1);
    glVertex3f(-hw, -hh, -hd); glVertex3f(-hw,  hh, -hd);
    glVertex3f( hw,  hh, -hd); glVertex3f( hw, -hh, -hd);

    // KIRI
    glNormal3f(-1, 0, 0);
    glVertex3f(-hw, -hh, -hd); glVertex3f(-hw, -hh,  hd);
    glVertex3f(-hw,  hh,  hd); glVertex3f(-hw,  hh, -hd);

    // KANAN
    glNormal3f(1, 0, 0);
    glVertex3f( hw, -hh, -hd); glVertex3f( hw,  hh, -hd);
    glVertex3f( hw,  hh,  hd); glVertex3f( hw, -hh,  hd);

    glEnd();
}

// ===================== DRAW CAR =====================
void drawCar(const Car *c)
{
    float halfLength = c->panjang / 2.0f;
    float halfWidth  = 0.9f / 2.0f;
    float front = (c->speed >= 0) ? 1.0f : -1.0f;

    float wx[2] = { halfLength - 0.25f, -(halfLength - 0.25f) };
    float wz    = halfWidth + 0.05f;

    glPushMatrix();
    glTranslatef(c->x, 0.35f, c->z);
    glRotatef(c->angle, 0, 1, 0);

    /* ---- BODY ---- */
    glColor3f(c->r, c->g, c->b);
    drawBox(c->panjang, 0.35f, 0.9f);

    /* ---- KABIN ---- */
    glPushMatrix();
        glTranslatef(-0.1f, 0.315f, 0);
        glColor3f(c->r * 0.65f, c->g * 0.65f, c->b * 0.65f);
        drawBox(c->panjang * 0.58f, 0.28f, 0.72f);
    glPopMatrix();

    /* ---- KACA DEPAN ---- */
    glPushMatrix();
        glTranslatef(halfLength * 0.28f, 0.31f, 0);
        glColor3f(0.5f, 0.8f, 1.0f);
        drawBox(0.08f, 0.2f, 0.62f);
    glPopMatrix();

    /* ---- KACA BELAKANG ---- */
    glPushMatrix();
        glTranslatef(-halfLength * 0.42f, 0.31f, 0);
        glColor3f(0.5f, 0.8f, 1.0f);
        drawBox(0.08f, 0.2f, 0.62f);
    glPopMatrix();

    /* ---- RODA (4 buah) ---- */
    glColor3f(0.1f, 0.1f, 0.1f);
    for (int i = 0; i < 2; i++) {
        // kiri
        glPushMatrix();
            glTranslatef(wx[i], -0.18f, -wz);
            drawBox(0.22f, 0.22f, 0.14f);
        glPopMatrix();
        // kanan
        glPushMatrix();
            glTranslatef(wx[i], -0.18f,  wz);
            drawBox(0.22f, 0.22f, 0.14f);
        glPopMatrix();
    }

    /* ---- VELG (tengah roda, abu terang) ---- */
    glColor3f(0.7f, 0.7f, 0.75f);
    for (int i = 0; i < 2; i++) {
        glPushMatrix();
            glTranslatef(wx[i], -0.18f, -wz - 0.08f);
            drawBox(0.14f, 0.14f, 0.02f);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(wx[i], -0.18f,  wz + 0.08f);
            drawBox(0.14f, 0.14f, 0.02f);
        glPopMatrix();
    }

    /* ---- BUMPER DEPAN ---- */
    glColor3f(0.2f, 0.2f, 0.2f);
    glPushMatrix();
        glTranslatef(front * (halfLength + 0.04f), -0.1f, 0);
        drawBox(0.08f, 0.12f, 0.78f);
    glPopMatrix();

    /* ---- BUMPER BELAKANG ---- */
    glPushMatrix();
        glTranslatef(-front * (halfLength + 0.04f), -0.1f, 0);
        drawBox(0.08f, 0.12f, 0.78f);
    glPopMatrix();

    /* ---- LAMPU DEPAN (emissive kuning) ---- */
    GLfloat emitYellow[] = { 1.0f, 1.0f, 0.6f, 1.0f };
    glMaterialfv(GL_FRONT, GL_EMISSION, emitYellow);
    glColor3f(1.0f, 1.0f, 0.6f);

    glPushMatrix();
        glTranslatef(front * halfLength, 0.08f, -0.28f);
        drawBox(0.06f, 0.1f, 0.14f);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(front * halfLength, 0.08f,  0.28f);
        drawBox(0.06f, 0.1f, 0.14f);
    glPopMatrix();

    glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);

    /* ---- LAMPU BELAKANG (emissive merah) ---- */
    GLfloat emitRed[] = { 0.9f, 0.1f, 0.1f, 1.0f };
    glMaterialfv(GL_FRONT, GL_EMISSION, emitRed);
    glColor3f(1.0f, 0.2f, 0.2f);

    glPushMatrix();
        glTranslatef(-front * halfLength, 0.08f, -0.28f);
        drawBox(0.06f, 0.1f, 0.14f);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-front * halfLength, 0.08f,  0.28f);
        drawBox(0.06f, 0.1f, 0.14f);
    glPopMatrix();

    glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);

    glPopMatrix(); // end car transform
}

// ===================== DISPLAY =====================
void display()
{
    // Update kamera
    if (deltaMove)
        moveMeFlat(deltaMove);
    if (deltaAngle) {
        angle += deltaAngle;
        orientMe(angle);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update posisi light agar tetap di world space
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // Gambar grid
    Grid();

    // Gambar lantai (tipis, warna gelap)
    glColor3f(0.25f, 0.25f, 0.25f);
    glPushMatrix();
        glTranslatef(0, -0.01f, 0);
        drawBox(100.0f, 0.01f, 100.0f);
    glPopMatrix();

    // Gambar mobil
    drawCar(&mobil1);
    // drawCar(&mobil2);

    glutSwapBuffers();
    glFlush();
}

// ===================== INPUT =====================
void pressKey(int key, int xx, int yy)
{
    switch (key) {
        case GLUT_KEY_LEFT  : deltaAngle = -0.01f; break;
        case GLUT_KEY_RIGHT : deltaAngle =  0.01f; break;
        case GLUT_KEY_UP    : deltaMove  =  1;     break;
        case GLUT_KEY_DOWN  : deltaMove  = -1;     break;
    }
}

void releaseKey(int key, int xx, int yy)
{
    switch (key) {
        case GLUT_KEY_LEFT  : if (deltaAngle < 0) deltaAngle = 0; break;
        case GLUT_KEY_RIGHT : if (deltaAngle > 0) deltaAngle = 0; break;
        case GLUT_KEY_UP    : if (deltaMove  > 0) deltaMove  = 0; break;
        case GLUT_KEY_DOWN  : if (deltaMove  < 0) deltaMove  = 0; break;
    }
}

void normalKey(unsigned char key, int xx, int yy)
{
    // ESC untuk keluar
    if (key == 27) exit(0);

    // Naik/turun kamera (Q/E)
    if (key == 'q' || key == 'Q') { y += 0.2f; orientMe(angle); }
    if (key == 'e' || key == 'E') { y -= 0.2f; orientMe(angle); }
}

// ===================== LIGHTING SETUP =====================
void initLighting()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION,  emit_off);
}

void init()
{
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f); // background gelap
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glShadeModel(GL_SMOOTH);
    initLighting();
}

// ===================== MAIN =====================
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Car - Lighting & Depth");

    glutIgnoreKeyRepeat(1);
    glutSpecialFunc(pressKey);
    glutSpecialUpFunc(releaseKey);
    glutKeyboardFunc(normalKey);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(Reshape);

    init();
    glutMainLoop();
    return 0;
}