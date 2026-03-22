#include <GL/glut.h>

float earthRotation = 0.0f;
float moonRotation = 0.0f;
float moonRevolution = 0.0f;

void init(){
    glClearColor(0,0,0,1);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat lightPos[] = {5,5,5,1};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0,0,-10);

    glPushMatrix();

        glRotatef(earthRotation,0,1,0);

        glColor3f(0.2,0.4,1.0);      // warna bumi
        glutSolidSphere(1.0,40,40);

        glPushMatrix();

            glRotatef(moonRevolution,0,1,0);
            glTranslatef(3,0,0);

            glRotatef(moonRotation,0,1,0);

            glColor3f(0.8,0.8,0.8);  // warna bulan
            glutSolidSphere(0.3,30,30);

        glPopMatrix();

    glPopMatrix();

    glutSwapBuffers();
}

void update(int value){
    earthRotation += 0.8f;
    moonRotation += 2.0f;
    moonRevolution += 0.3f;

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void reshape(int w,int h){
    glViewport(0,0,w,h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,(float)w/h,1,100);

    glMatrixMode(GL_MODELVIEW);
}

int main(int argc,char** argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800,600);
    glutCreateWindow("Animasi Bumi Bulan");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
}
