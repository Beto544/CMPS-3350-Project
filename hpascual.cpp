// Hunberto Pascual

void rainbow_ship(float *color){
    color[0] -= 0.01; // decrease red
    color[2] += 0.01; // increase blue
    if (color[0] < 0.0)
        color[0] = 1.0; // max red value

    if (color[2] > 1.0)
        color[2] = 0.0; // min blue value

}

