// Oscar Castillo
#include "hpascual.h"
#include <ctime>
#include "global.h"
extern Bullet *b;

void renderHill(float x)
{
    g.hill.color[0] = 150.0/255;
    g.hill.color[1] = 75.0/255;
    g.hill.color[2] = 0;
    glColor3fv(g.hill.color);
    glPushMatrix();
    glTranslatef(g.hill.pos[0]+x, g.hill.pos[1] + 100, g.hill.pos[2]);
    glScalef(4.0f, 4.0f, 4.0f);
    glRotatef(g.hill.angle, 0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    glShadeModel(GL_SMOOTH);
    glVertex2f(-40.0f, -50.0f);
    glVertex2f(-20.0f, -70.0f);
    glVertex2f(0.0f, -80.0f);
    glVertex2f(20.0f, -70.0f);
    glVertex2f(40.0f, -50.0f);
    glVertex2f(30.0f, -30.0f);
    glVertex2f(20.0f, -10.0f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(-20.0f, -10.0f);
    glVertex2f(-30.0f, -30.0f);
    glEnd();
    glPopMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
}

void renderLand(float x) 
{
    g.hill.color[0] = 150.0/255;
    g.hill.color[1] = 75.0/255;
    g.hill.color[2] = 0;
    glColor3fv(g.hill.color);
    glPushMatrix();
    glTranslatef(g.hill.pos[0]+x, g.hill.pos[1] + 5, g.hill.pos[2]);
    glScalef(1.0f, 1.0f, 1.0f);
    glRotatef(g.hill.angle, 0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-10.0f, -100.0f);
    glVertex2f(-10.0f, 100.0f);
    glVertex2f(10.0f, 100.0f);
    glVertex2f(10.0f, -100.0f);
    glEnd();
    glPopMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
}

void green_ship(float *color) {
    color[0] -= 0.05; 
    color[2] -= 0.05; 
    if (color[1] < 0.0)
        color[1] = 1.0; 

    if (color[2] < 0.0)
        color[2] = 0.0; 
}
