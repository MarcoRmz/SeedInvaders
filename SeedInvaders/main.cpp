//
//  Created by Marco Ramirez and Ricardo Canales on 10/18/15.
//  Game Seed Invaders
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
#include "imageloader.h"

using namespace std;

enum Status { STOPPED, STARTED, WON, LOST, PAUSED, INSTRUCTIONS };
Status gameStatus = STOPPED;
static GLuint texName[9];

float mat[4];


void drawText(string text,int x,int y, double size);

int screenWidth = 720, screenHeight = 640, gameZoneHeight = screenHeight * 0.9, textZoneHeight = screenHeight * 0.9;
int timer = 0, seconds = 0, minutes = 0, delta = 1, levels = 0, lives = 3, score = 0;
double angle = 0;
double bgLocation = 0;
double bgAccel = 1;
bool down = true;
double enemyInterval = 300.0, enemySpawnrate = 300.0;
double maxSpeed = 4.0;
bool speedCheck = false, spawnrateCheck = false;
int powerupStatus = 0;
int powerupTime = 0;
int levelCounter = 1;
string powerupTitle = "";

int imageTip = 5;

long long timerMS = 0;
double playerPositionX = screenWidth/2.0;

int playerTop = 28;

bool playerLeft = false, playerRight = false;
string minutesStr, secondsStr, milisecondsStr;

#include "invader.h"

vector<Invader> invaders;
vector<Invader> kills;
vector<Invader> hits;

//Directory Path
string fullPath = __FILE__;

//Sound load
Sound sonido = Sound(fullPath, "Lateralus.wav");


//Gets Directory Path
void getParentPath(){
    for (int i = fullPath.length() - 1; i>=0 && fullPath[i] != '/'; i--) {
        fullPath.erase(i, 1);
    }
}

//Makes the image into a texture, and returns the id of the texture
void loadTexture(Image* image,int k)
{
    
    glBindTexture(GL_TEXTURE_2D, texName[k]); //Tell OpenGL which texture to edit
    
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    
    //Map the image to the texture
    
    glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
                 0,                            //0 for now
                 GL_RGB,                       //Format OpenGL uses for image
                 image->width, image->height,  //Width and height
                 0,                            //The border of the image
                 GL_RGB, //GL_RGB, because pixels are stored in RGB format
                 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
                 //as unsigned numbers
                 image->pixels);               //The actual pixel data
    
}

void initRendering()
{
    int i=0;
    
    glGenTextures(9, texName); //Make room for our texture
    Image* image;
    
    image = loadBMP("/Imagenes/bkg1.bmp");
    loadTexture(image,i++);
    
    image = loadBMP("/Imagenes/menu.bmp");
    loadTexture(image,i++);
    
    image = loadBMP("/Imagenes/pausa.bmp");
    loadTexture(image,i++);

    image = loadBMP("/Imagenes/instrucciones.bmp");
    loadTexture(image,i++);

    image = loadBMP("/Imagenes/go1.bmp");
    loadTexture(image,i++);
    
    image = loadBMP("/Imagenes/go2.bmp");
    loadTexture(image,i++);
    
    image = loadBMP("/Imagenes/go3.bmp");
    loadTexture(image,i++);

    image = loadBMP("/Imagenes/go4.bmp");
    loadTexture(image,i++);
    
    image = loadBMP("/Imagenes/go5.bmp");
    loadTexture(image,i++);
    
    delete image;
}

void handleResize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, -6, 0, 0, 0, 0, 1, 0);
    //glTranslatef(0.0f, 1.0f, -6.0f);
}

