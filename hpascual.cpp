// hunberto Pascual
#include "hpascual.h"
#include <ctime>
#include <iostream>
#include "fonts.h"
#include "global.h"

extern int currentPlayer;
extern int bot;
extern float cannonVelocity1;
extern float cannonVelocity2;
extern bool tankHit;
extern bool cannonFired;
extern bool boxHit;
extern bool showControls;
extern bool newRound;
extern bool keyPressed;
extern bool downPressed;
extern Bullet *b;
extern GameStats game;
extern TankStats playerTank;
extern TankStats enemyTank;
extern void shootCannon(Tank *curr_tank);
extern Box box[1];
float bx;
float by;

TankStats::TankStats()
    : health(100), fuel(100), bullets(20), loser(false) 
{
}

double TankStats::getHealth() const 
{
    return health;
}

double TankStats::getFuel() const 
{
    return fuel;
}

int TankStats::getBullets() const 
{
    return bullets;
}

void TankStats::decreaseHealth(double amount) 
{
    health -= amount;
}

void TankStats::decreaseFuel(double amount) 
{
    fuel -= amount;
}

void TankStats::decreaseBullets(int amount) 
{
    bullets -= amount;
}

void TankStats::reset() 
{
    health = 100;
    fuel = 100;
    bullets = 20;
}

bool TankStats::getLoser() const {
    if (health <= 0) {
        return true;
    }
    return false;
}

GameStats::GameStats()
    : player1Wins(0), player2Wins(0), roundsPlayed(0), gameOver(false) 
{
}

void GameStats::increasePlayer1Wins() 
{
    player1Wins++;
    roundsPlayed++;
}

void GameStats::increasePlayer2Wins() 
{
    player2Wins++;
    roundsPlayed++;
}

int GameStats::getPlayer1Wins() const 
{
    return player1Wins;
}

int GameStats::getPlayer2Wins() const 
{
    return player2Wins;
}

int GameStats::getRoundsPlayed() const 
{
    return roundsPlayed;
}

bool GameStats::getGameStatus() const 
{
    return false;
}

Box::Box()
{
    w = 800.0f;
    h = 10.0f;
    pos[0] = (gl.xres / 2);
    pos[1] = (gl.yres / 4) - 160;
    radius = 20;
}



Box::Box(int wid, int hgt, int x, int y) 
{
    w = wid;
    h = hgt;
    pos[0] = x;
    pos[1] = y;
}

void moveTank(Tank *curr_tank) 
{
    if (currentPlayer % 2 == 0) {
        if (gl.tank_keys[XK_a]) {
            if (playerTank.getFuel() >= 0) {
                curr_tank->pos[0] -= 0.30;
                playerTank.decreaseFuel(.1);
            }
        }
        if (gl.tank_keys[XK_d]) {
            if (playerTank.getFuel() >= 0) {
                curr_tank->pos[0] += 0.30;
                playerTank.decreaseFuel(.1);
            }
        }

    } else {
        if (gl.tank2_keys[XK_a]) {
            if (enemyTank.getFuel() >= 0) {
                curr_tank->pos[0] -= 0.30;
                enemyTank.decreaseFuel(.1);
            }
        }
        if (gl.tank2_keys[XK_d]) {
            if (enemyTank.getFuel() >= 0) {
                curr_tank->pos[0] += 0.30;
                enemyTank.decreaseFuel(.1);
            }
        }
    }
    // Keep Tank within screen
    if (curr_tank->pos[0] < 0.0) {  // left side
        curr_tank->pos[0] = ((float)gl.xres - (float)gl.xres) + 10;
    } else if (curr_tank->pos[0] > (float)gl.xres) {  // right side
        curr_tank->pos[0] = ((float)gl.xres) - 10;
    } else if (curr_tank->pos[1] < 0.0) {  // bottom
        curr_tank->pos[1] = ((float)gl.yres - (float)gl.yres) + 10;
    } else if (curr_tank->pos[1] > (float)gl.yres) {  // top
        curr_tank->pos[1] = ((float)gl.yres) - 10;
    }
}

bool gameOver() 
{
    if (playerTank.getLoser() == true || enemyTank.getLoser() == true) {
        return true;
    }
    return false;
}

