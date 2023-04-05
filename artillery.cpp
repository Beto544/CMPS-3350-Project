// Modifeid by : group 2
// artillery tank game
//
// program: artillery.cpp
// modified from asteroids.cpp
// author:  Gordon Griesel
// date:    2014 - 2023
// mod spring 2015: added constructors
// This program is a game starting point for a 3350 project.
//
//
#include <X11/Xlib.h>
#include <unistd.h>

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <thread>
// #include <X11/Xutil.h>
// #include <GL/gl.h>
// #include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include <X11/keysym.h>

#include "fonts.h"
#include "global.h"
#include "hpascual.h"
#include "log.h"
#include <X11/Xatom.h>
// constants
const float gravity = 9.8f;

//-----------------------------------------------------------------------------
// Setup timers
const double physicsRate = 1.0 / 60.0;
const double oobillion = 1.0 / 1e9;
extern struct timespec timeStart, timeCurrent;
extern struct timespec timePause;
extern double physicsCountdown;
extern double timeSpan;
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);
//-----------------------------------------------------------------------------
Global gl;
Game g;
GameStats game; // track gamestats
TankStats playerTank; // track Tankstats
TankStats enemyTank;
int currentPlayer = 0;
float cannonVelocity1 = 10;
float cannonVelocity2 = 10;
bool nextRound = false;
bool keyPressed = false;
bool downPressed = false;
bool showControls = false;
bool newRound = false;

// the Classes - Global, Ship, Bullet, Asteroid, and Game
// are now in the header file global.h

// Put image file colors into a data stream.
class Image {
   public:
    int width, height;
    unsigned char *data;
    ~Image() { delete[] data; }
    Image(const char *fname) {
        if (fname[0] == '\0')
            return;
        int ppmFlag = 0;
        char name[40];
        strcpy(name, fname);
        int slen = strlen(name);
        char ppmname[80];
        if (strncmp(name + (slen - 4), ".ppm", 4) == 0)
            ppmFlag = 1;
        if (ppmFlag) {
            strcpy(ppmname, name);
        } else {
            name[slen - 4] = '\0';
            sprintf(ppmname, "%s.ppm", name);
            char ts[100];
            sprintf(ts, "convert %s %s", fname, ppmname);
            system(ts);
        }
        FILE *fpi = fopen(ppmname, "r");
        if (fpi) {
            char line[200];
            fgets(line, 200, fpi);
            fgets(line, 200, fpi);
            // skip comments and blank lines
            while (line[0] == '#' || strlen(line) < 2)
                fgets(line, 200, fpi);
            sscanf(line, "%i %i", &width, &height);
            fgets(line, 200, fpi);
            // get pixel data
            int n = width * height * 3;
            data = new unsigned char[n];
            for (int i = 0; i < n; i++)
                data[i] = fgetc(fpi);
            fclose(fpi);
        } else {
            printf("ERROR opening image: %s\n", ppmname);
            exit(0);
        }
        if (!ppmFlag)
            unlink(ppmname);
    }
} img1("desert.jpg");

// X Windows variables
class X11_wrapper {
   private:
    Display *dpy;
    Window win;
    GLXContext glc;

