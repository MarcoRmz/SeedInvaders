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

int screenWidth = 720, screenHeight = 640, gameZoneHeight = screenHeight * 0.9, textZoneHeight = screenHeight * 0.9;
int timer = 0, seconds = 0, minutes = 0, delta = 1, levels = 0, lives = 3, score = 0;
double invaderHeight = 0, angle = 0;
long long timerMS = 0;
double playerPositionX = screenWidth/2.0;
bool playerLeft = false, playerRight = false;

enum Status { STOPPED, STARTED, WON, LOST, PAUSED, INSTRUCTIONS };
Status gameStatus = STOPPED;

string minutesStr, secondsStr, milisecondsStr;

void drawTime(string pTimer) {
    glPushMatrix();
    glTranslatef(screenWidth * 0.05,screenHeight * 0.97, 0.0);
    glScalef(0.2, -0.2, 0.0);
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
    if (gameStatus == STARTED) {
        invaderHeight += 5;
        if (invaderHeight >= textZoneHeight-65) {
            invaderHeight = 0;
        }
        angle += 10;
        if (angle >= 360) {
            angle = 0;
        }
        if(timerMS % 20 == 0) timer += 1;
        timerMS += 5;
        glutPostRedisplay();
        //cout << timerMS << endl;
    }
    
    glutTimerFunc(5, myTimer,1);
}

