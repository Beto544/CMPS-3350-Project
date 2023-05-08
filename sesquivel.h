#include "hpascual.h"
#include <ctime>
#include <iostream>
#include "fonts.h"
#include "global.h"

extern void renderBars(int player, double stat);
extern void renderHealthBar(float cannonVelocity, int player);
extern void renderCannonPower(float cannonVelocity, int player);

#define MAXBUTTONS 10
typedef struct t_button {
    Rect r;
    char text[32];
    int over;
    int down;
    int click;
    float color[3];
    float dcolor[3];
    unsigned int text_color;
} Button;

