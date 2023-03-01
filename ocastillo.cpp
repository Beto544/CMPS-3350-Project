void green_ship(float *color) {
    color[0] -= 0.05; // decrease red
    color[2] -= 0.05; // decrease blue
    if (color[1] < 0.0)
        color[1] = 1.0; // max red value

    if (color[2] < 0.0)
        color[2] = 0.0; // min blue value


}