void  initLight(void) {
    initRendering();
    
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
            bgLocation+=bgAccel;
        } else {
            bgLocation-=bgAccel;
        }
        

        //cout << powerupTime << endl;
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
            if(invaders[i].invaderHeight >= textZoneHeight-150 && playerPositionX-7-playerTop <= invaders[i].invaderX && invaders[i].invaderX <= playerPositionX+7+playerTop){
                kills.push_back(invaders[i]);
                if(invaders[i].type == 10){
                    powerupStatus = 1;
                    powerupTime = 500;
                    powerupTitle = "Rapidez";
                } else if(invaders[i].type == 9){
                    lives++;
                } else if(invaders[i].type == 8){
                    powerupStatus = 2;
                    powerupTime = 500;
                    powerupTitle = "Grande";
                }
                
                invaders.erase(invaders.begin()+i);
                score += 10;

            }
            
            // HIT PLAYER
            if (invaders[i].invaderHeight >= textZoneHeight-65) {
                hits.push_back(invaders[i]);
                if (invaders[i].type != 10 && invaders[i].type != 9 && invaders[i].type != 8) {
                    lives--;
                }
                invaders.erase(invaders.begin()+i);
            }
        }
        
        if (lives <= 0 && gameStatus != LOST) {
            gameStatus = LOST;
            imageTip = rand() % (9) + 5;
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
    
    //Matriz de materiales
    
    if (gameStatus == STARTED) {
        //Display Game Screen
        
        //Dibuja Background
        //Imagen
        glPushMatrix();
        glTranslated(0, screenHeight + bgLocation, -100);
        glScaled(1, -1, 1);
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D, texName[0]);
        glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(screenWidth, 0.0f, 0);
        
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0);
        
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(0.0f, screenHeight*2.0, 0);
        
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(screenWidth, screenHeight*2.0, 0);
        glEnd();
        
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
        
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
            invaders[i].paint();
        }
        glDisable(GL_LIGHTING);
        
        double speedMeter = 1.0;
        playerTop = 28;
        if(powerupStatus == 1){
            speedMeter = 1.5;
        } else if(powerupStatus == 2){
            playerTop = 70;
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
        glTranslatef(playerPositionX, screenHeight * 0.85, -40.0);
        glRotatef(260.0, 1.0, 0.0, 0.0);
        gluQuadricDrawStyle(hero, GLU_FILL);
        gluSphere(hero, 21, 14, 4);
        glPopMatrix();
        //Cylinder
        glPushMatrix();
        glTranslatef(playerPositionX, screenHeight * 0.75, -30.0);
        glRotatef(260.0, 1.0, 0.0, 0.0);
        gluQuadricDrawStyle(hero, GLU_FILL);
        gluCylinder(hero, playerTop, 17, 63, 14, 4);
        glPopMatrix();
        glPopMatrix();
        glDisable(GL_LIGHTING);
    } else if (gameStatus == STOPPED) {
        //Game stopped Display MAIN
        //BKG Color
        glClearColor(0.8588235294, 0.3450980392, 0.1725490196,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Imagen
        glPushMatrix();
        glTranslated(0, screenHeight, -100);
        glScaled(1, -1, 1);
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D, texName[1]);
        glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(screenWidth, 0.0f, 0);
        
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0);
        
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(0.0f, screenHeight, 0);
        
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(screenWidth, screenHeight, 0);
        glEnd();
        
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
        
        /*
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
        */
        //Stop music
        sonido.PauseSound();
    } else if (gameStatus == PAUSED) {
        //Display Game Paused
        //Imagen
        glPushMatrix();
        glTranslated(0, screenHeight, -100);
        glScaled(1, -1, 1);
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D, texName[2]);
        glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(screenWidth, 0.0f, 0);
        
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0);
        
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(0.0f, screenHeight, 0);
        
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(screenWidth, screenHeight, 0);
        glEnd();
        
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
        
        /*
        glColor3f(1,1,1);
        drawText("Pausa", (screenWidth/2) - 50, screenHeight * 0.5, 0.25);
        
        glColor3f(0.1803921569,0.862745098,0.6901960784);
        glRectf(screenWidth * 0.37, screenHeight * 0.44,screenWidth * 0.61, screenHeight * 0.52);
        */
    } else if (gameStatus == WON) {
        //Display Game Won
        glColor3f(1,1,1);
        drawText("Felicidades ganaste con " + to_string(score) + " en " + minutesStr + ":" + secondsStr + "." + milisecondsStr + "!", screenWidth * 0.3, screenHeight * 0.6, 0.12);
        
        glColor3f(0.3490196078,0.7490196078,0.2156862745);
        glRectf(screenWidth * 0.25, screenHeight * 0.65,screenWidth * 0.75, screenHeight * 0.55);
        
        //Stop music
        sonido.PauseSound();
    } else if (gameStatus == LOST) {
        //Display Game Lost
        //Imagen
        glPushMatrix();
        glTranslated(0, screenHeight, -100);
        glScaled(1, -1, 1);
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D, texName[imageTip]);
        glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(screenWidth, 0.0f, 0);
        
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0);
        
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(0.0f, screenHeight, 0);
        
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(screenWidth, screenHeight, 0);
        glEnd();
        
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        glColor3f(1,0,0);
        drawText("LLegaste al nivel: " + to_string(levels) + " con: " + to_string(score) + " puntos y duraste " + minutesStr + ":" + secondsStr + "." + milisecondsStr + "!", screenWidth * 0.185, screenHeight * 0.25, 0.12);
        
        glColor3f(1,1,1);
        glRectf(screenWidth * 0.12, screenHeight * 0.2,screenWidth * 0.88, screenHeight * 0.27);
        
        //Stop music
        sonido.PauseSound();
    } else if (gameStatus == INSTRUCTIONS) {
        //Display Game Instructions
        //BKG Color
        glClearColor(0, 0, 0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Imagen
        glPushMatrix();
        glTranslated(0, screenHeight, -100);
        glScaled(1, -1, 1);
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D, texName[3]);
        glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(screenWidth, 0.0f, 0);
        
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0);
        
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(0.0f, screenHeight, 0);
        
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(screenWidth, screenHeight, 0);
        glEnd();
        
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
        
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
        glTranslatef(screenWidth*0.15, screenHeight * 0.88, -60.0);
        glRotatef(260.0, 1.0, 0.0, 0.0);
        gluQuadricDrawStyle(hero, GLU_FILL);
        gluSphere(hero, 21, 14, 4);
        glPopMatrix();
        //Cylinder
        glPushMatrix();
        glTranslatef(screenWidth*0.15, screenHeight * 0.78, -50.0);
        glRotatef(260.0, 1.0, 0.0, 0.0);
        gluQuadricDrawStyle(hero, GLU_FILL);
        gluCylinder(hero, 28, 17, 63, 14, 4);
        glPopMatrix();
        glPopMatrix();
        glDisable(GL_LIGHTING);
        
        //Dibuja Invader
        GLUquadricObj *invader = gluNewQuadric();
        glEnable(GL_LIGHTING);
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
        
        glPushMatrix();
        glPushMatrix();
        glTranslatef((screenWidth*0.9)-3.5, 85, -50.0);
        glRotatef(0, 0.0, -0.99, 0.0);
        glRotatef(260.0, 1.0, -0.2, 0.0);
        gluQuadricDrawStyle(invader, GLU_FILL);
        gluCylinder(invader, 1, 4, 20, 8, 4);
        
        glPopMatrix();
        glPushMatrix();
        glTranslatef((screenWidth*0.9)-3.5, 125, -50.0);
        glRotatef(-10, 0.0, 0.9, 0.0);
        glRotatef(-260.0, 1.0, 0.1, 0.0);
        gluQuadricDrawStyle(invader, GLU_FILL);
        gluCylinder(invader, 7, 4, 25, 8, 4);
        glPopMatrix();
        glPushMatrix();
        glTranslatef((screenWidth*0.9)-3, 130, -50.0);
        glRotatef(260.0, 1.0, 0.0, 0.0);
        gluQuadricDrawStyle(invader, GLU_FILL);
        gluSphere(invader, 10, 5, 4);
        
        glPopMatrix();
        glPopMatrix();
        
        glDisable(GL_LIGHTING);

        //Dibuja Powerup Speed
        GLUquadricObj *speed = gluNewQuadric();
        glEnable(GL_LIGHTING);
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
        glPushMatrix();
        glPushMatrix();
        glTranslatef((screenWidth*0.55)-3, screenHeight*0.38, -50.0);
        glRotatef(10, 0.5, 1.0, 0.0);
        gluQuadricDrawStyle(speed, GLU_LINE);
        gluSphere(speed, 10*2.0, 8*2.0, 8*2.0);
        glRotatef(260.0, 1.0, 0.0, 0.0);
        gluQuadricDrawStyle(speed, GLU_FILL);
        gluSphere(speed, 5*2.0, 5*2.0, 4*2.0);
        
        glPopMatrix();
        glPopMatrix();
        
        glDisable(GL_LIGHTING);

        //Dibuja Powerup Extra life
        GLUquadricObj *life = gluNewQuadric();
        glEnable(GL_LIGHTING);
        mat[0] = 0.0;
        mat[1] = 0.25;
        mat[2] = 0.05;
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
        glPushMatrix();
        glPushMatrix();
        glTranslatef((screenWidth*0.55)-3, screenHeight*0.49, -50.0);
        glRotatef(10, 0.5, 1.0, 0.0);
        gluQuadricDrawStyle(life, GLU_LINE);
        gluSphere(life, 10*1.3, 8*1.3, 8*1.3);
        glRotatef(260.0, 1.0, 0.0, 0.0);
        gluQuadricDrawStyle(life, GLU_FILL);
        gluSphere(life, 5*1.3, 5*1.3, 4*1.3);
        
        glPopMatrix();
        glPopMatrix();
        
        glDisable(GL_LIGHTING);

        //Dibuja Powerup Grow
        GLUquadricObj *grow = gluNewQuadric();
        glEnable(GL_LIGHTING);
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
        glPushMatrix();
        glPushMatrix();
        glTranslatef((screenWidth*0.55)-3, screenHeight*0.61, -50.0);
        glRotatef(10, 0.5, 1.0, 0.0);
        gluQuadricDrawStyle(grow, GLU_LINE);
        gluSphere(grow, 10*1.65, 8*1.65, 8*1.65);
        glRotatef(260.0, 1.0, 0.0, 0.0);
        gluQuadricDrawStyle(grow, GLU_FILL);
        gluSphere(grow, 5*1.65, 5*1.65, 4*1.65);
        
        glPopMatrix();
        glPopMatrix();
        
        glDisable(GL_LIGHTING);
        
        /*
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
        */
        //Stop music
        sonido.PauseSound();
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
                bgAccel = 1;
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
                bgAccel = 3;
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
    getParentPath();
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

