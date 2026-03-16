#include <gl/glut.h>
#include <gl/gl.h>
#include <math.h>

#define PI 3.1415926535898

void drawCircle(float r){
    int circle_points = 100;

    glBegin(GL_POLYGON);
    for(int i = 0; i < circle_points; i++){
        float angle = 2 * PI * i / circle_points;
        float x = r * cos(angle);
        float y = r * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    // aspal
    glPushMatrix();
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
            glVertex2f(-1.0f, -0.35f);
            glVertex2f(1.0f, -0.35f);
            glVertex2f(1.0f, -1.0f);
            glVertex2f(-1.0f, -1.0f);
        glEnd();
    glPopMatrix();

    // badan mobil bawah
    glPushMatrix();
        glColor3f(0.5f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
            glVertex2f(-0.6f, -0.2f);
            glVertex2f(0.6f, -0.2f);
            glVertex2f(0.6f, 0.1f);
            glVertex2f(-0.6f, 0.1f);
        glEnd();
    glPopMatrix();

    // lampu depan
    glPushMatrix();
        glTranslatef(0.55f, 0.0f, 0.0f);
        glColor3f(1.0f, 1.0f, 0.0f);
        drawCircle(0.05f);
    glPopMatrix();

    // lampu belakang
    glPushMatrix();
        glTranslatef(-0.55f, 0.0f, 0.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        drawCircle(0.05f);
    glPopMatrix();

    // atap mobil
    glPushMatrix();
        glColor3f(0.7f, 0.1f, 0.1f);
        glBegin(GL_QUADS);
            glVertex2f(-0.3f, 0.1f);
            glVertex2f(0.3f, 0.1f);
            glVertex2f(0.2f, 0.35f);
            glVertex2f(-0.2f, 0.35f);
        glEnd();
    glPopMatrix();

    // roda kiri
    glPushMatrix();
        glTranslatef(-0.35f, -0.25f, 0.0f);
        glColor3f(0.0f, 0.0f, 0.0f);
        drawCircle(0.12f);
    glPopMatrix();

    // roda kanan
    glPushMatrix();
        glTranslatef(0.35f, -0.25f, 0.0f);
        glColor3f(0.0f, 0.0f, 0.0f);
        drawCircle(0.12f);
    glPopMatrix();

    // velg kiri
    glPushMatrix();
        glTranslatef(-0.35f, -0.25f, 0.0f);
        glColor3f(0.8f, 0.8f, 0.8f);
        drawCircle(0.05f);
    glPopMatrix();

    // velg kanan
    glPushMatrix();
        glTranslatef(0.35f, -0.25f, 0.0f);
        glColor3f(0.8f, 0.8f, 0.8f);
        drawCircle(0.05f);
    glPopMatrix();

    //kaca kiri
    glPushMatrix();
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
            glVertex2f(-0.27f, 0.12f); // kiri bawah
            glVertex2f(-0.01f, 0.12f); // kanan bawah
            glVertex2f(-0.01f, 0.32f); // kanan atas
            glVertex2f(-0.19f, 0.32f); // kiri atas
        glEnd();
    glPopMatrix();

    //kaca kanan
    glPushMatrix();
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
            glVertex2f(0.01f, 0.12f); // kiri bawah
            glVertex2f(0.27f, 0.12f); // kanan bawah
            glVertex2f(0.19f, 0.32f); // kanan atas
            glVertex2f(0.01f, 0.32f); // kiri atas
        glEnd();
    glPopMatrix();

    glFlush();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow("Dzaki Fathul 'Alim Cahyo - 24060124130103");
    glutDisplayFunc(RenderScene);
    glClearColor(0.4f, 0.7f, 1.0f, 1.0f);
    glutMainLoop();
    return 0;
}
