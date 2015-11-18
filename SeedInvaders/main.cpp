//
//  Created by Marco Ramirez and Ricardo Canales on 10/18/15.
//

#include <iostream>
#include <GLUT/glut.h>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <ctime>
#include <utility>
#include <string>
#include "Sound.h"

using namespace std;

enum Status { STOPPED, STARTED, WON, LOST, PAUSED, INSTRUCTIONS };
Status gameStatus = STOPPED;

void drawText(string text,int x,int y, double size);

int screenWidth = 720, screenHeight = 640, gameZoneHeight = screenHeight * 0.9, textZoneHeight = screenHeight * 0.9;
int timer = 0, seconds = 0, minutes = 0, delta = 1, levels = 0, lives = 3, score = 0;
double angle = 0;
double bgLocation = 0;
bool down = true;
double enemyInterval = 300.0, enemySpawnrate = 300.0;
double maxSpeed = 4.0;
bool speedCheck = false, spawnrateCheck = false;
int powerupStatus = 0;
int powerupTime = 0;
int levelCounter = 1;
string powerupTitle = "";

long long timerMS = 0;
double playerPositionX = screenWidth/2.0;
bool playerLeft = false, playerRight = false;
string minutesStr, secondsStr, milisecondsStr;

#include "invader.h"

vector<Invader> invaders;
vector<Invader> kills;
vector<Invader> hits;

Sound sonido = Sound("/Users/Marco/Documents/Code/Graficas/SeedInvaders/SeedInvaders/Lateralus.wav");

void  initLight(void) {

    // Light
    GLfloat ambient[4] ={1.0, 1.0, 1.0, 1.0};
    GLfloat diffuse[4] ={1.0, 1.0, 1.0, 1.0};
    GLfloat position[4] ={8.0, 9.0, 5.0, 1.0};
    
    GLfloat lmodel_ambient[] ={1.0, 1.0, 1.0, 1.0};
    GLfloat local_view[] ={0.0};
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
    
    glFrontFace(GL_CW);
    glEnable(GL_LIGHT0);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
}

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
    drawTime(minutesStr + ":" + secondsStr);
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

void myTimer(int i) {
    if (gameStatus == STARTED) {
        enemyInterval -= 5;
        
        // Acceleration changes
        if(!spawnrateCheck && seconds % 5 != 0) spawnrateCheck = true;
        if(!speedCheck && seconds % 15 != 0) speedCheck = true;
        if(spawnrateCheck && seconds % 5 == 0 && enemySpawnrate > 20.0) {
            enemySpawnrate -= 20.0;
            spawnrateCheck = false;
            
            //cout << "LOWERED SPAWNRATE" << endl;
        }
        if(speedCheck && seconds % 15 == 0 && maxSpeed <= 10) {
            maxSpeed+= 0.5;
            speedCheck = false;
            //cout << "INCREASED SPEED" << endl;
            levelCounter++;
            if (levelCounter % 2 == 0) {
                levels++;
                levelCounter = 1;
            }
        }
        
        if(bgLocation >= screenHeight*0.25 || bgLocation < 0){
            down = !down;
        }
        
        if(down){
            bgLocation+=1;
        } else {
            bgLocation-=1;
        }
        

        cout << powerupTime << endl;
        if(powerupStatus != 0){
           powerupTime--;
           if(powerupTime <= 0) {
            powerupStatus = 0;
            powerupTime = 0;
            powerupTitle = "";
           }
        }
        
        // cout << "Enemy INTERVAL: " << enemyInterval << endl;
        // ENEMY CREATION
        if(enemyInterval <= 0) {
            invaders.push_back(Invader());
            enemyInterval = enemySpawnrate;
        }
        for(int i = 0; i < invaders.size(); i++){
            invaders[i].invaderHeight += invaders[i].speed;
            
            // KILL INVADER
            if(invaders[i].invaderHeight >= textZoneHeight-150 && playerPositionX-35 <= invaders[i].invaderX && invaders[i].invaderX <= playerPositionX+35){
                kills.push_back(invaders[i]);
                if(invaders[i].type == 10){
                    powerupStatus = 1;
                    powerupTime = 500;
                    powerupTitle = "Rapidez";
                }
                invaders.erase(invaders.begin()+i);
                score += 10;

            }
            
            // HIT PLAYER
            if (invaders[i].invaderHeight >= textZoneHeight-65) {
                hits.push_back(invaders[i]);
                if (invaders[i].type != 10) {
                    lives--;
                }
                invaders.erase(invaders.begin()+i);
            }
        }
        
        if (lives <= 0) {
            gameStatus = LOST;
        }
        
        if(timerMS % 20 == 0) timer += 1;
        timerMS += 5;
        glutPostRedisplay();
        //cout << timerMS << endl;
    }
    
    glutTimerFunc(5, myTimer,1);
}

