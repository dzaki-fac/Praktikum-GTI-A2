#include <GL/glut.h>
#include <stdlib.h>

// rotasi shoulder dan elbow tiap jari
static int s[5] = {105, 100, 95, 90, 100}; 
static int e[5] = {100, 100, 100, 100, 100};
static int jari = 0;

// u kontol kamera
static float camRotationX = 0.0, camRotationY = 50.0;
static int lastMouseX, lastMouseY;
static bool isDragging = false;
static float zoom = -13.0f; // jarak kamera
static float camX = 0.0, camY = 4.0; // atas bawah
static float camRotationZ = 90.0; // roll

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
}

void display(void) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    glTranslatef(camX, camY, zoom); // Kontrol Zoom
    glRotatef(camRotationX, 1.0, 0.0, 0.0); // Rotasi Vertikal
    glRotatef(camRotationY, 0.0, 1.0, 0.0); // Rotasi Horizontal
    glRotatef(camRotationZ, 0.0, 0.0, 1.0);
    
    //telapak
    glPushMatrix();
        glTranslatef(-3.5, 0.0, 0.9);
        glTranslatef(1.0, 0.0, 0.0);
        glPushMatrix();
            glScalef(2.5, 0.4, 2.5);
            glutWireCube(1.0);
        glPopMatrix();

    glPopMatrix();
    
    //kelingking
    glPushMatrix();
        glTranslatef(-1.25, 0.0, 0.0);
        glRotatef((GLfloat) s[0], 0.0, 0.0, 1.0);
        glTranslatef(1.0, 0.0, 0.0);
        glPushMatrix();
            glScalef(1.5, 0.4, 0.5);
            glutWireCube(1.0);
        glPopMatrix();
        
        glTranslatef(0.50, 0.0, 0.0);
        glRotatef((GLfloat) e[0], 0.0, 0.0, 1.0);
        glTranslatef(1.0, 0.0, 0.0);
        glPushMatrix();
            glScalef(1.5, 0.4, 0.5);
            glutWireCube(1.0);
        glPopMatrix();
    glPopMatrix();
    
    //manis
    glPushMatrix();
        glTranslatef(-1.25, 0.0, 0.6);
        glRotatef((GLfloat) s[1], 0.0, 0.0, 1.0);
        glTranslatef(1.0, 0.0, 0.0);
        glPushMatrix();
            glScalef(1.5, 0.4, 0.5);
            glutWireCube(1.0);
        glPopMatrix();
        
        glTranslatef(0.50, 0.0, 0.0);
        glRotatef((GLfloat) e[1], 0.0, 0.0, 1.0);
        glTranslatef(1.0, 0.0, 0.0);
        glPushMatrix();
            glScalef(1.5, 0.4, 0.5);
            glutWireCube(1.0);
        glPopMatrix();
    glPopMatrix();
    
    //tengah
    glPushMatrix();
        glTranslatef(-1.25, 0.0, 1.2);
        glRotatef((GLfloat) s[2], 0.0, 0.0, 1.0);
        glTranslatef(1.0, 0.0, 0.0);
        glPushMatrix();
            glScalef(1.5, 0.4, 0.5);
            glutWireCube(1.0);
        glPopMatrix();
        
        glTranslatef(0.50, 0.0, 0.0);
        glRotatef((GLfloat) e[2], 0.0, 0.0, 1.0);
        glTranslatef(1.0, 0.0, 0.0);
        glPushMatrix();
            glScalef(1.5, 0.4, 0.5);
            glutWireCube(1.0);
        glPopMatrix();
    glPopMatrix();
    
    
    //telunjuk
    glPushMatrix();
        glTranslatef(-1.25, 0.0, 1.8);
        glRotatef((GLfloat) s[3], 0.0, 0.0, 1.0);
        glTranslatef(1.0, 0.0, 0.0);
        glPushMatrix();
            glScalef(1.5, 0.4, 0.5);
            glutWireCube(1.0);
        glPopMatrix();
        
        glTranslatef(0.50, 0.0, 0.0);
        glRotatef((GLfloat) e[3], 0.0, 0.0, 1.0);
        glTranslatef(1.0, 0.0, 0.0);
        glPushMatrix();
            glScalef(1.5, 0.4, 0.5);
            glutWireCube(1.0);
        glPopMatrix();
    glPopMatrix();
    
    //jempol
    glPushMatrix();
        glTranslatef(-2.4, 0.0, 2.4);
        glRotatef((GLfloat) s[4], 0.0, 0.0, 1.0);
        glTranslatef(1.0, 0.0, 0.0);
        glPushMatrix();
            glScalef(1.5, 0.4, 0.5);
            glutWireCube(1.0);
        glPopMatrix();
        
        glTranslatef(0.50, 0.0, 0.0);
        glRotatef((GLfloat) e[4], 0.0, 1.0, 0.0);
        glTranslatef(1.0, 0.0, 0.0);
        glPushMatrix();
            glScalef(1.5, 0.4, 0.5);
            glutWireCube(1.0);
        glPopMatrix();
    glPopMatrix();
    
    //lengan
    glPushMatrix();
        glTranslatef(-7.3, 0.0, 0.9);
        glTranslatef(1.0, 0.0, 0.0);
        glPushMatrix();
            glScalef(5.0, 0.7, 1.8);
            glutWireCube(1.0);
        glPopMatrix();

    glPopMatrix();

    glutSwapBuffers();

}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isDragging = true;
            lastMouseX = x;
            lastMouseY = y;
        } else {
            isDragging = false;
        }
    }
}

