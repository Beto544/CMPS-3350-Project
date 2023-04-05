// hunberto Pascual
#include "hpascual.h"

#include <ctime>
#include <iostream>

#include "global.h"

extern int currentPlayer;
// extern bool gameOver;
extern float cannonVelocity1;
extern float cannonVelocity2;
extern GameStats game;
extern TankStats playerTank;
extern TankStats enemyTank;

TankStats::TankStats()
    : health(100), fuel(100), bullets(20), loser(false) {
}

double TankStats::getHealth() const {
    return health;
}

double TankStats::getFuel() const {
    return fuel;
}

int TankStats::getBullets() const {
    return bullets;
}

void TankStats::decreaseHealth(double amount) {
    health -= amount;
}

void TankStats::decreaseFuel(double amount) {
    fuel -= amount;
}

void TankStats::decreaseBullets(int amount) {
    bullets -= amount;
}
void TankStats::reset() {
    health = 100;
    fuel = 100;
    bullets = 10;
}
bool TankStats::getLoser() const {
    if (health <= 0) {
        // gameOver = true;
        return true;
    }
    return false;
}

GameStats::GameStats()
    : player1Wins(0), player2Wins(0), roundsPlayed(0), gameOver(false) {
}

void GameStats::increasePlayer1Wins() {
    player1Wins++;
    roundsPlayed++;
}

void GameStats::increasePlayer2Wins() {
    player2Wins++;
    roundsPlayed++;
}

int GameStats::getPlayer1Wins() const {
    return player1Wins;
}

int GameStats::getPlayer2Wins() const {
    return player2Wins;
}

int GameStats::getRoundsPlayed() const {
    return roundsPlayed;
}
bool GameStats::getGameStatus() const {
    return false;
}

void shootCannon(Ship *curr_tank) {
    // a little time between each bullet
    float cannonVelocity = 0;
    if (currentPlayer % 2 == 0) {
        cannonVelocity = cannonVelocity1;
    } else {
        cannonVelocity = cannonVelocity2;
    }
    struct timespec bt;
    clock_gettime(CLOCK_REALTIME, &bt);
    double ts = timeDiff(&g.bulletTimer, &bt);
    if (ts > 0.1) {
        timeCopy(&g.bulletTimer, &bt);
        if (g.nbullets < 1) {
            //  shoot a bullet...
            Bullet *b = &g.barr[g.nbullets];
            timeCopy(&b->time, &bt);
            b->pos[0] = curr_tank->pos[0];
            b->pos[1] = curr_tank->pos[1];
            b->vel[0] = curr_tank->vel[0];
            b->vel[1] = curr_tank->vel[1];
            // convert ship cannon angle to radians
            Flt rad = ((curr_tank->angle) / 360.0f) * PI * 2.0;
            // convert angle to a vector
            Flt xdir = cos(rad);
            Flt ydir = sin(rad);
            b->pos[0] += xdir * 20.0f;
            b->pos[1] += ydir * 20.0f;
            b->vel[0] += xdir * cannonVelocity + rnd() * 1.4;
            b->vel[1] += ydir * cannonVelocity + rnd() * 1.4;
            b->color[0] = 1.0f;
            b->color[1] = 1.0f;
            b->color[2] = 1.0f;
            g.nbullets++;
            if (currentPlayer % 2 == 0) {
                playerTank.decreaseBullets(1);
            } else {
                enemyTank.decreaseBullets(1);
            }
            currentPlayer++;
            return;
        } else {
            return;
        }
    }
}

bool gameOver() {
    if (playerTank.getLoser() == true || enemyTank.getLoser() == true) {
        return true;
    }
    return false;
}

void renderTanks() {
    // Draw Tank 1
    g.ship.color[0] = 1.0;
    g.ship.color[1] = 1.0;
    g.ship.color[2] = 0.;
    glColor3fv(g.ship.color);
    glPushMatrix();
    glTranslatef(g.ship.pos[0], g.ship.pos[1], g.ship.pos[2]);
    glScalef(0.75f, 0.75f, 0.75f);              // scale the tank 75%
    glRotatef(g.ship.angle, 0.0f, 0.0f, 0.0f);  // no TankStats rotation
    glBegin(GL_QUADS);
    // draw TankStats body
    glVertex2f(-30.0f, -15.0f);
    glVertex2f(-30.0f, 15.0f);
    glVertex2f(30.0f, 15.0f);
    glVertex2f(30.0f, -15.0f);
    // change the color of the tracks
    glColor3f(0.0f, 0.0f, 0.0f);
    // draw TankStats tracks
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
    glTranslatef(15.0f, 0.0f, 0.0f);            // Move to the position of the cannon
    glScalef(0.75f, 0.75f, 0.75f);              // scale the tank 75%
    glRotatef(g.ship.angle, 0.0f, 0.0f, 1.0f);  // Rotate the cannon
    glBegin(GL_QUADS);
    glVertex2f(15.0f, -7.5f);
    glVertex2f(15.0f, 7.5f);
    glVertex2f(60.0f, 7.5f);
    glVertex2f(45.0f, -7.5f);
    glEnd();
    glPopMatrix();
    // Draw Tank 2
    g.ship2.color[0] = 1.0;
    g.ship2.color[1] = 0;
    g.ship2.color[2] = 0.;
    glColor3fv(g.ship2.color);
    glPushMatrix();
    glTranslatef(g.ship2.pos[0], g.ship2.pos[1], g.ship2.pos[2]);
    glScalef(0.75f, 0.75f, 0.75f);               // scale the tank 75%
    glRotatef(g.ship2.angle, 0.0f, 0.0f, 0.0f);  // no TankStats rotation
    glBegin(GL_QUADS);
    // TankStats body
    glVertex2f(-30.0f, -15.0f);
    glVertex2f(-30.0f, 15.0f);
    glVertex2f(30.0f, 15.0f);
    glVertex2f(30.0f, -15.0f);
    // change the color of the tracks
    glColor3f(0.0f, 0.0f, 0.0f);
    // TankStats tracks
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
    glTranslatef(15.0f, 0.0f, 0.0f);             // Move to the position of the cannon
    glScalef(0.75f, 0.75f, 0.75f);               // scale the tank 75%
    glRotatef(g.ship2.angle, 0.0f, 0.0f, 1.0f);  // Rotate the cannon
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

void rainbow_ship(float *color) {
    color[0] -= 0.01;  // decrease red
    color[2] += 0.01;  // increase blue
    if (color[0] < 0.0)
        color[0] = 1.0;  // max red value
    if (color[2] > 1.0)
        color[2] = 0.0;  // min blue value
}
