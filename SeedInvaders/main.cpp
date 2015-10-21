//
//  Created by Marco Ramirez and Ricardo Canales on 10/18/15.
//

#include <iostream>
#include <GLUT/glut.h>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;

int screenWidth = 720, screenHeight = 640, gameZoneHeight = screenHeight * 0.8, textZoneHeight = screenHeight * 0.2;
int timer = 0, seconds = 0, minutes = 0, delta = 1, levels = 0;
double angle = 0;

enum Status { STOPPED, STARTED, WON, LOST, PAUSED };
Status gameStatus = STOPPED;

string minutesStr, secondsStr, milisecondsStr;

void drawTime(string pTimer) {
    glPushMatrix();
    glTranslatef(screenWidth * 0.1,screenHeight * 0.88, 0.0);
    glScalef(0.3, -0.3, 0.0);
    for (int x = 0; x < pTimer.length(); x++) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN , pTimer[x]);
    }
    glPopMatrix();
}

void getTime() {
    //Convierte de milisegundos
    seconds = timer / 10;
    minutes = seconds / 60;
    if (seconds == 60) {
        seconds = 0;
    }
    
    //Convierte tiempo a string
    minutesStr = to_string(minutes);
    secondsStr = to_string(seconds % 60);
    milisecondsStr = to_string(timer % 10);
    
    //Verifica si requiere 0
    if ((seconds % 60) <= 9) {
        secondsStr = "0" + secondsStr;
    }
    
    //Manda imprimir el tiempo
    drawTime(minutesStr + ":" + secondsStr + "." + milisecondsStr);
}

void myTimer(int i) {
    if (gameStatus == 1) {
        angle += 10;
    }
    glutPostRedisplay();
    glutTimerFunc(5, myTimer,1);
}

//void drawCardNum(string text,int x,int y, float size) {
//    glPushMatrix();
//    glTranslatef(x, y, 0.0);
//    float scale = size * ((float)screenWidth / (720.0 * 2.0));
//    glScalef(scale, -scale, 1.0);
//    for (int c=0; c < text.length(); c++) {
//        glutStrokeCharacter(GLUT_STROKE_ROMAN , text[c]);
//    }
//    glPopMatrix();
//}
//
void drawText(std::string text,int x,int y) {
    glPushMatrix();
    glTranslatef(x, y, 0.0);
    glScalef(0.12, -0.12, 0.0);
    //glScalef(1/screenWidth/0.01, -(1/screenHeight/0.01), 0.0);
    for (int c=0; c < text.length(); c++) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN , text[c]);
    }
    glPopMatrix();
}