   public:
    X11_wrapper() {}
    X11_wrapper(int w, int h) {
        GLint att[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
        // GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
        XSetWindowAttributes swa;
        setup_screen_res(gl.xres, gl.yres);
        dpy = XOpenDisplay(NULL);
        if (dpy == NULL) {
            std::cout << "\n\tcannot connect to X server" << std::endl;
            exit(EXIT_FAILURE);
        }
        Window root = DefaultRootWindow(dpy);
        XWindowAttributes getWinAttr;
        XGetWindowAttributes(dpy, root, &getWinAttr);
        int fullscreen = 0;
        gl.xres = w;
        gl.yres = h;
        if (!w && !h) {
            // Go to fullscreen.
            gl.xres = getWinAttr.width;
            gl.yres = getWinAttr.height;
            // When window is fullscreen, there is no client window
            // so keystrokes are linked to the root window.
            XGrabKeyboard(dpy, root, False,
                          GrabModeAsync, GrabModeAsync, CurrentTime);
            fullscreen = 1;
        }
        XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
        if (vi == NULL) {
            std::cout << "\n\tno appropriate visual found\n"
                      << std::endl;
            exit(EXIT_FAILURE);
        }
        Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
        swa.colormap = cmap;
        swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
                         PointerMotionMask | MotionNotify | ButtonPress | ButtonRelease |
                         StructureNotifyMask | SubstructureNotifyMask;
        unsigned int winops = CWBorderPixel | CWColormap | CWEventMask;
        if (fullscreen) {
            winops |= CWOverrideRedirect;
            swa.override_redirect = True;
        }
        win = XCreateWindow(dpy, root, 0, 0, gl.xres, gl.yres, 0,
                            vi->depth, InputOutput, vi->visual, winops, &swa);
        // win = XCreateWindow(dpy, root, 0, 0, gl.xres, gl.yres, 0,
        // vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
        set_title();
        glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
        glXMakeCurrent(dpy, win, glc);
        show_mouse_cursor(0);
    }
    ~X11_wrapper() {
        XDestroyWindow(dpy, win);
        XCloseDisplay(dpy);
    }
    void set_title() {
        // Set the window title bar.
        XMapWindow(dpy, win);
        XStoreName(dpy, win, "Artillery");
    }
    void check_resize(XEvent *e) {
        // The ConfigureNotify is sent by the
        // server if the window is resized.
        if (e->type != ConfigureNotify)
            return;
        XConfigureEvent xce = e->xconfigure;
        if (xce.width != gl.xres || xce.height != gl.yres) {
            // Window size did change.
            reshape_window(xce.width, xce.height);
        }
    }
    void reshape_window(int width, int height) {
        // window has been resized.
        setup_screen_res(width, height);
        glViewport(0, 0, (GLint)width, (GLint)height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
        set_title();
    }
    void setup_screen_res(const int w, const int h) {
        gl.xres = w;
        gl.yres = h;
    }
    void swapBuffers() {
        glXSwapBuffers(dpy, win);
    }
    bool getXPending() {
        return XPending(dpy);
    }
    XEvent getXNextEvent() {
        XEvent e;
        XNextEvent(dpy, &e);
        return e;
    }
    void set_mouse_position(int x, int y) {
        XWarpPointer(dpy, None, win, 0, 0, 0, 0, x, y);
    }
    void show_mouse_cursor(const int onoff) {
        if (onoff) {
            // this removes our own blank cursor.
            XUndefineCursor(dpy, win);
            return;
        }
        // vars to make blank cursor
        Pixmap blank;
        XColor dummy;
        char data[1] = {0};
        Cursor cursor;
        // make a blank cursor
        blank = XCreateBitmapFromData(dpy, win, data, 1, 1);
        if (blank == None)
            std::cout << "error: out of memory." << std::endl;
        cursor = XCreatePixmapCursor(dpy, blank, blank, &dummy, &dummy, 0, 0);
        XFreePixmap(dpy, blank);
        // this makes the cursor. then set it using this function
        XDefineCursor(dpy, win, cursor);
        // after you do not need the cursor anymore use this function.
        // it will undo the last change done by XDefineCursor
        //(thus do only use ONCE XDefineCursor and then XUndefineCursor):
    }
} x11(gl.xres, gl.yres);
// ---> for fullscreen x11(0, 0);

// function prototypes
void init_opengl(void);
void check_mouse(XEvent *e, Ship *ship);
int check_keys(XEvent *e);
void physics(Ship *curr_tank);
void render();

//==========================================================================
// M A I N
//==========================================================================
int main() {
    logOpen();
    init_opengl();
    srand(time(NULL));
    clock_gettime(CLOCK_REALTIME, &timePause);
    clock_gettime(CLOCK_REALTIME, &timeStart);
    x11.set_mouse_position(100, 100);
    int done = 0;
    while (!done) {
        while (x11.getXPending()) {
            XEvent e = x11.getXNextEvent();
            x11.check_resize(&e);
            if ((currentPlayer % 2) == 0) {
                // player 1's turn
                check_mouse(&e, &g.ship);
            }
            if ((currentPlayer % 2) != 0) {
                // player 2's turn
                check_mouse(&e, &g.ship2);
            }
            done = check_keys(&e);
        }
        clock_gettime(CLOCK_REALTIME, &timeCurrent);
        timeSpan = timeDiff(&timeStart, &timeCurrent);
        timeCopy(&timeStart, &timeCurrent);
        physicsCountdown += timeSpan;
        while (physicsCountdown >= physicsRate) {
            if ((currentPlayer % 2) == 0) {
                // player 1's turn
                physics(&g.ship);
                physicsCountdown -= physicsRate;
            }
            if ((currentPlayer % 2) != 0) {
                // player 2's turn
                physics(&g.ship2);
                physicsCountdown -= physicsRate;
            }
        }
        render();
        x11.swapBuffers();
    }
    cleanup_fonts();
    logClose();
    return 0;
}

void init_opengl(void) {
    // OpenGL initialization
    glViewport(0, 0, gl.xres, gl.yres);
    // Initialize matrices
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // This sets 2D mode (no perspective)
    glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
    //
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);
    glDisable(GL_CULL_FACE);
    //
    // Clear the screen to black
    glClearColor(0.0, 0.0, 0.0, 1.0);
    // Do this to allow fonts
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();

    glGenTextures(1, &gl.background_texture);
    int w = img1.width;
    int h = img1.height;
    glBindTexture(GL_TEXTURE_2D, gl.background_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, img1.data);
}

void normalize2d(Vec v) {
    Flt len = v[0] * v[0] + v[1] * v[1];
    if (len == 0.0f) {
        v[0] = 1.0;
        v[1] = 0.0;
        return;
    }
    len = 1.0f / sqrt(len);
    v[0] *= len;
    v[1] *= len;
}

void check_mouse(XEvent *e, Ship *curr_tank) {
    // Did the mouse move?
    // Was a mouse button clicked?
    static int savex = 0;
    static int savey = 0;
    //
    static int ct = 0;
    // std::cout << "m" << std::endl << std::flush;
    if (e->type == ButtonRelease) {
        return;
    }
    if (e->type == ButtonPress) {
        // ** BUGGED ** when switching from fi with space and mouse click
        
        if (e->xbutton.button == 1) {
            // Left button is down 
        } 
        if (e->xbutton.button == 3) {
            // Right button is down
        }
    }
    // keys[XK_Up] = 0;
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
        // Mouse moved
        int xdiff = savex - e->xbutton.x;
        int ydiff = savey - e->xbutton.y;
        if (++ct < 10)
            return;
        // std::cout << "savex: " << savex << std::endl << std::flush;
        // std::cout << "e->xbutton.x: " << e->xbutton.x << std::endl <<
        // std::flush;
        if (xdiff > 0) {
            // std::cout << "xdiff: " << xdiff << std::endl << std::flush;
            curr_tank->angle += 0.05f * (float)xdiff;
            if (curr_tank->angle >= 180.0f)
                curr_tank->angle = 180.0f;
        } else if (xdiff < 0) {
            // std::cout << "xdiff: " << xdiff << std::endl << std::flush;
            curr_tank->angle += 0.05f * (float)xdiff;
            if (curr_tank->angle < 0.0f)
                curr_tank->angle = 0.0f;
        }
        if (ydiff > 0) {
            // apply thrust
            // convert ship angle to radians
            Flt rad = ((curr_tank->angle + 90.0) / 360.0f) * PI * 2.0;
            // convert angle to a vector
            Flt xdir = cos(rad);
            Flt ydir = sin(rad);
            curr_tank->vel[0] += xdir * (float)ydiff * 0.01f;
            curr_tank->vel[1] += ydir * (float)ydiff * 0.01f;
            Flt speed = sqrt(curr_tank->vel[0] * curr_tank->vel[0] +
                             curr_tank->vel[1] * curr_tank->vel[1]);
            if (speed > 0.5f) {
                speed = 0.5f;
                normalize2d(g.ship.vel);
                curr_tank->vel[0] *= speed;
                curr_tank->vel[1] *= speed;
            }
            g.mouseThrustOn = true;
            clock_gettime(CLOCK_REALTIME, &g.mouseThrustTimer);
        }
        x11.set_mouse_position(100, 100);
        savex = 100;
        savey = 100;
    }
}

