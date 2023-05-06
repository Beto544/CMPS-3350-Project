CFLAGS = -I ./include
##LIB    = ./libggfonts.so
LFLAGS = -lrt -lX11 -lglut -lGLU -lGL -pthread -lm #-lXrandr

all: artillery

artillery: artillery.cpp log.cpp timers.cpp global.cpp image.cpp lgamboa.cpp hpascual.cpp ocastillo.cpp rvelasco.cpp sesquivel.cpp
	g++ $(CFLAGS) artillery.cpp log.cpp timers.cpp global.cpp image.cpp lgamboa.cpp hpascual.cpp ocastillo.cpp rvelasco.cpp sesquivel.cpp libggfonts.a /usr/lib/x86_64-linux-gnu/libopenal.so \
	/usr/lib/x86_64-linux-gnu/libalut.so \-Wall -Wextra $(LFLAGS) -oartillery

clean:
	rm -f artillery 
	rm -f *.o