void reshape(int w,int h) {
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, w, h, 0.0, 0.0, 1000.0);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    glClearColor(0.9215686275, 0.5960784314, 0.4862745098,1.0);
    
    float mat[4];
    
    //Dibuja Background
    glEnable(GL_LIGHTING);
    GLUquadricObj *background = gluNewQuadric();
    glShadeModel (GL_SMOOTH);
    glPushMatrix();
    mat[0] = 1.0;
    mat[1] = 0.4;
    mat[2] = 0.4;
    mat[3] = 1.0;
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
    mat[0] = 0.4;
    mat[1] = 0.4;
    mat[2] = 0.4;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
    mat[0] = 0.0;
    mat[1] = 0.0;
    mat[2] = 0.774597;
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
    glMaterialf(GL_FRONT, GL_SHININESS, 0.8 * 128.0);
    //Cylinder
    glPushMatrix();
    glTranslatef(screenWidth*0.5,  -screenWidth + bgLocation, 0);
    glRotatef(260.0, 1.0, 0.0, 0.0);
    gluQuadricDrawStyle(background, GLU_LINE);
    gluCylinder(background, 370, 370, screenWidth*2.0, 100, 50);
    glPopMatrix();
    glPopMatrix();
    glDisable(GL_LIGHTING);
    
    //Matriz de materiales
    
    if (gameStatus == STARTED) {
        //Display Game Screen
        
        //Game Stats
        glColor3f(1, 1, 1);
        getTime();
        drawText("Vidas: " + to_string(lives) + "  Puntos: " + to_string(score) + "  Nivel: " + to_string(levels) + "  Poderes: " + powerupTitle,screenWidth * 0.16,screenHeight * 0.97, 0.15);
        
        glColor3f(0, 0, 0);
        glRectf(0,textZoneHeight, screenWidth, screenHeight);
        
        // DRAW KILLS
        glColor3f(1, 1, 0);
        for(int i = 0; i < kills.size(); i++){
            if(kills[i].time <= 0) kills.erase(kills.begin()+i);
            if (kills[i].type == 10) {
                glColor3f(0.2196078431, 1.0, 0.9725490196);
            } else {
                glColor3f(1, 1, 0);
            }
            kills[i].paintText();
        }
        
        // DRAW HITS
        glColor3f(1, 0, 0);
        for(int i = 0; i < hits.size(); i++){
            if(hits[i].time <= 0) hits.erase(hits.begin()+i);
            if (hits[i].type != 10) {
                if (timer/15 > 10) {
                    hits[i].paintText("SIDA");
                } else {
                    hits[i].paintText("PREGNANT");
                }
                
            }
        }
        
        //Dibuja Invader
        glEnable(GL_LIGHTING);
        for(int i = 0; i < invaders.size(); i++){
            if (invaders[i].type != 10) {
                mat[0] = 0.0;
                mat[1] = 0.05;
                mat[2] = 0.15;
                mat[3] = 1.0;
                glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
                mat[0] = 0.4;
                mat[1] = 0.5;
                mat[2] = 0.5;
                glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
                mat[0] = 0.04;
                mat[1] = 0.7;
                mat[2] = 0.7;
                glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
                glMaterialf(GL_FRONT, GL_SHININESS, .25 * 128.0);
            } else {
                mat[0] = 0.35;
                mat[1] = 0.0;
                mat[2] = 0.0;
                mat[3] = 1.0;
                glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
                mat[0] = 0.4;
                mat[1] = 0.5;
                mat[2] = 0.5;
                glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
                mat[0] = 0.04;
                mat[1] = 0.7;
                mat[2] = 0.7;
                glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
                glMaterialf(GL_FRONT, GL_SHININESS, .15 * 128.0);
            }
            invaders[i].paint();
        }
        glDisable(GL_LIGHTING);
        
        double speedMeter = 1.0;
        if(powerupStatus == 1){
            speedMeter = 1.5;
        }
        
        if(playerLeft && playerPositionX > 0) {
            playerPositionX-= 10 * speedMeter;
        }
        if(playerRight && playerPositionX < screenWidth) {
            playerPositionX+= 10 * speedMeter;
        }
        
        //Dibuja Canasta
        glEnable(GL_LIGHTING);
        GLUquadricObj *hero = gluNewQuadric();
        glShadeModel (GL_SMOOTH);
        glPushMatrix();
        mat[0] = 0.4;
        mat[1] = 0.4;
        mat[2] = 0.4;
        mat[3] = 1.0;
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
        mat[0] = 0.4;
        mat[1] = 0.4;
        mat[2] = 0.4;
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
        mat[0] = 0.774597;
        mat[1] = 0.774597;
        mat[2] = 0.774597;
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
        glMaterialf(GL_FRONT, GL_SHININESS, 0.8 * 128.0);
        //Sphere
        glPushMatrix();
        glTranslatef(playerPositionX, screenHeight * 0.85, -60.0);
        glRotatef(260.0, 1.0, 0.0, 0.0);
        gluQuadricDrawStyle(hero, GLU_FILL);
        gluSphere(hero, 21, 14, 4);
        glPopMatrix();
        //Cylinder
        glPushMatrix();
        glTranslatef(playerPositionX, screenHeight * 0.75, -50.0);
        glRotatef(260.0, 1.0, 0.0, 0.0);
        gluQuadricDrawStyle(hero, GLU_FILL);
        gluCylinder(hero, 28, 17, 63, 14, 4);
        glPopMatrix();
        glPopMatrix();
        glDisable(GL_LIGHTING);
    } else if (gameStatus == STOPPED) {
        //Game stopped Display MAIN
        //BKG Color
        glClearColor(0.8588235294, 0.3450980392, 0.1725490196,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Title BKG
        //glColor3f(0,0,1);
        //glRectf(screenWidth * 0.23, screenHeight * 0.1,screenWidth * 0.75, screenHeight * 0.25);
        //Title
        glColor3f(1,1,1);
        drawText("Seed Invaders", (screenWidth/2) - 180, screenHeight * 0.2, 0.4);
        
        //Button Texts
        drawText("Iniciar (I)", (screenWidth/2) - 80, screenHeight * 0.5, 0.25);
        drawText("Ayuda (H)", (screenWidth/2) - 85, screenHeight * 0.63, 0.25);
        
        glColor3f(0.1803921569,0.862745098,0.6901960784);
        //Play Button
        glRectf(screenWidth * 0.37, screenHeight * 0.44,screenWidth * 0.61, screenHeight * 0.52);
        //Help Button
        glRectf(screenWidth * 0.37, screenHeight * 0.57,screenWidth * 0.61, screenHeight * 0.65);
    } else if (gameStatus == PAUSED) {
        //Display Game Paused
        glColor3f(1,1,1);
        drawText("Pausa", (screenWidth/2) - 50, screenHeight * 0.5, 0.25);
        
        glColor3f(0.1803921569,0.862745098,0.6901960784);
        glRectf(screenWidth * 0.37, screenHeight * 0.44,screenWidth * 0.61, screenHeight * 0.52);
    } else if (gameStatus == WON) {
        //Display Game Won
        glColor3f(1,1,1);
        drawText("Felicidades ganaste con " + to_string(score) + " en " + minutesStr + ":" + secondsStr + "." + milisecondsStr + "!", screenWidth * 0.3, screenHeight * 0.6, 0.12);
        
        glColor3f(0.3490196078,0.7490196078,0.2156862745);
        glRectf(screenWidth * 0.25, screenHeight * 0.65,screenWidth * 0.75, screenHeight * 0.55);
    } else if (gameStatus == LOST) {
        //Display Game Lost
        glColor3f(1,1,1);
        drawText("Perdiste! LLegaste al nivel: " + to_string(levels) + " con: " + to_string(score) + " puntos y duraste " + minutesStr + ":" + secondsStr + "." + milisecondsStr + "!", screenWidth * 0.14, screenHeight * 0.5, 0.12);
        
        glColor3f(1,0,0);
        glRectf(screenWidth * 0.12, screenHeight * 0.45,screenWidth * 0.92, screenHeight * 0.52);
    } else if (gameStatus == INSTRUCTIONS) {
        //Display Game Instructions
        //BKG Color
        glClearColor(0, 0, 0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Keys
        glColor3f(0,0,1);
        drawText("Instrucciones",screenWidth * 0.32,screenHeight * 0.1, 0.35);
        drawText("Controles:",screenWidth * 0.05,screenHeight * 0.2, 0.25);
        glColor3f(1,1,1);
        drawText("Iniciar: 'I'",screenWidth * 0.05,screenHeight * 0.25, 0.15);
        drawText("Pausa: 'P'",screenWidth * 0.05,screenHeight * 0.3, 0.15);
        drawText("Reiniciar: 'R'",screenWidth * 0.05,screenHeight * 0.35, 0.15);
        drawText("Ayuda: 'H'",screenWidth * 0.05,screenHeight * 0.4, 0.15);
        drawText("Menu Principal: 'S'",screenWidth * 0.05,screenHeight * 0.45, 0.15);
        drawText("Salir: 'Esc'",screenWidth * 0.05,screenHeight * 0.5, 0.18);
        drawText("Boton Derecho del mouse",screenWidth * 0.05,screenHeight * 0.55, 0.105);
        drawText("mostrara las mismas opciones",screenWidth * 0.05,screenHeight * 0.58, 0.105);
        
        //Game Goal
        glColor3f(0,0,1);
        drawText("Meta del Juego:",screenWidth * 0.4,screenHeight * 0.2, 0.25);
        glColor3f(1,1,1);
        drawText("LoremIpsum........",screenWidth * 0.4,screenHeight * 0.25, 0.15);
        
        //Game Powerups
        glColor3f(0,0,1);
        drawText("Poderes:",screenWidth * 0.4,screenHeight * 0.35, 0.25);
        glColor3f(1,1,1);
        drawText("S = Moviemiento de Jugador más rápido",screenWidth * 0.4,screenHeight * 0.4, 0.15);
        
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
            if(gameStatus != STARTED && gameStatus != LOST){
                gameStatus = STARTED;
                //Music
                sonido.PlaySound();
            }
            glClear( GL_COLOR_BUFFER_BIT );
            glFlush();// Limpia la pantalla
            break;
            
            //Reiniciar
        case 2:
            if(gameStatus != STOPPED && gameStatus != INSTRUCTIONS){
                timer = 0;
                levels = 0;
                score = 0;
                lives = 3;
                playerPositionX = screenWidth/2.0;
                invaders.clear();
                enemyInterval = 300.0;
                enemySpawnrate = 300.0;
                powerupStatus = 0;
                playerLeft = false;
                playerRight = false;
                
                gameStatus = STARTED;
                //Music
                sonido.PlaySound();
                
                glClear( GL_COLOR_BUFFER_BIT );
                glFlush();// Limpia la pantalla
            }
            break;
            
            //Pausa
        case 3:
            if (gameStatus == STARTED) {
                gameStatus = PAUSED;
                sonido.PauseSound();
            } else if (gameStatus == PAUSED) {
                gameStatus = STARTED;
                sonido.PlaySound();
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
        
        case 6:
            //Main Menu
            gameStatus = STOPPED;
            
            timer = 0;
            levels = 0;
            score = 0;
            lives = 3;
            playerPositionX = screenWidth/2.0;
            invaders.clear();
            enemyInterval = 300.0;
            enemySpawnrate = 300.0;
            powerupStatus = 0;
            playerLeft = false;
            playerRight = false;
            
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
    glutAddMenuEntry("Menu Principal", 6);
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
            if(gameStatus != STARTED && gameStatus != LOST){
                gameStatus = STARTED;
                //Music
                sonido.PlaySound();
            }
            glClear( GL_COLOR_BUFFER_BIT );
            glFlush();// Limpia la pantalla
            break;
            
            //Pause
        case 'p':
        case 'P':
            if (gameStatus == STARTED) {
                gameStatus = PAUSED;
                sonido.PauseSound();
            } else if (gameStatus == PAUSED) {
                gameStatus = STARTED;
                sonido.PlaySound();
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
            if(gameStatus != STOPPED && gameStatus != INSTRUCTIONS){
                timer = 0;
                levels = 0;
                score = 0;
                lives = 3;
                playerPositionX = screenWidth/2.0;
                invaders.clear();
                enemyInterval = 300.0;
                enemySpawnrate = 300.0;
                powerupStatus = 0;
                playerLeft = false;
                playerRight = false;
                
                gameStatus = STARTED;
                //Music
                sonido.PlaySound();
                
                glClear( GL_COLOR_BUFFER_BIT );
                glFlush();// Limpia la pantalla
            }
            break;
        case 'U':
        case 'u':
            if(gameStatus != STOPPED && gameStatus != INSTRUCTIONS){
                timer = 0;
                levels = 0;
                score = 0;
                lives = 500;
                playerPositionX = screenWidth/2.0;
                invaders.clear();
                enemyInterval = 5.0;
                enemySpawnrate = 5.0;
                powerupStatus = 0;
                
                gameStatus = STARTED;
                
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
            
        case 'S':
        case 's':
            //Go to main screen
            gameStatus = STOPPED;
            
            timer = 0;
            levels = 0;
            score = 0;
            lives = 3;
            playerPositionX = screenWidth/2.0;
            invaders.clear();
            enemyInterval = 300.0;
            enemySpawnrate = 300.0;
            powerupStatus = 0;
            playerLeft = false;
            playerRight = false;
            
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
    initLight();
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