int check_keys(XEvent *e) {
    static int shift = 0;
    int key = (XLookupKeysym(&e->xkey, 0) & 0x0000ffff);
    if ((currentPlayer % 2) == 0) {
        if (e->type != KeyRelease && e->type != KeyPress) {
            // not a keyboard event
            return 0;
        }
        // Log("key: %i\n", key);
        if (e->type == KeyRelease) {
            gl.ship_keys[key] = 0;
            if (key == XK_Shift_L || key == XK_Shift_R) {
                shift = 0;
            }
            return 0;
        }
        if (e->type == KeyPress) {
            // std::cout << "press" << std::endl;
            gl.ship_keys[key] = 1;
            if (key == XK_Shift_L || key == XK_Shift_R) {
                shift = 1;
                return 0;
            }
        }
    } else {
        if (e->type != KeyRelease && e->type != KeyPress) {
            // not a keyboard event
            return 0;
        }
        // Log("key: %i\n", key);
        if (e->type == KeyRelease) {
            gl.ship2_keys[key] = 0;
            if (key == XK_Shift_L || key == XK_Shift_R) {
                shift = 0;
            }
            return 0;
        }
        if (e->type == KeyPress) {
            // std::cout << "press" << std::endl;
            gl.ship2_keys[key] = 1;
            if (key == XK_Shift_L || key == XK_Shift_R) {
                shift = 1;
                return 0;
            }
        }
    }

    (void)shift;
    switch (key) {
        // fuel feature mode
        case XK_f:
            if (shift) {
                if (gl.feature_mode == 12)
                    gl.feature_mode = 0;
            } else
                gl.feature_mode = 12;
            break;

        case XK_Escape:
            return 1;
        case XK_1:
            // toggle feature modes with 1,2,3,4,5
            if (shift) {
                // if (gl.feature_mode == 1)
                //     gl.feature_mode = 0;
                nextRound = true;
            } else
                gl.feature_mode = 1;
            break;
        case XK_2:
            if (shift) {
                if (gl.feature_mode > 0)
                    gl.feature_mode = 0;
            } else
                gl.feature_mode = 2;
            break;
        case XK_3:
            if (shift) {
                if (gl.feature_mode > 0)
                    gl.feature_mode = 0;
            } else
                gl.feature_mode = 3;
            break;
        case XK_4:
            if (shift) {
                if (gl.feature_mode > 0)
                    gl.feature_mode = 0;
            } else
                gl.feature_mode = 4;
            break;
        case XK_5:
            if (shift) {
                if (gl.feature_mode > 0)
                    gl.feature_mode = 0;
            } else
                gl.feature_mode = 5;
            break;

        case XK_Tab:
            showControls =!showControls;
            break;
        case XK_y:
            newRound = true;
            break;
        case XK_minus:
            break;
    }
    return 0;
}
void deleteAsteroid(Game *g, Asteroid *node) {
    // Remove a node from doubly-linked list
    // Must look at 4 special cases below.
    if (node->prev == NULL) {
        if (node->next == NULL) {
            // only 1 item in list.
            g->ahead = NULL;
        } else {
            // at beginning of list.
            node->next->prev = NULL;
            g->ahead = node->next;
        }
    } else {
        if (node->next == NULL) {
            // at end of list.
            node->prev->next = NULL;
        } else {
            // in middle of list.
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }
    }
    delete node;
    node = NULL;
}

