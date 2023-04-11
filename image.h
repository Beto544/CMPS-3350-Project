// Hunberto Pascual
// Image header file
#ifndef _IMAGE_H_
#define _IMAGE_H_
#include <cstdio>
#include <cstring>
#include <cstdlib>


class Image {
    public:
        int width, height;
        unsigned char *data;
        ~Image() { delete [] data; }
        Image(const char *fname); 
        //}
        };

#endif
