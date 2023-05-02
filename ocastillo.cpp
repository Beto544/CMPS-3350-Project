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
    glTranslatef(g.hill.pos[0]+x, g.hill.pos[1], g.hill.pos[2]);
    glScalef(2.0f, 2.0f, 2.0f);
    glRotatef(g.hill.angle, 0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(-40.0f, -50.0f);
    glVertex2f(40.0f, 50.0f);
    glVertex2f(40.0f, -50.0f);
    glVertex2f(-40.0f, 50.0f);
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
    glTranslatef(g.hill.pos[0]+x, g.hill.pos[1], g.hill.pos[2]);
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
void renderMissile()
{
    for (int i = 0; i < g.nbullets; i++) {
        Bullet *b = &g.barr[i];
        // Log("draw bullet...\n");
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_TRIANGLES);
        glColor3f(0.8, 0.8, 0.8);
        glVertex2f(b->pos[0] - 10.0f, b->pos[1] + 5.0f);
        glVertex2f(b->pos[0] - 10.0f, b->pos[1] - 5.0f);
        glVertex2f(b->pos[0] + 10.0f, b->pos[1]);
        glEnd();
    }
}

void green_ship(float *color) {
    color[0] -= 0.05; 
    color[2] -= 0.05; 
    if (color[1] < 0.0)
        color[1] = 1.0; 

    if (color[2] < 0.0)
        color[2] = 0.0; 
}