#include <assert.h>
#include <fstream>

#include "imageloader.h"

using namespace std;

Image::Image(char* ps, int w, int h) : pixels(ps), width(w), height(h)
{
    
}

Image::~Image()
{
    delete[] pixels;
}

namespace
{
    //Converts a four-character array to an integer, using little-endian form
    int toInt(const char* bytes)
    {
        return (int)(((unsigned char)bytes[3] << 24) |
                     ((unsigned char)bytes[2] << 16) |
                     ((unsigned char)bytes[1] << 8) |
                     (unsigned char)bytes[0]);
    }
    
    //Converts a two-character array to a short, using little-endian form
    short toShort(const char* bytes)
    {
        return (short)(((unsigned char)bytes[1] << 8) |
                       (unsigned char)bytes[0]);
    }
    
    //Reads the next four bytes as an integer, using little-endian form
    int readInt(ifstream &input)
    {
        char buffer[4];
        input.read(buffer, 4);
        return toInt(buffer);
    }
    
    //Reads the next two bytes as a short, using little-endian form
    short readShort(ifstream &input)
    {
        char buffer[2];
        input.read(buffer, 2);
        return toShort(buffer);
    }
    
    //Just like auto_ptr, but for arrays
    template<class T>
    class auto_array
    {
    private:
        T* array;
        mutable bool isReleased;
    public:
        explicit auto_array(T* array_ = NULL) :
        array(array_), isReleased(false)
        {
        }
        
