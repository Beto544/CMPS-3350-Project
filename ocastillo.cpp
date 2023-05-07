// Oscar Castillo
#include "hpascual.h"
#include <ctime>
#include "global.h"
extern Bullet *b;
extern bool boxHit;

class Terrain {
public:
    float w, h;
    float pos[10];
    float radius;
    unsigned char color[3];
    void set_color(unsigned char col[3]) {
        memcpy(color, col, sizeof(unsigned char) * 3);
    }
    Terrain();
    Terrain(int wid, int hgt, int x, int y);
} terrain[3];

float tx, ty;

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
    glVertex2f(10.0f, -100.0f);
    glVertex2f(-10.0f, 100.0f);
    glVertex2f(10.0f, 100.0f);
    glVertex2f(10.0f, -100.0f);
    glEnd();
    glPopMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
}

Terrain::Terrain() 
{
    w = 0.0f;
    h = 0.0f;
    pos[0] = (gl.xres/2) - 200;
    pos[1] = (gl.yres/4) - 55;
    radius = 50;
}

Terrain::Terrain(int wid, int hgt, int x, int y) 
{
    w = wid;
    h = hgt;
    pos[0] = x;
    pos[1] = y;
}

void renderTerrain()
{   
    for (int i = 0; i < 3; i++) {
        for (int j = 300; j < 800; j+= 200) {
            glPushMatrix();
            glColor3ubv(terrain[i].color);
            glTranslatef(terrain[i].pos[j], terrain[i].pos[j + 100], 0.0f);
            glBegin(GL_POLYGON);
            glVertex2f(-terrain[i].w, -terrain[i].h);
            glVertex2f(-terrain[i].w, -terrain[i].h);
            glVertex2f(terrain[i].w, -terrain[i].h);
            glVertex2f(terrain[i].w, -terrain[i].h);
            glVertex2f(terrain[i].w, -terrain[i].h);
            glVertex2f(terrain[i].w, -terrain[i].h);
            glVertex2f(terrain[i].w, -terrain[i].h);
            glVertex2f(terrain[i].w, terrain[i].h);
            glVertex2f(-terrain[i].w, -terrain[i].h);
            glVertex2f(-terrain[i].w, -terrain[i].h);
            glEnd();
            glPopMatrix();
            j++;
        }
    }
}

void checkCollison(Bullet *b, int i)
{
    for (int j = 0; j < 3; j++) {
        if (b->pos[0] > terrain[j].pos[0] - terrain[j].w / 2 - 4.0f &&
                b->pos[0] < terrain[j].pos[0] + terrain[j].w / 2 + 4.0f &&
                b->pos[1] > terrain[j].pos[1] - terrain[j].h / 2 - 4.0f &&
                b->pos[1] < terrain[j].pos[1] + terrain[j].h / 2 + 4.0f) {
            boxHit = true;
            tx = b->pos[0];
            ty = b->pos[1];
            memcpy(&g.barr[i], &g.barr[g.nbullets - 1],
                    sizeof(Bullet));
            g.nbullets--;
            // damageBox(b);
        }
    }
}
void drawHills()
{
    for (float j = 325; j < 800; j+=200) {
        renderHill(j);
        renderLand(j);
    }
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
