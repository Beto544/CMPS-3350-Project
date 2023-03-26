CFLAGS = -I ./include
##LIB    = ./libggfonts.so
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: artillery

artillery: artillery.cpp log.cpp timers.cpp global.cpp lgamboa.cpp hpascual.cpp ocastillo.cpp rvelasco.cpp sesquivel.cpp
	g++ $(CFLAGS) artillery.cpp log.cpp timers.cpp global.cpp lgamboa.cpp hpascual.cpp ocastillo.cpp rvelasco.cpp sesquivel.cpp libggfonts.a -Wall -Wextra $(LFLAGS) -oartillery

clean:
	rm -f artillery 
	rm -f *.o

