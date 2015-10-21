//
//  Created by Marco Ramirez and Ricardo Canales on 10/18/15.
//

#include <iostream>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdlib>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;

int screenWidth = 720, screenHeight = 640, gameZoneHeight = screenHeight * 0.8, textZoneHeight = screenHeight * 0.2;
double cardWidth = screenWidth/4.0, cardHeight = gameZoneHeight/4.0;
int timer = 0, seconds = 0, minutes = 0, delta = 1, turns = 0;
vector<int> num;
vector<int> states; // 0 == hidden, 1 == selected, 2 == revealed
vector<bool> hover;

bool stop = true;
bool inProgress = false;
bool win = false;
bool help = false;

string minutesStr, secondsStr, milisecondsStr;

int randomGen (int i) { return rand()%i;}

void randomCards() {
    //cout << rand() << endl;
    num.clear();
    states = vector<int>(16, 0);
    for (int i = 0; i < 8; ++i) {
        num.push_back(i);
        num.push_back(i);
    }
    
    
    random_shuffle(num.begin(), num.end(), randomGen);
    
    /*
     cout << "vector contains: ";
     for (vector<int>::iterator it=num.begin(); it!=num.end(); ++it)
     cout << ' ' << *it;
     cout << endl; */
}

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
    delta = 1;
    if (!stop) {
        timer += delta;
    }
    glutPostRedisplay();
    glutTimerFunc(100, myTimer,1);
}

void drawCardNum(string text,int x,int y, float size) {
    glPushMatrix();
    glTranslatef(x, y, 0.0);
    float scale = size * ((float)screenWidth / (720.0 * 2.0));
    glScalef(scale, -scale, 1.0);
    for (int c=0; c < text.length(); c++) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN , text[c]);
    }
    glPopMatrix();
}

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
    gluOrtho2D(0,w,h,0);
    screenHeight = h;
    screenWidth = w;
    if(screenHeight < 350 || screenWidth < 600) gameZoneHeight = screenHeight;
    else gameZoneHeight = screenHeight * 0.8;
    cardWidth = screenWidth/4.0;
    cardHeight = gameZoneHeight/4.0;
    glutPostRedisplay();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void clearSelected(){
    for(int i = 0; i < states.size();i++){
        if(states[i] == 1){
            states[i] = 0;
        }
    }
}

void checkPairs(){
    // checa si son pares
    int selected = -1;
    bool success = false;
    for(int i = 0; i < states.size();i++){
        if(states[i] == 1 && selected == -1){
            selected = i;
        } else if(states[i] == 1 && num[i] == num[selected]){
            states[i] = 2;
            states[selected] = 2;
            success = true;
        }
    }
    if(success){
        clearSelected();
    }
    
}

