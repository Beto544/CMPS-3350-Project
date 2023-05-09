#include "global.h"
#include "fonts.h"

extern void display();
extern void cool_down();
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void initSound();
extern void cleanupSound();
extern void playSound(ALuint source);
extern void RvFeaturemode();
extern void renderFirework();
extern void drawArrowDown(float x, float y, float size);
