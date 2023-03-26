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

class Tank {
public:
    Tank(double initialHealth, double initialFuel, int initialBullets) {
        health = initialHealth;
        fuel = initialFuel;
        bullets = initialBullets;
	}
	double getHealth() const {
        return health;
    }

    double getFuel() const {
        return fuel;
    }

    int getBullets() const {
        return bullets;
    }

    void decreaseHealth(double amount) {
        health -= amount;
    }

    void decreaseFuel(double amount) {
        fuel -= amount;
    }

    void decreaseBullets(int amount) {
        bullets -= amount;
    }
private:
    double health;
    double fuel;
    int bullets;
};


//extern void moveTank(Ship *curr_tank, Tank *tanky, int player);

#endif

