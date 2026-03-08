#include <windows.h>
#include <GL/glut.h>

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // GL_LINE_STRIP
    glBegin(GL_LINE_STRIP);
        glVertex2f(-0.9, 0.6);
        glVertex2f(-0.7, 0.8);
        glVertex2f(-0.5, 0.6);
        glVertex2f(-0.3, 0.8);
    glEnd();

    // GL_LINE_LOOP
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.2, 0.6);
        glVertex2f(0.0, 0.8);
        glVertex2f(0.2, 0.6);
        glVertex2f(0.0, 0.4);
    glEnd();

    // GL_TRIANGLE_FAN
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(-0.7, 0.0);
        glVertex2f(-0.8, -0.2);
        glVertex2f(-0.6, -0.2);
        glVertex2f(-0.5, 0.0);
        glVertex2f(-0.6, 0.2);
        glVertex2f(-0.8, 0.2);
    glEnd();

    // GL_TRIANGLE_STRIP
    glBegin(GL_TRIANGLE_STRIP);
        glVertex2f(-0.2, 0.0);
        glVertex2f(-0.1, -0.2);
        glVertex2f(0.0, 0.0);
        glVertex2f(0.1, -0.2);
        glVertex2f(0.2, 0.0);
    glEnd();

    // GL_QUADS
    glBegin(GL_QUADS);
        glVertex2f(0.3, 0.2);
        glVertex2f(0.5, 0.2);
        glVertex2f(0.5, 0.4);
        glVertex2f(0.3, 0.4);
    glEnd();

    // GL_QUAD_STRIP
    glBegin(GL_QUAD_STRIP);
        glVertex2f(0.6, -0.2);
        glVertex2f(0.6, 0.0);
        glVertex2f(0.8, -0.2);
        glVertex2f(0.8, 0.0);
        glVertex2f(1.0, -0.2);
        glVertex2f(1.0, 0.0);
    glEnd();

    glFlush();
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-1, 1, -1, 1);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Primitive OpenGL");

    init();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}
