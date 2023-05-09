// Hunberto's header file
#ifndef _FEATUREMODE_
#define _FEATUREMODE_
#include "global.h"
#include "fonts.h"

class hpascual {
    int count;
};

// prototypes
extern void rainbow_ship(float *color);
extern void renderTanks();
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);
extern bool gameOver();
extern void renderExp();
extern void renderBoxes();
extern void damageBox(Bullet *b);
extern void renderExplosion();
extern void renderControls();
extern void moveTank(Tank *curr_Tank);
extern void checkBoxCollison(Bullet *b, int i);
extern void renderText();
extern void adjustCannon(Tank *curr_tank);
extern void botCannon(Tank *curr_tank);
extern bool toggle;
extern int fib(int n);

class TankStats {
   public:
    TankStats();

    double getHealth() const;
    double getFuel() const;
    int getBullets() const;
    void decreaseHealth(double amount);
    void decreaseFuel(double amount);
    void decreaseBullets(int amount);
    void reset();
    bool getLoser() const;

   private:
    double health;
    double fuel;
    int bullets;
    bool loser;
};

class GameStats {
   public:
    GameStats();

    void increasePlayer1Wins();
    void increasePlayer2Wins();
    int getPlayer1Wins() const;
    int getPlayer2Wins() const;
    int getRoundsPlayed() const;
    bool getGameStatus() const;

   private:
    int player1Wins;
    int player2Wins;
    int roundsPlayed;
    bool gameOver;
};

class Box {
public:
    float w, h;
    float pos[10];
    float radius;
    unsigned char color[3];
    void set_color(unsigned char col[3]) {
        memcpy(color, col, sizeof(unsigned char) * 3);
    }
    Box();
    Box(int wid, int hgt, int x, int y);
};

// extern void moveTank(Tank *curr_tank, Tank *tanky, int player);

#endif
