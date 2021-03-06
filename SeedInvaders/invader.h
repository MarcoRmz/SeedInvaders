//
//  invader.h
//  SeedInvaders
//
//  Created by Ricardo Canales on 11/11/15.
//  Copyright © 2015 Marco Ramirez. All rights reserved.
//

//#ifndef invader_h
//#define invader_h
//
//
//#endif /* invader_h */

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

struct Color {
    double r;
    double g;
    double b;
    Color() : r(1), g(1), b(1) {}
    Color(double r, double g, double b) : r(r), g(g), b(b) {}
};

class Invader {
    public:
        double invaderX;
        double invaderHeight;
        int type; // 0 == invader, 1 == stretch
        double speed;
        Color rgb;
        string text;
        int time;
        double angle;
    
    
    Invader(){ //Random Generate
        invaderX = rand() % (screenWidth - 20) + 20;
        invaderHeight = -40;
        type = rand() % (10) + 1;
        speed = fRand(2.0, maxSpeed);
        time = 50;
        angle = 0;
        switch (type) {
            case 10:
                text = "RAPIDEZ";
                //0.3, 0.3, 1.0
                rgb = Color(0.35,0.0,0.0);
                break;
            case 9:
                text = "+VIDA";
                //0.3, 0.3, 1.0
                rgb = Color(0.0,0.25,0.05);
                break;
            case 8:
                text = "GRANDE";
                //0.3, 0.3, 1.0
                rgb = Color(0.4,0.4,0.4);
                break;
            default:
                text = "+10";
                rgb = Color(0.0,0.05,0.15);
                break;
        }
        
    }
    
    Invader(int x, int t, double s, Color color, string text) {
        invaderX = x;
        invaderHeight = -80;
        type = t;
        speed = s;
        rgb = color;
        time = 50;
    }
    
    void paint(){
        GLUquadricObj *invader = gluNewQuadric();
        mat[0] = rgb.r;
        mat[1] = rgb.g;
        mat[2] = rgb.b;
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
        glColor3f(rgb.r, rgb.g, rgb.b);
        glShadeModel (GL_SMOOTH);
        //Invader Rotation
        
        angle += speed*5.0;
        if (angle >= 360) {
            angle = 0;
        }
        
        double scale = 1.0;
        switch (type) {
            case 10:
                scale = 2.0;
                break;
            case 9:
                scale = 1.5;
                break;
            case 8:
                scale = 0.5;
                break;
            default:
                scale = 1.0;
                break;
        }
        glPushMatrix();
        
        glPushMatrix();
        if(type!=10 && type != 9 && type != 8){

            glTranslatef(invaderX-3.5*scale, invaderHeight + 5, -50.0);
            glRotatef(angle, 0.0, -0.99, 0.0);
            glRotatef(260.0, 1.0, -0.2, 0.0);
            gluQuadricDrawStyle(invader, GLU_FILL);
            gluCylinder(invader, 1*scale, 4*scale, 20*scale, 8*scale, 4*scale);
            
            glPopMatrix();
            glPushMatrix();
            glTranslatef(invaderX-3.5, invaderHeight + 45*scale, -50.0);
            glRotatef(-angle-10, 0.0, 0.9, 0.0);
            glRotatef(-260.0, 1.0, 0.1, 0.0);
            gluQuadricDrawStyle(invader, GLU_FILL);
            gluCylinder(invader, 7*scale, 4*scale, 25*scale, 8*scale, 4*scale);
            glPopMatrix();
            glPushMatrix();
        
        }
        glTranslatef(invaderX-3, invaderHeight + 50, -50.0);
        if(type == 10 || type == 9 || type == 8){
            glRotatef(-angle-10, 0.5, 1.0, 0.0);
            gluQuadricDrawStyle(invader, GLU_LINE);
            gluSphere(invader, 10*scale, 8*scale, 8*scale);
        }
        glRotatef(260.0, 1.0, 0.0, 0.0);
        gluQuadricDrawStyle(invader, GLU_FILL);
        if(type == 10 || type == 9 || type == 8)
            gluSphere(invader, 5*scale, 5*scale, 4*scale);
        else
            gluSphere(invader, 10*scale, 5*scale, 4*scale);

        glPopMatrix();

        
        glPopMatrix();
    }
    
    void paintText(){
        time--;
        drawText(text, invaderX - 20, invaderHeight +30, 0.15);
    }
    
    void paintText(string message){
        time--;
        drawText(message, invaderX - 20, invaderHeight +30, 0.15);
    }
    
};