void motion(int x, int y) {
    if (isDragging) {
        camRotationY += (x - lastMouseX) * 0.5f;
        camRotationX += (y - lastMouseY) * 0.5f;
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
		case '1': jari = 0; break;
        case '2': jari = 1; break;
        case '3': jari = 2; break;
        case '4': jari = 3; break;
        case '5': jari = 4; break;
        
        // roll
        case '7': camRotationZ -= 5.0f; break;
        case '8': camRotationZ += 5.0f; break;

        // kontrol s jari yang dipilih
        case 's': 
            s[jari] = (s[jari] + 5) % 360; 
            break;
        case 'S': 
            s[jari] = (s[jari] - 5) % 360; 
            break;

        // kontrol e jari yang dipilh
        case 'e': 
            e[jari] = (e[jari] + 5) % 360; 
            break;
        case 'E': 
            e[jari] = (e[jari] - 5) % 360; 
            break;
            
        //emote
        case 'z' :
        	s[0] = 105;
			s[1] = 100;
			s[2] = 95;
			s[3] = 5;
			s[4] = 100;
			e[0] = 100;
			e[1] = 100;
			e[2] = 100;
			e[3] = 5;
			e[4] = 100;
			break;
		
		case 'x' :
        	s[0] = 5;
			s[1] = 100;
			s[2] = 95;
			s[3] = 5;
			s[4] = 100;
			e[0] = 5;
			e[1] = 100;
			e[2] = 100;
			e[3] = 5;
			e[4] = 100;
			break;
			
		case 'c' :
        	s[0] = 105;
			s[1] = 100;
			s[2] = 95;
			s[3] = 90;
			s[4] = 100;
			e[0] = 100;
			e[1] = 100;
			e[2] = 100;
			e[3] = 100;
			e[4] = 100;
			break;
			
		case 'v' :
        	s[0] = 0;
			s[1] = 0;
			s[2] = 0;
			s[3] = 0;
			s[4] = 0;
			e[0] = 0;
			e[1] = 0;
			e[2] = 0;
			e[3] = 0;
			e[4] = 0;
			break;
		            
        // zoom
        case '+': zoom += 0.5f; break; 
        case '-': zoom -= 0.5f; break;
        case 27: exit(0); break;
        default: break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:    camY += 0.1f; break; // ke atas
        case GLUT_KEY_DOWN:  camY -= 0.1f; break; // ke bawah
        case GLUT_KEY_LEFT:  camX -= 0.1f; break; // ke kiri
        case GLUT_KEY_RIGHT: camX += 0.1f; break; // ke kanan
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(700, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Dzaki Fathul 'Alim Cahyo");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    
    //arrow
    glutSpecialFunc(specialKeys);
    
    //mouse
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    
    glutMainLoop();
    return 0;
}