void renderTanks() 
{
    // Draw Tank 1
    g.tank.color[0] = 1.0;
    g.tank.color[1] = 1.0;
    g.tank.color[2] = 0.;
    glColor3fv(g.tank.color);
    glPushMatrix();
    glTranslatef(g.tank.pos[0], g.tank.pos[1], g.tank.pos[2]);
    glScalef(0.75f, 0.75f, 0.75f);              // scale the tank 75%
    glRotatef(g.tank.angle, 0.0f, 0.0f, 0.0f);  // no Tank body rotation
    glBegin(GL_QUADS);
    // draw Tank body
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
    glTranslatef(g.tank.pos[0], g.tank.pos[1], g.tank.pos[2]);
    glTranslatef(15.0f, 0.0f, 0.0f);  // Move to the position of the cannon
    glScalef(0.75f, 0.75f, 0.75f);    // scale the tank 75%
    glRotatef(g.tank.angle, 0.0f, 0, 1.0f);  // Rotate the cannon
    glBegin(GL_QUADS);
    glVertex2f(15.0f, -7.5f);
    glVertex2f(15.0f, 7.5f);
    glVertex2f(60.0f, 7.5f);
    glVertex2f(45.0f, -7.5f);
    glEnd();
    glPopMatrix();
    // Draw Tank 2
    g.tank2.color[0] = 1.0;
    g.tank2.color[1] = 0;
    g.tank2.color[2] = 0.;
    glColor3fv(g.tank2.color);
    glPushMatrix();
    glTranslatef(g.tank2.pos[0], g.tank2.pos[1], g.tank2.pos[2]);
    glScalef(0.75f, 0.75f, 0.75f);               // scale the tank 75%
    glRotatef(g.tank2.angle, 0.0f, 0.0f, 0.0f);  // no Tank body rotation
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
    glTranslatef(g.tank2.pos[0], g.tank2.pos[1], g.tank2.pos[2]);
    glTranslatef(15.0f, 0.0f, 0.0f); 
    glScalef(0.75f, 0.75f, 0.75f); // scale the tank 75%
    glRotatef(g.tank2.angle, 0.0f, 0.0f, 1.0f);  // Rotate the cannon
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

void renderBoxes()
{
    // draw boxes
    for (int i = 0; i <= 1; i++) {      // i = num of boxes
        for (int j = 0; j <= 1; j++) {  // j = box positions
            glPushMatrix();
            glColor3ubv(box[i].color);
            glTranslatef(box[i].pos[j], box[i].pos[j + 1], 0.0f);
            glBegin(GL_QUADS);
            glVertex2f(-box[i].w, -box[i].h);
            glVertex2f(-box[i].w, box[i].h);
            glVertex2f(box[i].w, box[i].h);
            glVertex2f(box[i].w, -box[i].h);
            glEnd();
            glPopMatrix();
            j++;
        }
    }
}


void renderExplosion() 
{
    double positionx = 0;
    double positiony = 0;
    // get position of explosion
    if (tankHit && currentPlayer % 2 == 0) {
        positionx = g.tank.pos[0];
        positiony = g.tank.pos[1];
    }
    else if (tankHit && currentPlayer % 2 != 0) {
        positionx = g.tank2.pos[0];
        positiony = g.tank2.pos[1];
    } else if (boxHit) {
        positionx = bx;
        positiony = by;
    }
    struct timespec bt;
    clock_gettime(CLOCK_REALTIME, &bt);
    double ts = timeDiff(&g.bulletTimer, &bt);
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 0.5f + rnd() * 0.5f, 0.0f);
    glVertex2f(positionx, positiony);
    int numVertices = 16;
    float radius = 20.0f;
    for (int i = 0; i <= numVertices; i++) {
        float angle = i * (2 * M_PI / numVertices);
        float x = cos(angle) * radius * (0.7f + rnd() * 0.3f);
        float y = sin(angle) * radius * (0.7f + rnd() * 0.3f);
        glVertex2f(positionx + x, positiony + y);
    }
    if (ts > 2.8) {
        tankHit = false;
        boxHit = false;
    }
    glEnd();
    // prevent background color bleed
    glColor3f(1.0f, 1.0f, 1.0f);
}

