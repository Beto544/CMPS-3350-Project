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
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include "image.h"
#include "fonts.h"
#include "global.h"
#include "hpascual.h"
#include "log.h"
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
void shootCannon(Tank *curr_tank);
GameStats game;        // track gamestats
TankStats playerTank;  // track Tankstats
TankStats enemyTank;
Box box[10];

Image img1("desert.jpg");

int currentPlayer = 0;
float cannonVelocity1 = 10;
float cannonVelocity2 = 10;
bool nextRound = false;
bool keyPressed = false;
bool downPressed = false;
bool showControls = false;
bool newRound = false;
bool tankHit = false;
bool cannonFired = false;
bool boxHit = false;
extern void renderHill(float x);
// the Classes - Global, Ship, Bullet, Asteroid, and Game
// are now in the header file global.h

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
void check_mouse(XEvent *e, Tank *tank);
int check_keys(XEvent *e);
void physics(Tank *curr_tank);
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
                check_mouse(&e, &g.tank);
            }
            if ((currentPlayer % 2) != 0) {
                // player 2's turn
                check_mouse(&e, &g.tank2);
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
                physics(&g.tank);
                physicsCountdown -= physicsRate;
            }
            if ((currentPlayer % 2) != 0) {
                // player 2's turn
                physics(&g.tank2);
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

void check_mouse(XEvent *e, Tank *curr_tank) {
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
            gl.tank_keys[key] = 0;
            if (key == XK_Shift_L || key == XK_Shift_R) {
                shift = 0;
            }
            return 0;
        }
        if (e->type == KeyPress) {
            // std::cout << "press" << std::endl;
            gl.tank_keys[key] = 1;
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
            gl.tank2_keys[key] = 0;
            if (key == XK_Shift_L || key == XK_Shift_R) {
                shift = 0;
            }
            return 0;
        }
        if (e->type == KeyPress) {
            // std::cout << "press" << std::endl;
            gl.tank2_keys[key] = 1;
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
                if (gl.feature_mode > 0)
                    gl.feature_mode = 0;
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
        case XK_m:
            if (shift) {
                if (gl.switch_weapon > 0)
                    gl.switch_weapon = 0;
            } else
                gl.switch_weapon = 10;
            break;

        case XK_5:
            if (shift) {
                if (gl.feature_mode > 0)
                    gl.feature_mode = 0;
            } else
                gl.feature_mode = 5;
            break;

        case XK_Tab:
            showControls = !showControls;
            break;
        case XK_y:
            newRound = true;
            break;
        case XK_minus:
            break;
    }
    return 0;
}

void shootCannon(Tank *curr_tank) {
    float cannonVelocity = 0;
    if (currentPlayer % 2 == 0) {
        cannonVelocity = cannonVelocity1;
    } else {
        cannonVelocity = cannonVelocity2;
    }
    struct timespec bt;
    clock_gettime(CLOCK_REALTIME, &bt);
    double ts = timeDiff(&g.bulletTimer, &bt);
    if (ts > 0.1) {
        timeCopy(&g.bulletTimer, &bt);
        if (g.nbullets < 1) {
            //  shoot a bullet...
            Bullet *b = &g.barr[g.nbullets];
            timeCopy(&b->time, &bt);
            b->pos[0] = curr_tank->pos[0];
            b->pos[1] = curr_tank->pos[1];
            b->vel[0] = curr_tank->vel[0];
            b->vel[1] = curr_tank->vel[1];
            // convert tank cannon angle to radians
            Flt rad = ((curr_tank->angle) / 360.0f) * PI * 2.0;
            // convert angle to a vector
            Flt xdir = cos(rad);
            Flt ydir = sin(rad);
            b->pos[0] += xdir * 20.0f;
            b->pos[1] += ydir * 20.0f;
            b->vel[0] += xdir * cannonVelocity + rnd() * 1.0;
            b->vel[1] += ydir * cannonVelocity + rnd() * 1.0;
            b->color[0] = 1.0f;
            b->color[1] = 1.0f;
            b->color[2] = 1.0f;
            g.nbullets++;
            if (currentPlayer % 2 == 0) {
                playerTank.decreaseBullets(1);
            } else {
                enemyTank.decreaseBullets(1);
            }
            currentPlayer++;
            return;
        } else {
            return;
        }
    }
}

