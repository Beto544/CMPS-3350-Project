// Hunberto Pascual


void rainbow_ship(float *color){
    color[0] -= 0.01; // decrease red
    color[2] += 0.01; // increase blue
    if (color[0] < 0.0)
        color[0] = 1.0; // max red value

    if (color[2] > 1.0)
        color[2] = 0.0; // min blue value

}
/*
void confineShipToWindow() {
	//Flt d0,d1,dist;
        //Update ship position
        g.ship.pos[0] += g.ship.vel[0];
        g.ship.pos[1] += g.ship.vel[1];
        //Check for collision with window edges
        if (g.ship.pos[0] < 0.0) { // left side
                g.ship.pos[0] = ((float)gl.xres- (float)gl.xres) + 10; //plus

        }
        else if (g.ship.pos[0] > (float)gl.xres) { // right side
                g.ship.pos[0] = ((float)gl.xres) - 10;
        }
        else if (g.ship.pos[1] < 0.0) { // bottom
                g.ship.pos[1] = ((float)gl.yres - (float)gl.yres) + 10; // plus
        }
        else if (g.ship.pos[1] > (float)gl.yres) { //top
                g.ship.pos[1] = ((float)gl.yres) -10; // minus
        }
}
*/

// gives the ship a limited amount of fuel to move around
double shipFuelTest(double fuel) {
	// define fuel variables
	//const int MAX_FUEL = 100;
	//int fuel = MAX_FUEL;
	//fuel = fuel -10;
	fuel = fuel - .5;
	if (fuel == 0.0) {
		return 0;
	}
	return fuel;
}