void reshape(int w,int h) {
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, w, h, 0.0, 0.0, 100.0);
    screenHeight = h;
    screenWidth = w;
    if(screenHeight < 350 || screenWidth < 600) gameZoneHeight = screenHeight;
    else gameZoneHeight = screenHeight * 0.8;
    glutPostRedisplay();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display() {
    
    
    //BKG Color
    glClearColor(0,0,1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    glColor3f(0.7f, 0.7f, 0.7f);
    glRectf(0,0, screenWidth, gameZoneHeight);
    
    glColor3f(1, 1, 1);
    //Imprime Timer
    getTime();
    
    string nivel = to_string(levels/2);
    drawText("Nivel: " + nivel,screenWidth * 0.3,screenHeight * 0.865);
    
    //Instrucciones Juego
    drawText("'I' :Iniciar ",screenWidth * 0.1,screenHeight * 0.97);
    drawText("'P' :Pausa",screenWidth * 0.205,screenHeight * 0.97);
    drawText("'R' :Reiniciar",screenWidth * 0.3202,screenHeight * 0.97);
    drawText("'Esc' :Salir",screenWidth * 0.4601,screenHeight * 0.97);
    
    //Autor
    drawText("Autores: Marco Ramirez : A01191344 y Ricardo Canales : A01191463",screenWidth * 0.1,screenHeight * 0.92);
    
    //Dibuja Canasta
    GLUquadricObj *p = gluNewQuadric();
    //glPointSize(5);
    glColor3f(1.0, 1.0, 1.0);
    glShadeModel (GL_FLAT);
    
    glPushMatrix();
    glTranslatef(screenWidth/2, screenHeight * 0.65, -80.0);
    glRotatef(260.0, 1.0, 0.0, 0.0);
    gluQuadricDrawStyle(p, GLU_FILL);
    gluCylinder(p, 28, 17, 63, 14, 4);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(screenWidth/2, screenHeight * 0.75, -80.0);
    glRotatef(260.0, 1.0, 0.0, 0.0);
    gluQuadricDrawStyle(p, GLU_FILL);
    gluSphere(p, 21, 14, 4);
    glPopMatrix();
    
    if(gameStatus == WON) {
        glColor3f(0,0,1);
        glRectf(screenWidth * 0.25, screenHeight * 0.65,screenWidth * 0.75, screenHeight * 0.55);
        glColor3f(1,1,1);
        drawText("Felicidades lLegaste al nivel " + to_string(levels/2) + " y duraste " + minutesStr + ":" + secondsStr + "." + milisecondsStr + "!", screenWidth * 0.3, screenHeight * 0.6);
    }
    
    //Intercambia los frame buffers
    glutSwapBuffers();//ya tiene integrado el glFlush
}

void animate() {
    angle = 0;
}

void onMenu(int opcion) {
    switch(opcion) {
            //Iniciar
        case 1:
            if(gameStatus != STARTED){
                gameStatus = STARTED;
            }
            break;
            
            //Reiniciar
        case 2:
            timer = 0;
            levels = 0;
            gameStatus = STOPPED;
            glClear( GL_COLOR_BUFFER_BIT );
            glFlush();// Limpia la pantalla
            break;
            
            //Pausa
        case 3:
            gameStatus = PAUSED;
            break;
            
            //Salir
        case 4:
            exit(-1);
            break;
            
            //Ayuda
        case 5:
            //Display Instructions
            break;
    }
    glutPostRedisplay();
}
void crearMenu(void) {
    int autores, menuPrincipal;
    
    autores = glutCreateMenu(onMenu);
    glutAddMenuEntry("Marco Ramirez : A01191344", 0);
    glutAddMenuEntry("Ricardo Canales : A01191463", 0);
    
    menuPrincipal = glutCreateMenu(onMenu);
    glutAddMenuEntry("Iniciar", 1);
    glutAddMenuEntry("Reiniciar", 2);
    glutAddMenuEntry("Pausa", 3);
    glutAddMenuEntry("Salir", 4);
    //glutAddMenuEntry("Ayuda", 5);
    glutAddSubMenu("Autores", autores);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void myMouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        //Si el usuario oprime el boton izq del mouse
        int selected;
        if (button == GLUT_LEFT_BUTTON) {
            if (y < gameZoneHeight && y >= 0) {
            }
        }
    }
}

void specialKey(int key, int x, int y) {
    if (gameStatus == STARTED) {
        if (key ==  GLUT_KEY_LEFT) {
            //Move left
        } else if (key == GLUT_KEY_RIGHT) {
            //Move Right
        }
    }
}

void myKeyboard(unsigned char theKey, int mouseX, int mouseY) {
    switch (theKey) {
            //Inicio
        case 'i':
        case 'I':
            if(gameStatus != STARTED){
                gameStatus = STARTED;
            }
            break;
            
            //Stop
        case 'p':
        case 'P':
            gameStatus = PAUSED;
            break;
            
        case 'a':
        case 'A':
            // pending
            break;
            
            //Reset
        case 'R':
        case 'r':
            gameStatus = STOPPED;
            timer = 0;
            levels = 0;
            glClear( GL_COLOR_BUFFER_BIT );
            glFlush();// Limpia la pantalla
            break;
            
            //Salir
        case 27:
            exit(-1);
        default:
            break;
    }
}


int main(int argc, char *argv[]) {
    srand(time(0));
    glutInit(&argc, argv);
    glutInitWindowSize(screenWidth,screenHeight);
    glutInitWindowPosition(100,100);
    //Double frame buffer
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Seed Invaders");
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(5, myTimer,1);
    glutKeyboardFunc(myKeyboard);
    glutSpecialFunc(specialKey);
    glutMouseFunc(myMouse);
    crearMenu();
    glutMainLoop();
    return EXIT_SUCCESS;
}