void physics(Tank *curr_tank) {
    Flt d0, d1, dist;
    moveTank(curr_tank);
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
        // move the bullet & add gravity effect
        b->pos[0] += b->vel[0];
        b->pos[1] += b->vel[1] - gravity * ts;
        // calc distance between bullet and tank
        d0 = b->pos[0] - curr_tank->pos[0];
        d1 = b->pos[1] - curr_tank->pos[1];
        dist = (d0 * d0 + d1 * d1);
        // check tank collision
        if (dist < (curr_tank->radius * curr_tank->radius)) {
            tankHit = true;
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
        }
        // check box-bullet collision
        checkBoxCollison(b, i);
        ++i;
    }

    //---------------------------------------------------
    // adjust cannon angle
    if ((currentPlayer % 2) == 0) {
        if (gl.tank_keys[XK_Left]) {
            curr_tank->angle += 4.0;
            if (curr_tank->angle >= 180.0f) {
                curr_tank->angle = 180.0f;
            }
        }
        if (gl.tank_keys[XK_Right]) {
            curr_tank->angle -= 4.0;
            if (curr_tank->angle < 0.0f)
                curr_tank->angle = 0.0f;
        }
        if (gl.tank_keys[XK_Up] && !keyPressed) {
            cannonVelocity1 += .5;
            if (cannonVelocity1 >= 16) {
                cannonVelocity1 = 16.0;
            }
            keyPressed = true;
        } else if (!gl.tank_keys[XK_Up]) {
            keyPressed = false;
        }
        if (gl.tank_keys[XK_Down] && !downPressed) {
            cannonVelocity1 -= .5;
            if (cannonVelocity1 <= 4) {
                cannonVelocity1 = 4.0;
            }
            downPressed = true;
        } else if (!gl.tank_keys[XK_Down]) {
            downPressed = false;
        }
    } else {
        if (gl.tank2_keys[XK_Left]) {
            curr_tank->angle += 4.0;
            if (curr_tank->angle >= 180.0f)
                curr_tank->angle = 180.0f;
        }
        if (gl.tank2_keys[XK_Right]) {
            curr_tank->angle -= 4.0;
            if (curr_tank->angle < 0.0f)
                curr_tank->angle = 0.0f;
        }
        if (gl.tank2_keys[XK_Up] && !keyPressed) {
            cannonVelocity2 += .5;
            if (cannonVelocity2 >= 16) {
                cannonVelocity2 = 16.0;
            }
            keyPressed = true;
        } else if (!gl.tank2_keys[XK_Up]) {
            keyPressed = false;
        }
        if (gl.tank2_keys[XK_Down] && !downPressed) {
            cannonVelocity2 -= .5;
            if (cannonVelocity2 <= 4) {
                cannonVelocity2 = 4.0;
            }
            downPressed = true;
        } else if (!gl.tank2_keys[XK_Down]) {
            downPressed = false;
        }
    }

    // shoot cannon when space is pressed
    if (gl.tank_keys[XK_space] && currentPlayer % 2 == 0) {
        if (playerTank.getBullets() > 0 && !gameOver()) {
            cannonFired = true;
            shootCannon(&g.tank);
        }
    }
    if (gl.tank2_keys[XK_space] && currentPlayer % 2 != 0) {
        if (enemyTank.getBullets() > 0 && !gameOver()) {
            shootCannon(&g.tank2);
        }
    }
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
        // Hunberto's feature mode
        if (gl.feature_mode == 12) {
        }
    }

    if (!gl.feature_mode) {
        r.bot = gl.yres - 20;
        r.left = (gl.xres / 2) - 50;
        r.center = 1;
        ggprint8b(&r, 16, 0x00ffff00, "Artillery");
        renderText();
    }
    extern void renderLand(float x);
    renderLand(400);
    for (int j = 250; j < 800; j+=100) {
        renderHill(j);
    }
    renderBoxes();
    renderTanks();
    if (tankHit) {
        renderExplosion();
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
    if (gl.switch_weapon == 10) {
        extern void renderMissile();
        renderMissile();
    }
}
