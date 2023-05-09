//Raul Velasco
//date: 02/22/2023
#include <iostream>
#include <unistd.h>
#include <ctime>
#include "fonts.h"
#include "global.h"
#include "rvelasco.h"
#include </usr/include/AL/alut.h>

extern bool tankHit;
extern bool boxHit;
bool firework = true;
struct Particle {
    GLfloat color[3];
    GLfloat velocity[2];
    GLfloat acceleration[2];
    GLfloat position[2];
    GLfloat size; // Add a size member

};


//Examle used in class
void cool_down(float *color){
    color[0] += .01;
    color[1] += .01;
    if (color[0] > 1.0)
        color[0] = 0.1;

    if (color[1] > 1.0)
        color[1] = 0.1;
}
/*
void display()
{
    //glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(200.0, 300.0);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Start Game");
    glRasterPos2f(200.0, 250.0);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Settings");
    glRasterPos2f(200.0, 200.0);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Help");
    glFlush();
} */
void initSound()
{
    alutInit(0, NULL);
    if (alGetError() != AL_NO_ERROR) {
        printf("ERROR: alutInit()\n");
        return;
    }
    //Clear error state.
    alGetError();
    //
    //Setup the listener.
    //Forward and up vectors are used.
    float vec[6] = {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f};
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, vec);
    alListenerf(AL_GAIN, 1.0f);
    //
    //Buffer holds the sound information.
    gl.alBufferDrip = alutCreateBufferFromFile("./sounds/Cannon2.wav");
    gl.alBufferTick = alutCreateBufferFromFile("./sounds/explode.wav");
    gl.alBufferWin = alutCreateBufferFromFile("./sounds/VictorySound.wav");
    gl.alBufferBackground = alutCreateBufferFromFile("./sounds/forest1.wav");
    //
    //Source refers to the sound.
    //Generate a source, and store it in a buffer.
    alGenSources(1, &gl.alSourceDrip);
    alSourcei(gl.alSourceDrip, AL_BUFFER, gl.alBufferDrip);
    //Set volume and pitch to normal, no looping of sound.
    alSourcef(gl.alSourceDrip, AL_GAIN, 1.0f);
    alSourcef(gl.alSourceDrip, AL_PITCH, 1.0f);
    alSourcei(gl.alSourceDrip, AL_LOOPING, AL_FALSE);
    if (alGetError() != AL_NO_ERROR) {
        printf("ERROR: setting source\n");
        return;
    }
    //Generate a source, and store it in a buffer.
    alGenSources(1, &gl.alSourceTick);
    alSourcei(gl.alSourceTick, AL_BUFFER, gl.alBufferTick);
    //Set volume and pitch to normal, no looping of sound.
    alSourcef(gl.alSourceTick, AL_GAIN, 1.0f);
    alSourcef(gl.alSourceTick, AL_PITCH, 1.0f);
    alSourcei(gl.alSourceTick, AL_LOOPING, AL_FALSE);
    if (alGetError() != AL_NO_ERROR) {
        printf("ERROR: setting source\n");
        return;
    }

    //Generate a source, and store it in a buffer.
    alGenSources(1, &gl.alSourceWin);
    alSourcei(gl.alSourceWin, AL_BUFFER, gl.alBufferWin);
    //Set volume and pitch to normal, no looping of sound.
    alSourcef(gl.alSourceWin, AL_GAIN, 1.0f);
    alSourcef(gl.alSourceWin, AL_PITCH, 1.0f);
    alSourcei(gl.alSourceWin, AL_LOOPING, AL_FALSE);
    if (alGetError() != AL_NO_ERROR) {
        printf("ERROR: setting source\n");
        return;
    }

    //Generate a source, and store it in a buffer.
    alGenSources(1, &gl.alSourceBackground);
    alSourcei(gl.alSourceBackground, AL_BUFFER, gl.alBufferBackground);
    //Set volume and pitch to normal, no looping of sound.
    alSourcef(gl.alSourceBackground, AL_GAIN, 1.0f);
    alSourcef(gl.alSourceBackground, AL_PITCH, 1.0f);
    alSourcei(gl.alSourceBackground, AL_LOOPING, AL_TRUE);
    if (alGetError() != AL_NO_ERROR) {
        printf("ERROR: setting source\n");
        return;
    }
}

void cleanupSound()
{
    //First delete the source.
    alDeleteSources(1, &gl.alSourceDrip);
    alDeleteSources(1, &gl.alSourceTick);
    alDeleteSources(1, &gl.alSourceWin);
    alDeleteSources(1, &gl.alSourceBackground);
    //Delete the buffer.
    alDeleteBuffers(1, &gl.alBufferDrip);
    alDeleteBuffers(1, &gl.alBufferTick);
    alDeleteBuffers(1, &gl.alBufferWin);
    alDeleteBuffers(1, &gl.alBufferBackground);
    //Close out OpenAL itself.
    //Get active context.
    ALCcontext *Context = alcGetCurrentContext();
    //Get device for active context.
    ALCdevice *Device = alcGetContextsDevice(Context);
    //Disable context.
    alcMakeContextCurrent(NULL);
    //Release context(s).
    alcDestroyContext(Context);
    //Close device.
    alcCloseDevice(Device);
}

void playSound(ALuint source)
{
    alSourcePlay(source);
}
void RvFeaturemode(){
    playSound(gl.alSourceWin); // Plays win Sound
        playSound(gl.alSourceBackground); // Plays background Sound
}
void renderFirework()
{
    const int numParticles = 100;
    static bool firstTime = true;
    static struct timespec startTime;
    static Particle particles[numParticles];

    if (firstTime) {
        clock_gettime(CLOCK_REALTIME, &startTime);
        firstTime = false;

        // Initialize particles
        for (int i = 0; i < numParticles; i++) {
            particles[i].color[0] = rnd() * 0.5f + 0.5f;
            particles[i].color[1] = rnd() * 0.5f + 0.5f;
            particles[i].color[2] = rnd() * 0.5f + 0.5f;

            particles[i].size = rnd() * 10.0f + 5.0f;

            float angle = rnd() * M_PI * 2.0f;
            float speed = rnd() * 200.0f + 100.0f;
            particles[i].velocity[0] = cos(angle) * speed;
            particles[i].velocity[1] = sin(angle) * speed;

            particles[i].acceleration[0] = 0.0f;
            particles[i].acceleration[1] = -100.0f;
        }
    }

    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);
    double dt = timeDiff(&startTime, &currentTime);

    glBegin(GL_POINTS);
    for (int i = 0; i < numParticles; i++) {
        if (particles[i].size > 0.0f) {
            particles[i].velocity[0] += particles[i].acceleration[0] * dt;
            particles[i].velocity[1] += particles[i].acceleration[1] * dt;
            particles[i].position[0] += particles[i].velocity[0] * dt;
            particles[i].position[1] += particles[i].velocity[1] * dt;

            glColor3fv(particles[i].color);
            glVertex2fv(particles[i].position);
            particles[i].size -= dt * 30.0f;
        }
    }
    glEnd();

    if (dt > 2.0) {
        firstTime = true;
    }
}

void drawArrowDown(float x, float y, float size)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 0);

    glBegin(GL_TRIANGLES);
    glVertex2f(-0.5, 0.0);
    glVertex2f(0.5, 0.0);
    glVertex2f(0.0, -1.0);
    glEnd();

    glPopMatrix();
}







	
