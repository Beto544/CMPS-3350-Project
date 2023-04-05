// global.cpp
#include "global.h"
#include <cstring>


Global::Global() {
		xres = 1200;
		yres = 780;
		std::memset(ship_keys, 0, 65536);
		std::memset(ship2_keys, 0, 65536);
}

Ship::Ship() {
		pos[0] = (Flt)(gl.xres / 8);
		pos[1] = (Flt)(gl.yres / 8);
		pos[2] = 0.0f;
		VecZero(dir);
		VecZero(vel);
		VecZero(acc);
		angle = 0.0;
		radius = 30;
		color[0] = color[1] = color[2] = 1.0;
}

void Ship::setPosition(Vec position) {
		pos[0] = position[0];
		pos[1] = position[1];
}

Bullet::Bullet() {}

Asteroid::Asteroid() {
		prev = NULL;
		next = NULL;
}

Game::Game() {
		ship2_pos[0] = (Flt)(gl.xres - 100);
		ship2_pos[1] = (Flt)(gl.yres / 8);
		ship2_pos[2] = 0.0f;
		ship2.setPosition(ship2_pos);
		ahead = NULL;
		barr = new Bullet[MAX_BULLETS];
		nasteroids = 0;
		nbullets = 0;
		barr2 = new Bullet[MAX_BULLETS];
		nbullets2 = 0;
		mouseThrustOn = false;
		clock_gettime(CLOCK_REALTIME, &bulletTimer);
}

Game::~Game() {
		delete[] barr;
		delete[] barr2;
}