void drawText(std::string text,int x,int y, double size) {
    glPushMatrix();
    glTranslatef(x, y, 0.0);
    glScalef(size, -size, 0.0);
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
    else gameZoneHeight = screenHeight * 0.9;
    glutPostRedisplay();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display() {
    
    //BKG Color
    glClearColor(0.75, 0.75, 0.75,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    if (gameStatus == STARTED) {
        //Display Game Screen
        glColor3f(0, 0, 1);
        glRectf(0,textZoneHeight, screenWidth, screenHeight);
        
        glColor3f(1, 1, 1);
        //Imprime Timer
        getTime();
        
        //Game Stats
        drawText("Lives: " + to_string(lives) + "  Score: " + to_string(score) + "  Level: " + to_string(levels/2) + "  Powerups:",screenWidth * 0.2,screenHeight * 0.97, 0.15);
        
        //Dibuja Invader
        GLUquadricObj *invader = gluNewQuadric();
        glColor3f(0.05, 0.67, 0.87);
        glShadeModel (GL_FLAT);
        
        glPushMatrix();
        //Invader Rotation
        //glRotatef(angle, 0.0, 1.0, 0.0);
        glPushMatrix();
        glTranslatef((screenWidth/2)-3.5, invaderHeight, -50.0);
        glRotatef(260.0, 1.0, -0.2, 0.0);
        gluQuadricDrawStyle(invader, GLU_LINE);
        gluCylinder(invader, 1, 4, 20, 8, 4);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(screenWidth/2, invaderHeight + 15, -50.0);
        glRotatef(260.0, 1.0, 0.1, 0.0);
        gluQuadricDrawStyle(invader, GLU_LINE);
        gluCylinder(invader, 4, 7, 30, 8, 4);
        glPopMatrix();
        glPushMatrix();
        glTranslatef((screenWidth/2)-3, invaderHeight + 50, -50.0);
        glRotatef(260.0, 1.0, 0.0, 0.0);
        gluQuadricDrawStyle(invader, GLU_LINE);
        gluSphere(invader, 10, 5, 4);
        glPopMatrix();
        glPopMatrix();
        
        if(playerLeft && playerPositionX > 0) playerPositionX-= 10;
        if(playerRight && playerPositionX < screenWidth) playerPositionX+= 10;
        
        //Dibuja Canasta
        GLUquadricObj *hero = gluNewQuadric();
        glColor3f(1.0, 1.0, 1.0);
        glShadeModel (GL_FLAT);
        
        glPushMatrix();
        glTranslatef(playerPositionX, screenHeight * 0.75, -50.0);
        glRotatef(260.0, 1.0, 0.0, 0.0);
        gluQuadricDrawStyle(hero, GLU_LINE);
        gluCylinder(hero, 28, 17, 63, 14, 4);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(playerPositionX, screenHeight * 0.85, -50.0);
        glRotatef(260.0, 1.0, 0.0, 0.0);
        gluQuadricDrawStyle(hero, GLU_FILL);
        gluSphere(hero, 21, 14, 4);
        glPopMatrix();
    } else if (gameStatus == STOPPED) {
        //Game stopped Display MAIN
        //BKG Color
        glClearColor(0, 0, 0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Title BKG
        //glColor3f(0,0,1);
        //glRectf(screenWidth * 0.23, screenHeight * 0.1,screenWidth * 0.75, screenHeight * 0.25);
        //Title
        glColor3f(1,1,1);
        drawText("Seed Invaders", (screenWidth/2) - 180, screenHeight * 0.2, 0.4);
        
        //Play Button
        glColor3f(1,1,1);
        glRectf(screenWidth * 0.37, screenHeight * 0.44,screenWidth * 0.61, screenHeight * 0.52);
        glColor3f(0,0,1);
        drawText("Iniciar (I)", (screenWidth/2) - 80, screenHeight * 0.5, 0.25);
        
        //Help Button
        glColor3f(1,1,1);
        glRectf(screenWidth * 0.37, screenHeight * 0.57,screenWidth * 0.61, screenHeight * 0.65);
        glColor3f(0,0,1);
        drawText("Ayuda (H)", (screenWidth/2) - 85, screenHeight * 0.63, 0.25);
    } else if (gameStatus == PAUSED) {
        //Display Game Paused
        glColor3f(0,0,1);
        glRectf(screenWidth * 0.37, screenHeight * 0.44,screenWidth * 0.61, screenHeight * 0.52);
        glColor3f(1,1,1);
        drawText("Pausa", (screenWidth/2) - 50, screenHeight * 0.5, 0.25);
    } else if (gameStatus == WON) {
        //Display Game Won
        glColor3f(0,0,1);
        glRectf(screenWidth * 0.25, screenHeight * 0.65,screenWidth * 0.75, screenHeight * 0.55);
        glColor3f(1,1,1);
        drawText("Felicidades ganaste con " + to_string(score) + " en " + minutesStr + ":" + secondsStr + "." + milisecondsStr + "!", screenWidth * 0.3, screenHeight * 0.6, 0.12);
    } else if (gameStatus == LOST) {
        //Display Game Lost
        glColor3f(0,0,1);
        glRectf(screenWidth * 0.25, screenHeight * 0.65,screenWidth * 0.75, screenHeight * 0.55);
        glColor3f(1,1,1);
        drawText("Perdiste llegaste al nivel " + to_string(levels/2) + " y duraste " + minutesStr + ":" + secondsStr + "." + milisecondsStr + "!", screenWidth * 0.3, screenHeight * 0.6, 0.12);
    } else if (gameStatus == INSTRUCTIONS) {
        //Display Game Instructions
        //BKG Color
        glClearColor(0, 0, 0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Keys
        glColor3f(1,1,1);
        drawText("Instrucciones",screenWidth * 0.32,screenHeight * 0.1, 0.35);
        drawText("Iniciar: 'I'",screenWidth * 0.05,screenHeight * 0.2, 0.2);
        drawText("Pausa: 'P'",screenWidth * 0.05,screenHeight * 0.25, 0.2);
        drawText("Reiniciar: 'R'",screenWidth * 0.05,screenHeight * 0.3, 0.2);
        drawText("Ayuda: 'H'",screenWidth * 0.05,screenHeight * 0.35, 0.2);
        drawText("Salir: 'Esc'",screenWidth * 0.05,screenHeight * 0.4, 0.2);
        drawText("Boton Derecho del mouse mostrara las mismas opciones",screenWidth * 0.05,screenHeight * 0.45, 0.1);
        
        //Autors
        drawText("Autores: Marco Ramirez : A01191344 y Ricardo Canales : A01191463",screenWidth * 0.1,screenHeight * 0.97, 0.12);
    }
    
    //Intercambia los frame buffers
    glutSwapBuffers();//ya tiene integrado el glFlush
}

void onMenu(int opcion) {
    switch(opcion) {
            //Iniciar
        case 1:
            if(gameStatus != STARTED){
                gameStatus = STARTED;
            }
            glClear( GL_COLOR_BUFFER_BIT );
            glFlush();// Limpia la pantalla
            break;
            
            //Reiniciar
        case 2:
            if(gameStatus == STARTED || gameStatus == PAUSED){
                timer = 0;
                levels = 0;
                gameStatus = STOPPED;
                playerPositionX = screenWidth/2.0;
                glClear( GL_COLOR_BUFFER_BIT );
                glFlush();// Limpia la pantalla
            }
            break;
            
            //Pausa
        case 3:
            if (gameStatus == STARTED) {
                gameStatus = PAUSED;
            } else if (gameStatus == PAUSED) {
                gameStatus = STARTED;
            }
            glClear( GL_COLOR_BUFFER_BIT );
            glFlush();// Limpia la pantalla
            break;
            
            //Salir
        case 4:
            exit(-1);
            break;
            
            //Ayuda
        case 5:
            //Display Instructions
            if (gameStatus == INSTRUCTIONS) {
                gameStatus = STOPPED;
            } else if(gameStatus != STARTED && gameStatus != PAUSED){
                gameStatus = INSTRUCTIONS;
            }
            glClear( GL_COLOR_BUFFER_BIT );
            glFlush();// Limpia la pantalla
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
    glutAddMenuEntry("Instrucciones", 5);
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
            if(gameStatus == STARTED){
                //Move Left
                playerLeft = true;
            }
        } else if (key == GLUT_KEY_RIGHT) {
            if(gameStatus == STARTED){
                //Move Right
                playerRight = true;
            }
        }
    }
}

void specialKeyUp(int key, int x, int y) {
    if (gameStatus == STARTED) {
        if (key ==  GLUT_KEY_LEFT) {
            //Stop Move left
            playerLeft = false;
        } else if (key == GLUT_KEY_RIGHT) {
            //Stop Move Right
            playerRight = false;
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
            glClear( GL_COLOR_BUFFER_BIT );
            glFlush();// Limpia la pantalla
            break;
            
            //Pause
        case 'p':
        case 'P':
            if (gameStatus == STARTED) {
                gameStatus = PAUSED;
            } else if (gameStatus == PAUSED) {
                gameStatus = STARTED;
            }
            glClear( GL_COLOR_BUFFER_BIT );
            glFlush();// Limpia la pantalla
            break;
            
            //Move Left
        case 'a':
        case 'A':
            if(gameStatus == STARTED){
                playerLeft = true;
            }
            break;
            
            //Move Right
        case 'd':
        case 'D':
            if(gameStatus == STARTED){
                playerRight = true;
            }
            break;
            
            //Reset
        case 'R':
        case 'r':
            if(gameStatus == STARTED || gameStatus == PAUSED){
                timer = 0;
                levels = 0;
                gameStatus = STOPPED;
                playerPositionX = screenWidth/2.0;
                glClear( GL_COLOR_BUFFER_BIT );
                glFlush();// Limpia la pantalla
            }
            break;
            
        case 'H':
        case 'h':
            //Display Instructions
            if (gameStatus == INSTRUCTIONS) {
                gameStatus = STOPPED;
            } else if(gameStatus != STARTED && gameStatus != PAUSED){
                gameStatus = INSTRUCTIONS;
            }
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

void myKeyboardUp(unsigned char theKey, int mouseX, int mouseY) {
    switch (theKey) {

        case 'a':
        case 'A':
            //Stop Move Left
            playerLeft = false;
            break;
            
        case 'd':
        case 'D':
            //Stop Move Right
            playerRight = false;
            break;

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
    glutKeyboardUpFunc(myKeyboardUp);
    glutSpecialUpFunc(specialKeyUp);
    glutSpecialFunc(specialKey);
    glutMouseFunc(myMouse);
    crearMenu();
    glutMainLoop();
    return EXIT_SUCCESS;
}