void buildAsteroidFragment(Asteroid *ta, Asteroid *a) {
    // build ta from a
    ta->nverts = 8;
    ta->radius = a->radius / 2.0;
    Flt r2 = ta->radius / 2.0;
    Flt angle = 0.0f;
    Flt inc = (PI * 2.0) / (Flt)ta->nverts;
    for (int i = 0; i < ta->nverts; i++) {
        ta->vert[i][0] = sin(angle) * (r2 + rnd() * ta->radius);
        ta->vert[i][1] = cos(angle) * (r2 + rnd() * ta->radius);
        angle += inc;
    }
    ta->pos[0] = a->pos[0] + rnd() * 10.0 - 5.0;
    ta->pos[1] = a->pos[1] + rnd() * 10.0 - 5.0;
    ta->pos[2] = 0.0f;
    ta->angle = 0.0;
    ta->rotate = a->rotate + (rnd() * 4.0 - 2.0);
    ta->color[0] = 0.8;
    ta->color[1] = 0.8;
    ta->color[2] = 0.7;
    ta->vel[0] = a->vel[0] + (rnd() * 2.0 - 1.0);
    ta->vel[1] = a->vel[1] + (rnd() * 2.0 - 1.0);
    // std::cout << "frag" << std::endl;
}

void physics(Ship *curr_tank) {
    Flt d0, d1, dist;
    if (currentPlayer % 2 == 0) {
        if (gl.ship_keys[XK_a]) {
            if (playerTank.getFuel() >= 0) {
                curr_tank->pos[0] -= 0.30; 
                playerTank.decreaseFuel(.1);
            }
        }
        if (gl.ship_keys[XK_d]) {
            if (playerTank.getFuel() >= 0) {
                curr_tank->pos[0] += 0.30;
                playerTank.decreaseFuel(.1);
            }
        }
        
    } else {
        if (gl.ship2_keys[XK_a]) {
            if (enemyTank.getFuel() >= 0) {
                curr_tank->pos[0] -= 0.30;
                enemyTank.decreaseFuel(.1);
            }
        }
        if (gl.ship2_keys[XK_d]) {
            if (enemyTank.getFuel() >= 0) {
                curr_tank->pos[0] += 0.30;
                enemyTank.decreaseFuel(.1);
            }
        }
        
    }
    // Check for collision with window edges
    if (curr_tank->pos[0] < 0.0) {                                   // left side
        curr_tank->pos[0] = ((float)gl.xres - (float)gl.xres) + 10;  // plus
    } else if (curr_tank->pos[0] > (float)gl.xres) {                 // right side
        curr_tank->pos[0] = ((float)gl.xres) - 10;
    } else if (curr_tank->pos[1] < 0.0) {                            // bottom
        curr_tank->pos[1] = ((float)gl.yres - (float)gl.yres) + 10;  // plus
    } else if (curr_tank->pos[1] > (float)gl.yres) {                 // top
        curr_tank->pos[1] = ((float)gl.yres) - 10;                   // minus
    }

    // Update bullet positions
    struct timespec bt;
    clock_gettime(CLOCK_REALTIME, &bt);
    int i = 0;
    while (i < g.nbullets) {
        Bullet *b = &g.barr[i];
        // How long has bullet been alive?
        double ts = timeDiff(&b->time, &bt);
        if (ts > 3.5) {
            // time to delete the bullet.
            memcpy(&g.barr[i], &g.barr[g.nbullets - 1],
                   sizeof(Bullet));
            g.nbullets--;
            // do not increment i.
            continue;
        }
        // move the bullet
        b->pos[0] += b->vel[0];
        b->pos[1] += b->vel[1] - gravity * ts;
        d0 = b->pos[0] - curr_tank->pos[0];
        d1 = b->pos[1] - curr_tank->pos[1];
        dist = (d0 * d0 + d1 * d1);
        if (dist < (curr_tank->radius * curr_tank->radius)) {
            if (currentPlayer % 2 == 0) {
                playerTank.decreaseHealth(25);
            }
            if (currentPlayer % 2 != 0) {
                enemyTank.decreaseHealth(25);
            }
            memcpy(&g.barr[i], &g.barr[g.nbullets - 1],
                   sizeof(Bullet));
            g.nbullets--;
            std::cout << "Tank hit." << std::endl;
            // this asteroid is hit.
        }
        ++i;
    }

    /* -- don't need to check for this --
     *
    //Check for collision with window edges
    if (b->pos[0] < 0.0) {
    b->pos[0] += (float)gl.xres;
    }
    else if (b->pos[0] > (float)gl.xres) {
    b->pos[0] -= (float)gl.xres;
    if (gl.feature_mode == 3) {
    extern float bullet_gravity(float GRAVITY);
    b->pos[0] -= bullet_gravity(g.nbullets);
    }
    }
    else if (b->pos[1] < 0.0) {
    b->pos[1] += (float)gl.yres;
    }
    else if (b->pos[1] > (float)gl.yres) {
    b->pos[1] -= (float)gl.yres;
    if (gl.feature_mode == 3) {
    extern float bullet_gravity(float GRAVITY);
    b->pos[1] -= bullet_gravity(g.nbullets);
    }
    }*/

    //
    // Update asteroid positions
    Asteroid *a = g.ahead;
    while (a) {
        a->pos[0] += a->vel[0];
        a->pos[1] += a->vel[1];
        // Check for collision with window edges
        if (a->pos[0] < -100.0) {
            a->pos[0] += (float)gl.xres + 200;
        } else if (a->pos[0] > (float)gl.xres + 100) {
            a->pos[0] -= (float)gl.xres + 200;
        } else if (a->pos[1] < -100.0) {
            a->pos[1] += (float)gl.yres + 200;
        } else if (a->pos[1] > (float)gl.yres + 100) {
            a->pos[1] -= (float)gl.yres + 200;
        }
        a->angle += a->rotate;
        a = a->next;
    }
    //
    // Asteroid collision with bullets?
    // If collision detected:
    //     1. delete the bullet
    //     2. break the asteroid into pieces
    //        if asteroid small, delete it
    a = g.ahead;
    while (a) {
        // is there a bullet within its radius?
        int i = 0;
        while (i < g.nbullets) {
            Bullet *b = &g.barr[i];
            d0 = b->pos[0] - curr_tank->pos[0];
            d1 = b->pos[1] - curr_tank->pos[1];
            dist = (d0 * d0 + d1 * d1);
            if (dist < (curr_tank->radius * curr_tank->radius)) {
                // std::cout << "asteroid hit." << std::endl;
                // this asteroid is hit.
                if (a->radius > MINIMUM_ASTEROID_SIZE) {
                    // break it into pieces.
                    Asteroid *ta = a;
                    buildAsteroidFragment(ta, a);
                    int r = rand() % 10 + 5;
                    for (int k = 0; k < r; k++) {
                        // get the next asteroid position in the array
                        Asteroid *ta = new Asteroid;
                        buildAsteroidFragment(ta, a);
                        // add to front of asteroid linked list
                        ta->next = g.ahead;
                        if (g.ahead != NULL)
                            g.ahead->prev = ta;
                        g.ahead = ta;
                        g.nasteroids++;
                    }
                } else {
                    a->color[0] = 1.0;
                    a->color[1] = 0.1;
                    a->color[2] = 0.1;
                    // asteroid is too small to break up
                    // delete the asteroid and bullet
                    Asteroid *savea = a->next;
                    deleteAsteroid(&g, a);
                    a = savea;
                    g.nasteroids--;
                }
                // delete the bullet...
                memcpy(&g.barr[i], &g.barr[g.nbullets - 1], sizeof(Bullet));
                g.nbullets--;
                if (a == NULL)
                    break;
            }
            i++;
        }
        if (a == NULL)
            break;
        a = a->next;
    }
    //---------------------------------------------------

    // check keys pressed now
    if ((currentPlayer % 2) == 0) {
        if (gl.ship_keys[XK_Left]) {
            curr_tank->angle += 4.0;
            if (curr_tank->angle >= 180.0f)
                curr_tank->angle = 180.0f;
        }
        if (gl.ship_keys[XK_Right]) {
            curr_tank->angle -= 4.0;
            if (curr_tank->angle < 0.0f)
                curr_tank->angle = 0.0f;
        }
        if (gl.ship_keys[XK_Up] && !keyPressed) {
            cannonVelocity1 += .5;
            if (cannonVelocity1 >= 16) {
                cannonVelocity1 = 16.0;
            }
            keyPressed = true;
        } else if (!gl.ship_keys[XK_Up]) {
            keyPressed = false;
        }
        if (gl.ship_keys[XK_Down] && !downPressed) {
            cannonVelocity1 -= .5;
            if (cannonVelocity1 <= 4) {
                cannonVelocity1 = 4.0;
            }
            downPressed = true;
        } else if (!gl.ship_keys[XK_Down]) {
            downPressed = false;
        }
    } else {
        if (gl.ship2_keys[XK_Left]) {
            curr_tank->angle += 4.0;
            if (curr_tank->angle >= 180.0f)
                curr_tank->angle = 180.0f;
        }
        if (gl.ship2_keys[XK_Right]) {
            curr_tank->angle -= 4.0;
            if (curr_tank->angle < 0.0f)
                curr_tank->angle = 0.0f;
        }
        if (gl.ship2_keys[XK_Up] && !keyPressed) {
            cannonVelocity2 += .5;
            if (cannonVelocity2 >= 16) {
                cannonVelocity2 = 16.0;
            }
            keyPressed = true;
        } else if (!gl.ship2_keys[XK_Up]) {
            keyPressed = false;
        }
        if (gl.ship2_keys[XK_Down] && !downPressed) {
            cannonVelocity2 -= .5;
            if (cannonVelocity2 <= 4) {
                cannonVelocity2 = 4.0;
            }
            downPressed = true;
        } else if (!gl.ship2_keys[XK_Down]) {
            downPressed = false;
        }
    }

    if (gl.ship_keys[XK_Up] || gl.ship2_keys[XK_Up]) {
        // apply thrust
        // convert ship angle to radians
        Flt rad = ((curr_tank->angle + 90.0) / 360.0f) * PI * 2.0;
        // convert angle to a vector
        Flt xdir = cos(rad);
        Flt ydir = sin(rad);
        curr_tank->vel[0] += xdir * 0.05f;
        curr_tank->vel[1] += ydir * 0.05f;
        Flt speed = sqrt(curr_tank->vel[0] * curr_tank->vel[0] +
                         curr_tank->vel[1] * curr_tank->vel[1]);
        if (speed > 0.5f) {
            speed = 0.5f;
            normalize2d(curr_tank->vel);
            curr_tank->vel[0] *= speed;
            curr_tank->vel[1] *= speed;
        }
    }
    if (gl.ship_keys[XK_space] && currentPlayer % 2 == 0) {
        if (playerTank.getBullets() > 0 && !gameOver()) {
            shootCannon(&g.ship);
        }
    }
    if (gl.ship2_keys[XK_space] && currentPlayer % 2 != 0) {
        if (enemyTank.getBullets() > 0&& !gameOver()) {
            shootCannon(&g.ship2);
        }
    }
    /*
    if (g.mouseThrustOn) {
        // should thrust be turned off
        struct timespec mtt;
        clock_gettime(CLOCK_REALTIME, &mtt);
        double tdif = timeDiff(&mtt, &g.mouseThrustTimer);
        // std::cout << "tdif: " << tdif << std::endl;
        if (tdif < -0.3)
            g.mouseThrustOn = false;
    } */
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    // draw background
    glViewport(0, 0, gl.xres, gl.yres);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, gl.background_texture);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1);
    glVertex2f(0, 0);
    glTexCoord2f(1, 1);
    glVertex2f(gl.xres, 0);
    glTexCoord2f(1, 0);
    glVertex2f(gl.xres, gl.yres);
    glTexCoord2f(0, 0);
    glVertex2f(0, gl.yres);
    glEnd();
    Rect r;
    if (gl.feature_mode) {
        // render green border
        int t = 40;
        // glColor3f(0.0f, 1.0f, 0.0f);
        // glBegin(GL_TRIANGLE_STRIP);
        glColor3f(0.0, 1.0, 0.0);
        glBegin(GL_TRIANGLE_STRIP);
        glVertex2i(0, 0);
        glVertex2i(t, t);
        glVertex2i(0, gl.yres);
        glVertex2i(t, gl.yres - t);
        glVertex2i(gl.xres, gl.yres);
        glVertex2i(gl.xres - t, gl.yres - t);
        glVertex2i(gl.xres, 0);
        glVertex2i(gl.xres - t, t);
        glVertex2i(0, 0);
        glVertex2i(t, t);
        glEnd();
        r.bot = gl.yres - 30;
        r.left = gl.yres / 2;
        r.center = 0;
        // Hunberto's feature mode -- Gives the ship a fuel tank, no fuel cant move
        if (gl.feature_mode == 12) {
            // x11.show_mouse_cursor(1);
            /*
               if (fuel_tank > 0) {
               ggprint8b(&r, 0, 0x00ff0000, "Fuel Level: %.2f", fuel_tank);
               }
               if (fuel_tank == 0.0) {
               ggprint8b(&r, 16, 0x00ff0000, "Out of Fuel!");
               } */
        }
    }

    if (!gl.feature_mode) {
        if (!gameOver()) {
            r.bot = gl.yres - 20;
            r.left = (gl.xres / 2) - 50;
            r.center = 1;
            ggprint8b(&r, 16, 0x00ffff00, "Artillery");
            // Controls
            //ggprint8b(&r, 16, 0x00ffff00, "a & d to move tank, arrow keys to adjust cannon angle");
            ggprint8b(&r, 16, 0x00ffffff, "Tab to view controls");
            ggprint8b(&r, 16, 0x00ffff00, "Round : %d", game.getRoundsPlayed());
            // check for gameOver
            if (game.getGameStatus()) {
    
            }
            if (showControls) {
                ggprint8b(&r, 16, 0x00ffffff, "a/d to move tank");
                ggprint8b(&r, 16, 0x00ffffff, "Space to shoot Cannon");
                ggprint8b(&r, 16, 0x00ffffff, "L/R arrow to adjust cannon angle");
                ggprint8b(&r, 16, 0x00ffffff, "Up/Down arrow to adjust cannon power");
            }
            //  display player stats
            r.bot = gl.yres - 20;
            r.left = 10;
            r.center = 0;
            ggprint8b(&r, 16, 0x00ffff00, "Wins: %d", game.getPlayer1Wins());
            ggprint8b(&r, 16, 0xff00ff00, "Player1 Health: %.1f", playerTank.getHealth());
            ggprint8b(&r, 16, 0x00ffff00, "Player1 Fuel: %.1f", playerTank.getFuel());
            ggprint8b(&r, 16, 0x00ffff00, "Player1 Bullets: %d", playerTank.getBullets());
            double power = (cannonVelocity1 - 4.0) / (16 - 4) * 100;
            ggprint8b(&r, 16, 0x00ff0000, "Cannon Power: %.2f", power);  // min of 4 max of 16
            ggprint8b(&r, 16, 0x00ffff00, "press 1, 2, 3, 4, or 5 for feature modes");
            ggprint8b(&r, 16, 0x00ffff00, "f for Fuel testing");
            // display enemy stats
            r.bot = gl.yres - 20;
            r.top = r.bot + 100;
            r.right = gl.xres - 10;
            int textWidth = 120;
            r.left = gl.xres - 10 - textWidth;  // Position text on the right side
            ggprint8b(&r, 16, 0x00ffff00, "Wins: %d", game.getPlayer2Wins());
            ggprint8b(&r, 16, 0xff00ff00, "Player2 Health: %.1f", enemyTank.getHealth());
            ggprint8b(&r, 16, 0x00ffff00, "Player2 Fuel: %.1f", enemyTank.getFuel());
            ggprint8b(&r, 16, 0x00ffff00, "Player2 Bullets: %d", enemyTank.getBullets());
            double power2 = (cannonVelocity2 - 4.0) / (16 - 4) * 100;
            ggprint8b(&r, 16, 0x00ff0000, "Cannon Power: %.2f", power2);  // min of 4 max of 16
            renderTanks();  // rendering tanks done in hpascual.cpp
        } else {
            r.bot = gl.yres - 20;
            r.left = (gl.xres / 2) - 50;
            r.center = 1;
            ggprint8b(&r, 16, 0x00ffff00, "GAME OVER");
            if (playerTank.getHealth() > 0) {
                ggprint8b(&r, 16, 0x00ffff00, "Player 1 Wins");
                ggprint8b(&r, 16, 0x00ffff00, "y to Continue");
                if (newRound) {
                    game.increasePlayer1Wins();
                    cannonVelocity1 = 10;
                    cannonVelocity2 = 10;
                    playerTank.reset();
                    enemyTank.reset();
                    newRound = false;
                }
            }
            if (enemyTank.getHealth() > 0) {
                ggprint8b(&r, 16, 0x00ffff00, "Player 2 Wins");
                ggprint8b(&r, 16, 0x00ffff00, "y to Continue");
                if (newRound) {
                    game.increasePlayer2Wins();
                    cannonVelocity1 = 10;
                    cannonVelocity2 = 10;
                    playerTank.reset();
                    enemyTank.reset();
                    newRound = false;
                }
            }
            glColor3f(1.0f, 1.0f, 1.0f);
            // add timer before the reset calls
            // playerTank.reset();
            // enemyTank.reset();
        }
    }
    if (gl.ship_keys[XK_Up] || g.mouseThrustOn) {
        // draw thrust
        if (gl.feature_mode == 12) {
        } /*
                 else {
                 Flt rad = ((g.ship.angle+90.0) / 360.0f) * PI * 2.0;
        //convert angle to a vector
        Flt xdir = cos(rad);
        Flt ydir = sin(rad);
        Flt xs,ys,xe,ye,r;
        glBegin(GL_LINES);
        for (i=0; i<16; i++) {
        xs = -xdir * 11.0f + rnd() * 4.0 - 2.0;
        ys = -ydir * 11.0f + rnd() * 4.0 - 2.0;
        r = rnd()*40.0+40.0;
        xe = -xdir * r + rnd() * 18.0 - 9.0;
        ye = -ydir * r + rnd() * 18.0 - 9.0;
        glColor3f(rnd()*.3+.7, rnd()*.3+.7, 0);
        glVertex2f(g.ship.pos[0]+xs,g.ship.pos[1]+ys);
        glVertex2f(g.ship.pos[0]+xe,g.ship.pos[1]+ye);
        }
        glEnd();
        glEnd();
        } */
    }

    //-------------------------------------------------------------------------
    // Draw the asteroids
    Asteroid *a = g.ahead;
    while (a) {
        // Log("draw asteroid...\n");
        glColor3fv(a->color);
        glPushMatrix();
        glTranslatef(a->pos[0], a->pos[1], a->pos[2]);
        glRotatef(a->angle, 0.0f, 0.0f, 1.0f);
        glBegin(GL_LINE_LOOP);
        // Log("%i verts\n",a->nverts);
        for (int j = 0; j < a->nverts; j++) {
            glVertex2f(a->vert[j][0], a->vert[j][1]);
        }
        glEnd();
        // glBegin(GL_LINES);
        //	glVertex2f(0,   0);
        //	glVertex2f(a->radius, 0);
        // glEnd();
        glPopMatrix();
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_POINTS);
        glVertex2f(a->pos[0], a->pos[1]);
        glEnd();
        a = a->next;
    }
    //-------------------------------------------------------------------------
    // Draw the bullets
    for (int i = 0; i < g.nbullets; i++) {
        Bullet *b = &g.barr[i];
        // Log("draw bullet...\n");
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_POINTS);
        glVertex2f(b->pos[0], b->pos[1]);
        glVertex2f(b->pos[0] - 1.0f, b->pos[1]);
        glVertex2f(b->pos[0] + 1.0f, b->pos[1]);
        glVertex2f(b->pos[0], b->pos[1] - 1.0f);
        glVertex2f(b->pos[0], b->pos[1] + 1.0f);
        glColor3f(0.8, 0.8, 0.8);
        glVertex2f(b->pos[0] - 1.0f, b->pos[1] - 1.0f);
        glVertex2f(b->pos[0] - 1.0f, b->pos[1] + 1.0f);
        glVertex2f(b->pos[0] + 1.0f, b->pos[1] - 1.0f);
        glVertex2f(b->pos[0] + 1.0f, b->pos[1] + 1.0f);
        glEnd();
    }
}