void display() {
    
    GLubyte sym[] = {
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x08,0x00,0x00,0x08,
        0x09,0xFF,0xFF,0xC8,
        0x08,0x00,0x00,0x08,
        0x09,0x00,0x80,0x48,
        0x08,0x80,0x80,0x88,
        0x08,0x40,0x81,0x08,
        0x08,0x20,0x82,0x08,
        0x08,0x10,0x84,0x08,
        0x08,0x08,0x88,0x08,
        0x08,0x04,0x90,0x08,
        0x08,0x02,0x20,0x08,
        0x08,0x01,0x40,0x08,
        0x08,0x00,0x80,0x08,
        0x08,0x01,0x40,0x08,
        0x08,0x02,0x20,0x08,
        0x08,0x04,0x90,0x08,
        0x08,0x08,0x88,0x08,
        0x08,0x10,0x84,0x08,
        0x08,0x20,0x82,0x08,
        0x08,0x40,0x81,0x08,
        0x08,0x80,0x80,0x88,
        0x09,0x00,0x80,0x48,
        0x08,0x80,0x80,0x88,
        0x08,0x40,0x01,0x08,
        0x09,0xFF,0xFF,0xC8,
        0x08,0x00,0x00,0x08,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
    };
    GLubyte itc[] = {
        
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x1E,0x07,0xC1,0xFC,
        0x1E,0x07,0xC1,0xFC,
        0x1E,0x07,0xC1,0xFC,
        0x1E,0x07,0xC1,0xFC,
        0x1E,0x07,0xC1,0xFC,
        0x1E,0x07,0xC1,0xFC,
        0x1E,0x07,0xC1,0xE0,
        0x1E,0x07,0xC1,0xE0,
        0x1E,0x07,0xC1,0xE0,
        0x1E,0x07,0xC1,0xE0,
        0x1E,0x07,0xC1,0xE0,
        0x1E,0x07,0xC1,0xE0,
        0x1E,0x07,0xC1,0xE0,
        0x1E,0x07,0xC1,0xE0,
        0x1E,0x07,0xC1,0xE0,
        0x1E,0x07,0xC1,0xE0,
        0x1E,0x07,0xC1,0xE0,
        0x1E,0x07,0xC1,0xE0,
        0x1E,0x07,0xC1,0xE0,
        0x1E,0x07,0xC1,0xE0,
        0x1E,0x07,0xC1,0xE0,
        0x1E,0x07,0xC1,0xE0,
        0x1E,0x3F,0xF9,0xE0,
        0x1E,0x3F,0xF9,0xFC,
        0x1E,0x3F,0xF9,0xFC,
        0x1E,0x3F,0xF9,0xFC,
        0x1E,0x3F,0xF9,0xFC,
        0x1E,0x3F,0xF9,0xFC,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
 					  
        
    };
    
    //BKG Color
    glClearColor(0,0,1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    checkPairs();
    
    //Imprime Timer
    getTime();
    
    string turno = to_string(turns/2);
    drawText("Turno: " + turno,screenWidth * 0.3,screenHeight * 0.865);
    
    //Instrucciones
    drawText("'I' :Iniciar ",screenWidth * 0.1,screenHeight * 0.97);
    drawText("'P' :Pausa",screenWidth * 0.205,screenHeight * 0.97);
    drawText("'R' :Reiniciar",screenWidth * 0.3202,screenHeight * 0.97);
    drawText("'Esc' :Salir",screenWidth * 0.4601,screenHeight * 0.97);
    
    //Autor
    drawText("Autores: Marco Ramirez : A01191344 y Ricardo Canales : A01191463",screenWidth * 0.1,screenHeight * 0.92);
    
    // pinta Rectangulos
    double x1 = 0, x2 = cardWidth;
    double y1 = 0, y2 = cardHeight;
    bool colorFirst = true;
    int currentRectangle = 0;
    
    //Activar un patron de relleno
    
    
    for(int y = 0; y < 4; y++){
        x1 = 0;
        x2 = cardWidth;
        
        for (int x = 0; x < 2; x++) {
            if(states[currentRectangle] == 1){
                glColor3f(0.7f, 0.0f, 0.7f);
            } else if(states[currentRectangle] == 2) {
                glColor3f(0.40f, 0.60f, 0.80f);
            } else if(hover[currentRectangle]){
                glColor3f(0.3f, 0.8f, 0.8f);
            } else {
                if(colorFirst) glColor3f(0.0f, 0.3f, 0.5f);
                else glColor3f(0.5f, 0.0f, 0.3f);
            }
            glRectf(x1,y1, x2, y2);
            
            if(states[currentRectangle] == 0){
                glEnable (GL_POLYGON_STIPPLE);
                glColor3f (0.0, 0.0, 0.0);
                glPolygonStipple (sym);
                glRectf(x1,y1, x2, y2);
                glDisable (GL_POLYGON_STIPPLE);
            }
            currentRectangle++;
            
            
            
            
            x1 += cardWidth;
            x2 += cardWidth;
            if(states[currentRectangle] == 1){
                glColor3f(0.7f, 0.0f, 0.7f);
            } else if(states[currentRectangle] == 2){
                glColor3f(0.40f, 0.60f, 0.80f);
            } else if(hover[currentRectangle]){
                glColor3f(0.3f, 0.8f, 0.8f);
            } else {
                if(colorFirst) glColor3f(0.5f, 0.0f, 0.3f);
                else glColor3f(0.0f, 0.3f, 0.5f);
            }
            glRectf(x1,y1,x2,y2);
            if(states[currentRectangle] == 0){
                glEnable (GL_POLYGON_STIPPLE);
                glColor3f (0.0, 0.0, 0.0);
                glPolygonStipple (itc);
                glRectf(x1,y1, x2, y2);
                glDisable (GL_POLYGON_STIPPLE);
            }
            
            currentRectangle++;
            
            
            x1 += cardWidth;
            x2 += cardWidth;
        }
        colorFirst = !colorFirst;
        y1 += cardHeight;
        y2 += cardHeight;
    }
    
    glColor3f(1,1,1);
    
    // pinta numeros
    string carta;
    int y=0;
    for(int i = 0; i < num.size(); i++){
        carta = to_string(num[i]);
        if(i && i%4 == 0) y++;
        if(states[i] != 0)
            drawCardNum(carta, (cardWidth*0.4 + (i%4 * cardWidth)), (cardHeight*0.7 + (y*cardHeight)), 1.0 );
    }
    
    if(inProgress){
        
        // checa si esta ganado
        win = true;
        inProgress = false;
        for(int i = 0; i < states.size(); i++){
            if(states[i] != 2){
                win = false;
                inProgress = true;
                break;
            }
        }
        if(win) {
            stop = true;
            cout << "GAME WON!" << endl;
        }
        if (help && !stop) {
            string carta;
            int y=0;
            for(int i = 0; i < num.size(); i++){
                carta = to_string(num[i]);
                if(i && i%4 == 0) y++;
                drawCardNum(carta, (cardWidth*0.9 + (i%4 * cardWidth)), (cardHeight*0.9 + (y*cardHeight)), 0.3 );
            }
        }
    }
    
    if(win) {
        glColor3f(0,0,1);
        glRectf(screenWidth * 0.25, screenHeight * 0.65,screenWidth * 0.75, screenHeight * 0.55);
        glColor3f(1,1,1);
        drawText("You won in " + to_string(turns/2) + " turns and in " + minutesStr + ":" + secondsStr + "." + milisecondsStr + "!", screenWidth * 0.3, screenHeight * 0.6);
    }
    
    //Intercambia los frame buffers
    glutSwapBuffers();//ya tiene integrado el glFlush
}

void onMenu(int opcion) {
    switch(opcion) {
            //Iniciar
        case 1:
            if(inProgress == false){
                randomCards();
                inProgress = true;
            }
            stop = false;
            break;
            
            //Reiniciar
        case 2:
            stop = true;
            timer = 0;
            turns = 0;
            win = false;
            randomCards();
            inProgress = false;
            glClear( GL_COLOR_BUFFER_BIT );
            glFlush();// Limpia la pantalla
            break;
            
            //Pausa
        case 3:
            stop = true;
            break;
            
            //Salir
        case 4:
            exit(-1);
            break;
            
            //Ayuda
        case 5:
            //Display/Hide Card Nums
            if(inProgress){
                help = !help;
            }
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
    glutAddMenuEntry("Ayuda", 5);
    glutAddSubMenu("Autores", autores);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void myMouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        //Si el usuario oprime el boton izq del mouse
        int selected;
        if (button == GLUT_LEFT_BUTTON && stop != true) {
            if (y < gameZoneHeight && y >= 0) {
                if(turns%2 == 0){
                    clearSelected();
                }
                selected = ((x / (double)screenWidth) * 4.0) + ((int)((y / (double)gameZoneHeight) * 4.0)) * 4;
                if(states[selected] != 2) {
                    states[selected] = 1;
                    turns++;
                }
            }
        }
    }
}

void myMotion(int x, int y){
    for(int i = 0; i < hover.size(); i++){ hover[i] = false; }
    if (y < gameZoneHeight && y >= 0) {
        int selected = ((x / (double)screenWidth) * 4.0) + ((int)((y / (double)gameZoneHeight) * 4.0)) * 4;
        if(states[selected] != 2) {
            hover[selected] = true;
        }
    }
}

void myKeyboard(unsigned char theKey, int mouseX, int mouseY) {
    switch (theKey) {
            //Inicio
        case 'i':
        case 'I':
            if(inProgress == false){
                randomCards();
                inProgress = true;
            }
            stop = false;
            break;
            
            //Stop
        case 'p':
        case 'P':
            stop = true;
            break;
            
            //Ayuda
        case 'a':
        case 'A':
            if(inProgress){
                help = !help;
            }
            break;
            
            //Reset
        case 'R':
        case 'r':
            stop = true;
            timer = 0;
            turns = 0;
            win = false;
            randomCards();
            inProgress = false;
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
    hover = vector<bool>(16, false);
    srand(time(0));
    glutInit(&argc, argv);
    glutInitWindowSize(720,640);
    glutInitWindowPosition(100,100);
    //Double frame buffer
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE );
    glutCreateWindow("Memorama");
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(100, myTimer,1);
    glutKeyboardFunc(myKeyboard);
    glutMouseFunc(myMouse);
    glutPassiveMotionFunc(myMotion);
    randomCards();
    crearMenu();
    glutMainLoop();
    return EXIT_SUCCESS;
}