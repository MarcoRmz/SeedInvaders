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
    
    
    Invader(){ //Random Generate
        invaderX = rand() % (screenWidth - 20) + 20;
        invaderHeight = -40;
        type = rand() % (10) + 1;
        speed = fRand(2.0, maxSpeed);
        time = 50;
        switch (type) {
            case 10:
                text = "POWERUP";
                //0.3, 0.3, 1.0
                rgb = Color(0.1803921569,0.862745098,0.6901960784);
                break;
            default:
                text = "+10";
                rgb = Color(0.2196078431,0.5176470588,1);
                break;
        }
        
    }
    
    Invader(int x, int t, double s, Color color, string text) {
        invaderX = x;
        invaderHeight = -40;
        type = t;
        speed = s;
        rgb = color;
        time = 50;
    }
    
    void paint(){
        GLUquadricObj *invader = gluNewQuadric();
        glColor3f(rgb.r, rgb.g, rgb.b);
        glShadeModel (GL_FLAT);
        //Invader Rotation
        
        double scale = 1.0;
        switch (type) {
            case 10:
                scale = 2.0;
                break;
            default:
                scale = 1.0;
                break;
        }
        
        glPushMatrix();
        
        glPushMatrix();
        if(type!=10){


            glTranslatef(invaderX-3.5*scale, invaderHeight, -50.0);
            glRotatef(angle, 0.0, -0.99, 0.0);
            glRotatef(260.0, 1.0, -0.2, 0.0);
            gluQuadricDrawStyle(invader, GLU_LINE);
            gluCylinder(invader, 1*scale, 4*scale, 20*scale, 8*scale, 4*scale);
            
            glPopMatrix();
            glPushMatrix();
            glTranslatef(invaderX-3.5, invaderHeight + 45*scale, -50.0);
            glRotatef(-angle-10, 0.0, 0.9, 0.0);
            glRotatef(-260.0, 1.0, 0.1, 0.0);
            gluQuadricDrawStyle(invader, GLU_LINE);
            gluCylinder(invader, 7*scale, 4*scale, 25*scale, 8*scale, 4*scale);
            glPopMatrix();
            glPushMatrix();
        
        }
        glTranslatef(invaderX-3, invaderHeight + 50, -50.0);
        glRotatef(260.0, 1.0, 0.0, 0.0);
        gluQuadricDrawStyle(invader, GLU_LINE);
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