        auto_array(const auto_array<T> &aarray)
        {
            array = aarray.array;
            isReleased = aarray.isReleased;
            aarray.isReleased = true;
        }
        
        ~auto_array()
        {
            if (!isReleased && array != NULL)
            {
                delete[] array;
            }
        }
        
        T* get() const
        {
            return array;
        }
        
        T &operator*() const
        {
            return *array;
        }
        
        void operator=(const auto_array<T> &aarray)
        {
            if (!isReleased && array != NULL)
            {
                delete[] array;
            }
            array = aarray.array;
            isReleased = aarray.isReleased;
            aarray.isReleased = true;
        }
        
        T* operator->() const
        {
            return array;
        }
        
        T* release()
        {
            isReleased = true;
            return array;
        }
        
        void reset(T* array_ = NULL)
        {
            if (!isReleased && array != NULL)
            {
                delete[] array;
            }
            array = array_;
        }
        
        T* operator+(int i)
        {
            return array + i;
        }
        
        T &operator[](int i)
        {
            return array[i];
        }
    };
}

Image* loadBMP(const char* filename)
{
    ifstream input;
    input.open(fullPath + filename, ifstream::binary);
    assert(!input.fail() || !"Could not find file");
    char buffer[2];
    input.read(buffer, 2);
    assert(buffer[0] == 'B' && buffer[1] == 'M' || !"Not a bitmap file");
    input.ignore(8);
    int dataOffset = readInt(input);
    
    //Read the header
    int headerSize = readInt(input);
    int width;
    int height;
    switch (headerSize)
    {
        case 40:
            //V3
            width = readInt(input);
            height = readInt(input);
            input.ignore(2);
            assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
            assert(readShort(input) == 0 || !"Image is compressed");
            break;
        case 12:
            //OS/2 V1
            width = readShort(input);
            height = readShort(input);
            input.ignore(2);
            assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
            break;
        case 64:
            //OS/2 V2
            assert(!"Can't load OS/2 V2 bitmaps");
            break;
        case 108:
            //Windows V4
            assert(!"Can't load Windows V4 bitmaps");
            break;
        case 124:
            //Windows V5
            assert(!"Can't load Windows V5 bitmaps");
            break;
        default:
            assert(!"Unknown bitmap format");
    }
    
    //Read the data
    int bytesPerRow = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
    int size = bytesPerRow * height;
    auto_array<char> pixels(new char[size]);
    input.seekg(dataOffset, ios_base::beg);
    input.read(pixels.get(), size);
    
    //Get the data into the right format
    auto_array<char> pixels2(new char[width * height * 3]);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            for (int c = 0; c < 3; c++)
            {
                pixels2[3 * (width * y + x) + c] =
                pixels[bytesPerRow * y + 3 * x + (2 - c)];
            }
        }
    }
    
    input.close();
    return new Image(pixels2.release(), width, height);
}