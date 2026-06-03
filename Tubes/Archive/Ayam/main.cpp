#ifdef _WIN32
  #include <windows.h>
  #include <GL/freeglut.h>
#else
  #include <GL/glut.h>
#endif

#include <math.h>

float angle = 0;
int viewMode = 0;

/* ================= BOX ================= */
void drawBox(float w, float h, float d) {
    float hw = w/2, hh = h/2, hd = d/2;

    // ===== FILL =====
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    glBegin(GL_QUADS);

    // Top
    glVertex3f(-hw, hh,-hd); glVertex3f(hw, hh,-hd);
    glVertex3f(hw, hh, hd);  glVertex3f(-hw, hh, hd);
    // Bottom
    glVertex3f(-hw,-hh,-hd); glVertex3f(-hw,-hh, hd);
    glVertex3f(hw,-hh, hd);  glVertex3f(hw,-hh,-hd);
    // Front
    glVertex3f(-hw,-hh, hd); glVertex3f(hw,-hh, hd);
    glVertex3f(hw, hh, hd);  glVertex3f(-hw, hh, hd);
    // Back
    glVertex3f(-hw,-hh,-hd); glVertex3f(-hw, hh,-hd);
    glVertex3f(hw, hh,-hd);  glVertex3f(hw,-hh,-hd);
    // Left
    glVertex3f(-hw,-hh,-hd); glVertex3f(-hw,-hh, hd);
    glVertex3f(-hw, hh, hd); glVertex3f(-hw, hh,-hd);
    // Right
    glVertex3f(hw,-hh,-hd); glVertex3f(hw, hh,-hd);
    glVertex3f(hw, hh, hd); glVertex3f(hw,-hh, hd);

    glEnd();

    glDisable(GL_POLYGON_OFFSET_FILL);

    // ===== OUTLINE =====
    glColor3f(0,0,0);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    // Bottom square
    glVertex3f(-hw,-hh,-hd); glVertex3f(hw,-hh,-hd);
    glVertex3f(hw,-hh,-hd);  glVertex3f(hw,-hh,hd);
    glVertex3f(hw,-hh,hd);   glVertex3f(-hw,-hh,hd);
    glVertex3f(-hw,-hh,hd);  glVertex3f(-hw,-hh,-hd);
    // Top square
    glVertex3f(-hw,hh,-hd); glVertex3f(hw,hh,-hd);
    glVertex3f(hw,hh,-hd);  glVertex3f(hw,hh,hd);
    glVertex3f(hw,hh,hd);   glVertex3f(-hw,hh,hd);
    glVertex3f(-hw,hh,hd);  glVertex3f(-hw,hh,-hd);
    // Vertical edges
    glVertex3f(-hw,-hh,-hd); glVertex3f(-hw,hh,-hd);
    glVertex3f(hw,-hh,-hd);  glVertex3f(hw,hh,-hd);
    glVertex3f(hw,-hh,hd);   glVertex3f(hw,hh,hd);
    glVertex3f(-hw,-hh,hd);  glVertex3f(-hw,hh,hd);
    glEnd();
}

