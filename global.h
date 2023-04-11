// Artillery classes header file
#ifndef _GLOBAL_
#define _GLOBAL_
#include <X11/Xlib.h>
#include <unistd.h>

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
// #include <X11/Xutil.h>
// #include <GL/gl.h>
// #include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/keysym.h>
#include <time.h>

// macros
#define rnd() (((Flt)rand()) / (Flt)RAND_MAX)
#define random(a) (rand() % a)
#define VecZero(v) (v)[0] = 0.0, (v)[1] = 0.0, (v)[2] = 0.0
#define MakeVector(x, y, z, v) (v)[0] = (x), (v)[1] = (y), (v)[2] = (z)
#define VecCopy(a, b) \
    (b)[0] = (a)[0];  \
    (b)[1] = (a)[1];  \
    (b)[2] = (a)[2]
#define VecDot(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])
#define VecSub(a, b, c)       \
    (c)[0] = (a)[0] - (b)[0]; \
    (c)[1] = (a)[1] - (b)[1]; \
    (c)[2] = (a)[2] - (b)[2]

// defined types
typedef float Flt;
typedef float Vec[3];
typedef Flt Matrix[4][4];

// constants
// const float timeslice = 1.0f;
#define PI 3.141592653589793
#define ALPHA 1
const int MAX_BULLETS = 1;
const Flt MINIMUM_ASTEROID_SIZE = 60.0;

#define MAX_BULLETS 1

class Global {
   public:
    int xres, yres;
    char tank_keys[65536];
    char tank2_keys[65536];
    GLuint background_texture;
    int feature_mode;
    Global();
};

class Tank {
   public:
    Vec pos;
    Vec dir;
    Vec vel;
    Vec acc;
    Flt radius;
    float angle;
    float color[3];

   public:
    Tank();
    void setPosition(Vec position);
};

class Bullet {
   public:
    Vec pos;
    Vec vel;
    float mass;
    float color[3];
    struct timespec time;

   public:
    Bullet();
};

class Asteroid {
   public:
    Vec pos;
    Vec vel;
    int nverts;
    Flt radius;
    Vec vert[8];
    float angle;
    float rotate;
    float color[3];
    struct Asteroid* prev;
    struct Asteroid* next;

   public:
    Asteroid();
};

class Game {
   public:
    Tank tank;
    Tank tank2;
    Vec tank2_pos;
    Asteroid* ahead;
    Bullet* barr;
    int nasteroids;
    int nbullets;
    struct timespec bulletTimer;
    struct timespec mouseThrustTimer;
    bool mouseThrustOn;

   public:
    Game();
    ~Game();
};

extern Global gl;
extern Game g;

#endif