void checkBoxCollison(Bullet *b, int i)
{
    if (b->pos[0] > box[0].pos[0] - box[0].w &&
        b->pos[0] < box[0].pos[0] + box[0].w &&
        b->pos[1] > box[0].pos[1] - box[0].h &&
        b->pos[1] < box[0].pos[1] + box[0].h ) {
        bx = b->pos[0];
        by = b->pos[1];
        boxHit = true;
        printf("Floor hit");
        memcpy(&g.barr[i], &g.barr[g.nbullets - 1],
               sizeof(Bullet));
        g.nbullets--;
        // damageBox(b);
    }
}
/*
void damageBox(Bullet *b) 
{
    if (boxHit) {
        printf("Box hit");
    }
}
*/
void renderText() 
{
    Rect r;
    if (!gameOver()) {
        r.bot = gl.yres - 20;
        r.left = (gl.xres / 2) - 50;
        r.center = 1;
        ggprint8b(&r, 16, 0x00ffff00, "Artillery");
        ggprint8b(&r, 16, 0x00ffffff, "Tab to view controls");
        ggprint8b(&r, 16, 0x00ffff00, "Round : %d", game.getRoundsPlayed());
        // check for gameOver
        if (game.getGameStatus()) {
        }
        if (showControls) {
            ggprint8b(&r, 16, 0x00ffffff, 
                "a/d to move tank");
            ggprint8b(&r, 16, 0x00ffffff, 
                "Space to shoot Cannon");
            ggprint8b(&r, 16, 0x00ffffff, 
                "L/R arrow to adjust cannon angle");
            ggprint8b(&r, 16, 0x00ffffff, 
                "Up/Down arrow to adjust cannon power");
        }
        //  display player stats
        r.bot = gl.yres - 20;
        r.left = 10;
        r.center = 0;
        ggprint8b(&r, 16, 0x00ffff00, "  Wins: %d", 
            game.getPlayer1Wins());
        ggprint8b(&r, 16, 0x00ffff00, "  Player 1 Bullets: %d", 
            playerTank.getBullets());
        ggprint8b(&r, 16, 0x00ffff00, "  Player 1 Fuel: %.2f", 
            playerTank.getFuel());
        ggprint8b(&r, 16, 0xff00ff00, ""); 
        ggprint8b(&r, 16, 0xff00ff00, "  Player 1 Health"); 
        ggprint8b(&r, 16, 0xff00ff00, ""); 
        ggprint8b(&r, 16, 0xff00ff00, ""); 
        ggprint8b(&r, 16, 0xff00ff00, ""); 
        ggprint8b(&r, 16, 0xff00ff00, ""); 
        ggprint8b(&r, 32, 0xFFFFA500, "  Cannon Power"); 
        // display enemy stats
        r.bot = gl.yres - 20;
        r.top = r.bot + 100;
        r.right = gl.xres - 10;
        int textWidth = 120;
        r.left = gl.xres - 10 - textWidth;  // Position text on the right side
        ggprint8b(&r, 16, 0x00ffff00, "Wins: %d", 
            game.getPlayer2Wins());
        ggprint8b(&r, 16, 0x00ffff00, " Player 2 Bullets: %d", 
            enemyTank.getBullets());
        ggprint8b(&r, 16, 0x00ffff00, " Player 2 Fuel: %.2f", 
            enemyTank.getFuel());
        ggprint8b(&r, 16, 0xff00ff00, ""); 
        ggprint8b(&r, 16, 0xff00ff00, "  Player 2 Health"); 
        ggprint8b(&r, 16, 0xff00ff00, ""); 
        ggprint8b(&r, 16, 0xff00ff00, ""); 
        ggprint8b(&r, 16, 0xff00ff00, ""); 
        ggprint8b(&r, 16, 0xff00ff00, ""); 
        ggprint8b(&r, 32, 0xFFFFA500, "  Cannon Power");                                                
    } else {
        r.bot = gl.yres - 20;
        r.left = (gl.xres / 2) - 50;
        r.center = 1;
        ggprint8b(&r, 16, 0x00ffff00, "GAME OVER");
        if (playerTank.getHealth() > 0) {
            ggprint8b(&r, 16, 0x00ffff00, "Player 1 Wins");
            ggprint8b(&r, 16, 0x00ffff00, "y to Continue");
            if (newRound) {
                game.increasePlayer1Wins();
                cannonVelocity1 = 10;
                cannonVelocity2 = 10;
                playerTank.reset();
                enemyTank.reset();
                newRound = false;
            }
        }
        if (enemyTank.getHealth() > 0) {
            ggprint8b(&r, 16, 0x00ffff00, "Player 2 Wins");
            ggprint8b(&r, 16, 0x00ffff00, "y to Continue");
            if (newRound) {
                game.increasePlayer2Wins();
                cannonVelocity1 = 10;
                cannonVelocity2 = 10;
                playerTank.reset();
                enemyTank.reset();
                newRound = false;
            }
        }
        glColor3f(1.0f, 1.0f, 1.0f);
    }
}

