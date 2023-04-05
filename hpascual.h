// Hunberto's header file
#ifndef _FEATUREMODE_
#define _FEATUREMODE_
#include "global.h"

class hpascual{
    int count;
};

// prototypes
extern void rainbow_ship(float *color);
extern void renderTanks();
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);
extern void shootCannon(Ship *curr_tank);
extern bool gameOver();
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



//extern void moveTank(Ship *curr_tank, Tank *tanky, int player);

#endif

