// Hunberto Pascual

/*
void checkFeatureMode(float color) {

if (gl.feature_mode) {
            r.bot = gl.yres - 20;
            r.left = 10;
            r.center = 0;
            ggprint8b(&r, 16, 0x00ff0000, "3350 - Asteroids");
            ggprint8b(&r, 16, 0x00ffff00, "n bullets: %i", g.nbullets);
            ggprint8b(&r, 16, 0x00ffff00, "n asteroids: %i", g.nasteroids);
            ggprint8b(&r, 16, 0x00ffff00, "press shift f for feature mode");
    }
}
*/
void cool_down(float *color){
    color[1] += .01;
    color[2] += .01;
    if (color[1] >1.0) 
        color[1] = 1.0;
    if (color[2] > 1.0)
        color[2] = 1.0;
}