void rainbow_ship(float *color) 
{
    color[0] -= 0.01;  // decrease red
    color[2] += 0.01;  // increase blue
    if (color[0] < 0.0)
        color[0] = 1.0;  // max red value
    if (color[2] > 1.0)
        color[2] = 0.0;  // min blue value
}

void adjustCannon(Tank *curr_tank) 
{
    // adjust cannon angle
    if ((currentPlayer % 2) == 0) {
        if (gl.tank_keys[XK_Left]) {
            curr_tank->angle += 4.0;
            if (curr_tank->angle >= 180.0f)
                curr_tank->angle = 180.0f;
        }
        if (gl.tank_keys[XK_Right]) {
            curr_tank->angle -= 4.0;
            if (curr_tank->angle < 0.0f)
                curr_tank->angle = 0.0f;
        }  // adjust cannonVelocity
        if (gl.tank_keys[XK_Up] && !keyPressed) {
            cannonVelocity1 += .25;
            if (cannonVelocity1 >= 18) {
                cannonVelocity1 = 18.0;
            }
            keyPressed = true;
        } else if (!gl.tank_keys[XK_Up]) {
            keyPressed = false;
        }
        if (gl.tank_keys[XK_Down] && !downPressed) {
            cannonVelocity1 -= .25;
            if (cannonVelocity1 <= 4) {
                cannonVelocity1 = 4.0;
            }
            downPressed = true;
        } else if (!gl.tank_keys[XK_Down]) {
            downPressed = false;
        }
    } else {
        if (gl.tank2_keys[XK_Left]) {
            curr_tank->angle += 4.0;
            if (curr_tank->angle >= 180.0f)
                curr_tank->angle = 180.0f;
        }
        if (gl.tank2_keys[XK_Right]) {
            curr_tank->angle -= 4.0;
            if (curr_tank->angle < 0.0f)
                curr_tank->angle = 0.0f;
        }
        if (gl.tank2_keys[XK_Up] && !keyPressed) {
            cannonVelocity2 += .5;
            if (cannonVelocity2 >= 18) {
                cannonVelocity2 = 18.0;
            }
            keyPressed = true;
        } else if (!gl.tank2_keys[XK_Up]) {
            keyPressed = false;
        }
        if (gl.tank2_keys[XK_Down] && !downPressed) {
            cannonVelocity2 -= .5;
            if (cannonVelocity2 <= 4) {
                cannonVelocity2 = 4.0;
            }
            downPressed = true;
        } else if (!gl.tank2_keys[XK_Down]) {
            downPressed = false;
        }
    }
}

// bot
void botCannon() 
{
    int count = 0;
    double error;
    double threshold = 0.250;
    double tank_pos = 0;
    double cannon_landing_pos = 0;
    error = tank_pos - cannon_landing_pos;
    if (error > threshold) {
        // lower
    }
    if (error < threshold) {
        // increase
    }
    // bot should aim towards the position of tank1
    // calc distance to tank, adjust velocity, as needed
    g.tank2.angle = 120.0;
    if (enemyTank.getBullets() > 0 && !gameOver() && currentPlayer % 2 != 0 && bot) {
        count++;
        if (count <= 1) {
            printf("Bot shot\n");
            shootCannon(&g.tank2);
        }
        // shootCannon(&g.tank2);
    }
}
