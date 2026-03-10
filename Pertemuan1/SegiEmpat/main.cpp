#include <gl/glut.h>

void SegiEmpat(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 0.0f, 0.0f); // Mengatur warna menjadi merah
    glRectf(-0.18, 0.18, 0.18, -0.18); // Menggambar segi empat (x1, y1, x2, y2)
    glFlush();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow("Dzaki Fathul 'Alim Cahyo'");
    glutDisplayFunc(SegiEmpat);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Latar belakang warna biru
    glutMainLoop();
    return 0;
}
