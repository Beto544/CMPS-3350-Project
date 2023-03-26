// Oscar Castillo

void green_ship(float *color) {
    color[0] -= 0.05; 
    color[2] -= 0.05; 
    if (color[1] < 0.0)
        color[1] = 1.0; 

    if (color[2] < 0.0)
        color[2] = 0.0; 


}

float bullet_gravity(float GRAVITY) {
    GRAVITY = 9.80665;
    return GRAVITY;
}
