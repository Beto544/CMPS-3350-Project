// Stephanie Esquivel

#include "sesquivel.h"
#include "global.h"
#include "hpascual.h"



void renderBars(int player, double stat) {
    float barPercentage = stat / 100;
    float barX;
    float barY;
    float barWidth = 100.0f;
    float barHeight = 10.0f;

    if (player == 1) {
        barX = gl.xres / 2 - 640;
        barY = gl.yres / 2 + 320;

    } else {
        barX = gl.xres / 2 + 540;
        barY = gl.yres / 2 + 320;
    }

    float renderBarWidth = barWidth * barPercentage;

    // Set the color of the health bar based on the current stat

    glColor3f(1.0f - barPercentage, barPercentage, 0.0f);

    // Draw the health bar rectangle

    glPushMatrix();
    glTranslatef(barX, barY, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(renderBarWidth, 0.0f);
    glVertex2f(renderBarWidth, barHeight);
    glVertex2f(0.0f, barHeight);
    glEnd();
    glPopMatrix();

    // Reset the color to white

    glColor3f(1.0f, 1.0f, 1.0f);
}

void renderCannonPower(float cannonVelocity, int player) {
    float cannonPower = (cannonVelocity - 4.0) / (15 - 4);
    float barX;
    float barY;
    float barWidth = 100.0f;
    float barHeight = 10.0f;

    if (player == 1) {
        barX = gl.xres / 2 - 640;
        barY = gl.yres / 2 + 250;

    } else {
        barX = gl.xres / 2 + 580;
        barY = gl.yres / 2 + 250;
    }

    float cannonBarWidth = barWidth * cannonPower;

    float red = 1.0f;

    float green = 1.0f;

    float blue = 0.0f;

    if (cannonPower < 1.0f) {
        red = cannonPower + 20;

        green = cannonPower;

        blue = 0.0f;

    } else {
        red = 1.0f;

        green = 1.0f - (cannonPower - 1.0f);

        blue = 0.0f;
    }

    glColor3f(red, green, blue);

    // Draw the power bar

    glPushMatrix();

    glTranslatef(barX, barY, 0.0f);

    glBegin(GL_QUADS);

    glVertex2f(0.0f, 0.0f);

    glVertex2f(cannonBarWidth, 0.0f);

    glVertex2f(cannonBarWidth, barHeight);

    glVertex2f(0.0f, barHeight);

    glEnd();

    glPopMatrix();

    // Reset the color to white

    glColor3f(1.0f, 1.0f, 1.0f);
}