/* ================= CHICKEN ================= */
void drawChicken() {
    glPushMatrix();

    // BODY
    glColor3f(1.0f, 1.05f, 1.0f);
    drawBox(0.7f, 0.6f, 0.9f);

    // HEAD
    glPushMatrix();
    glTranslatef(0, 0.55f, 0.3f);
    glColor3f(1.0f, 1.05f, 1.0f);
    drawBox(0.45f, 0.4f, 0.45f);
    glPopMatrix();

    // BEAK
    glPushMatrix();
    glTranslatef(0, 0.5f, 0.53f);
    glColor3f(1.0f, 1.05f, 1.0f);
    drawBox(0.15f, 0.1f, 0.1f);
    glPopMatrix();

    // COMB (jengger)
    glPushMatrix();
    glTranslatef(0, 0.82f, 0.28f);
    glColor3f(1.0f, 1.05f, 1.0f);
    drawBox(0.12f, 0.15f, 0.12f);
    glPopMatrix();

    // LEFT WING
    glPushMatrix();
    glTranslatef(-0.38f, 0.05f, 0);
    glColor3f(1.0f, 1.05f, 1.0f);
    drawBox(0.08f, 0.4f, 0.7f);
    glPopMatrix();

    // RIGHT WING
    glPushMatrix();
    glTranslatef(0.38f, 0.05f, 0);
    glColor3f(1.0f, 1.05f, 1.0f);
    drawBox(0.08f, 0.4f, 0.7f);
    glPopMatrix();

    // LEFT LEG
    glPushMatrix();
    glTranslatef(-0.15f, -0.42f, 0);
    glColor3f(1.0f, 1.05f, 1.0f);
    drawBox(0.1f, 0.25f, 0.1f);
    glPopMatrix();

    // RIGHT LEG
    glPushMatrix();
    glTranslatef(0.15f, -0.42f, 0);
    glColor3f(1.0f, 1.05f, 1.0f);
    drawBox(0.1f, 0.25f, 0.1f);
    glPopMatrix();

    // LEFT FOOT
    glPushMatrix();
    glTranslatef(-0.15f, -0.57f, 0.08f);
    glColor3f(1.0f, 1.05f, 1.0f);
    drawBox(0.18f, 0.06f, 0.22f);
    glPopMatrix();

    // RIGHT FOOT
    glPushMatrix();
    glTranslatef(0.15f, -0.57f, 0.08f);
    glColor3f(1.0f, 1.05f, 1.0f);
    drawBox(0.18f, 0.06f, 0.22f);
    glPopMatrix();

    // TAIL
    glPushMatrix();
    glTranslatef(0, 0.2f, -0.55f);
    glColor3f(1.0f, 1.05f, 1.0f);
    drawBox(0.35f, 0.35f, 0.15f);
    glPopMatrix();

    glPopMatrix();
}

/* ================= CAMERA ================= */
void setCamera() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Bounding box ayam: x[-0.5,0.5], y[-0.62,0.97], z[-0.70,0.58]
    // Center approx: x=0, y=0.175, z=-0.06
    float cx = 0.0f, cy = 0.175f, cz = -0.06f;
    float dist = 4.0f;

    switch(viewMode) {
        case 0: // Depan (front) - lihat dari +Z
            gluLookAt(cx, cy, cz+dist,  cx,cy,cz,  0,1,0);
            break;
        case 1: // Kanan (right) - lihat dari +X
            gluLookAt(cx+dist, cy, cz,  cx,cy,cz,  0,1,0);
            break;
        case 2: // Belakang (back) - lihat dari -Z
            gluLookAt(cx, cy, cz-dist,  cx,cy,cz,  0,1,0);
            break;
        case 3: // Kiri (left) - lihat dari -X
            gluLookAt(cx-dist, cy, cz,  cx,cy,cz,  0,1,0);
            break;
        case 4: // Atas (top) - lihat dari +Y
            gluLookAt(cx, cy+dist, cz,  cx,cy,cz,  0,0,-1);
            break;
    }
}

/* ================= DISPLAY ================= */
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Label nama view
    const char* labels[] = {
        "Front (Depan)",
        "Right (Kanan)",
        "Back (Belakang)",
        "Left (Kiri)",
        "Top (Atas)"
    };

    setCamera();
    drawChicken();

    // Tampilkan label view di pojok kiri atas
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(0.1f, 0.1f, 0.1f);
    glRasterPos2i(10, 580);
    const char* s = labels[viewMode];
    while(*s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *s++);

    // Info navigasi
    glRasterPos2i(10, 10);
    const char* nav = "Left/Right Arrow = Ganti View";
    while(*nav) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *nav++);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}

/* ================= UPDATE ================= */
void update() {
    glutPostRedisplay();
}

/* ================= INPUT ================= */
void specialKeys(int key, int x, int y) {
    if(key == GLUT_KEY_RIGHT) viewMode = (viewMode + 1) % 5;
    if(key == GLUT_KEY_LEFT)  viewMode = (viewMode - 1 + 5) % 5;
}

/* ================= INIT ================= */
void init() {
    glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Orthographic: sesuaikan dengan ukuran ayam + margin
    // Ayam span y: ~1.6 unit, z: ~1.3 unit, x: ~1 unit
    // Pakai window 3x2.25 agar proporsional 800x600
    glOrtho(-1.8, 1.8, -1.35, 1.35, -10, 10);

    glMatrixMode(GL_MODELVIEW);
}

/* ================= MAIN ================= */
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Ayam Orthographic - Arrow Keys: Ganti View");

    init();

    glutDisplayFunc(display);
    glutIdleFunc(update);
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}