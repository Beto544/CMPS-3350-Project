#include "global.h"
#include <iostream>
// hunberto Pascual

void rainbow_ship(float *color)
{
    color[0] -= 0.01; // decrease red
    color[2] += 0.01; // increase blue
    if (color[0] < 0.0)
        color[0] = 1.0; // max red value

    if (color[2] > 1.0)
        color[2] = 0.0; // min blue value

}
/*
void  moveTank(Ship *curr_tank, Tank *tanky, int player)
{
		//Flt d0,d1,dist;
        if (player % 2 == 0) {
            if (gl.ship_keys[XK_a]) {
                curr_tank->pos[0] -= 0.30;//curr_tank->vel[0];
                tanky->decreaseFuel(.1);
            }
            if (gl.ship_keys[XK_d]) {
                curr_tank->pos[0] += 0.30;
                tanky->decreaseFuel(.1);

            }
             //can be used to adjust cannon power
            if (gl.ship_keys[XK_s]) {
                curr_tank->pos[1] -= curr_tank->vel[1];
            }
            if (gl.ship_keys[XK_w]) {
                curr_tank->pos[1] += curr_tank->vel[1];
            } 
        } else {
            if (gl.ship2_keys[XK_a]) {
                curr_tank->pos[0] -= 0.30;
                tanky->decreaseFuel(.1);
            }
            if (gl.ship2_keys[XK_d]) {
                curr_tank->pos[0] += 0.30;
                tanky->decreaseFuel(.1);
            }
            
            if (gl.ship2_keys[XK_Down]) {
                curr_tank->pos[1] -= curr_tank->vel[1];
            }
            if (gl.ship2_keys[XK_Up]) {
                curr_tank->pos[1] += curr_tank->vel[1];
            } 
        }

        
}
*/

void renderTanks() 
{
//Draw tank 1
        g.ship.color[0] = 1.0;
        g.ship.color[1] = 1.0;
        g.ship.color[2] = 0.;
        glColor3fv(g.ship.color);
        glPushMatrix();
        glTranslatef(g.ship.pos[0], g.ship.pos[1], g.ship.pos[2]);
        glRotatef(g.ship.angle, 0.0f, 0.0f, 0.0f); // no tank rotation
        glBegin(GL_QUADS);
        // draw Tank body
        glVertex2f(-30.0f, -15.0f);
        glVertex2f(-30.0f, 15.0f);
        glVertex2f(30.0f, 15.0f);
        glVertex2f(30.0f, -15.0f);
        // change the color of the tracks   
        glColor3f(0.0f, 0.0f, 0.0f);
        // draw Tank tracks
        glVertex2f(-33.0f, -18.0f);
        glVertex2f(-33.0f, -12.0f);
        glVertex2f(33.0f, -12.0f);
        glVertex2f(33.0f, -18.0f);
        glVertex2f(-33.0f, 23.0f);
        glVertex2f(-33.0f, 18.0f);
        glVertex2f(33.0f, 18.0f);
        glVertex2f(33.0f, 23.0f);
        glEnd();
        glPopMatrix();
        // Draw the cannon
        glPushMatrix();
        glTranslatef(g.ship.pos[0], g.ship.pos[1], g.ship.pos[2]);
        glTranslatef(15.0f, 0.0f, 0.0f); // Move to the position of the cannon
        glRotatef(g.ship.angle, 0.0f, 0.0f, 1.0f); // Rotate the cannon
        glBegin(GL_QUADS);
        glVertex2f(15.0f, -7.5f); 
        glVertex2f(15.0f, 7.5f);
        glVertex2f(60.0f, 7.5f);
        glVertex2f(45.0f, -7.5f); 
        glEnd();
        glPopMatrix();
        // Draw tank 2 
        g.ship2.color[0] = 1.0;
        g.ship2.color[1] = 0;
        g.ship2.color[2] = 0.;
        glColor3fv(g.ship2.color);
        glPushMatrix();
        glTranslatef(g.ship2.pos[0], g.ship2.pos[1], g.ship2.pos[2]);
        glRotatef(g.ship2.angle, 0.0f, 0.0f, 0.0f); // no tank rotation
        glBegin(GL_QUADS);
        // Tank body
        glVertex2f(-30.0f, -15.0f);
        glVertex2f(-30.0f, 15.0f);
        glVertex2f(30.0f, 15.0f);
        glVertex2f(30.0f, -15.0f);
        // change the color of the tracks   
        glColor3f(0.0f, 0.0f, 0.0f);
        // Tank tracks
        glVertex2f(-33.0f, -18.0f);
        glVertex2f(-33.0f, -12.0f);
        glVertex2f(33.0f, -12.0f);
        glVertex2f(33.0f, -18.0f);
        glVertex2f(-33.0f, 23.0f);
        glVertex2f(-33.0f, 18.0f);
        glVertex2f(33.0f, 18.0f);
        glVertex2f(33.0f, 23.0f);
        glEnd();
        glPopMatrix();
		// Draw the cannon
        glPushMatrix();
        glTranslatef(g.ship2.pos[0], g.ship2.pos[1], g.ship2.pos[2]);
        glTranslatef(15.0f, 0.0f, 0.0f); // Move to the position of the cannon
        glRotatef(g.ship2.angle, 0.0f, 0.0f, 1.0f); // Rotate the cannon
        glBegin(GL_QUADS);
        glVertex2f(15.0f, -7.5f);
        glVertex2f(15.0f, 7.5f);
        glVertex2f(60.0f, 7.5f);
        glVertex2f(45.0f, -7.5f);
        glEnd();
        glPopMatrix();
        // prevent background color bleed
        glColor3f(1.0f, 1.0f, 1.0f);
}
