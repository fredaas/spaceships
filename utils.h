#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <omp.h>

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>

/* Rotate (x, y) around (0, 0) by r radians */
#define rotate_x(x, y, r) (x * cos(r) - y * sin(r))
#define rotate_y(x, y, r) (x * sin(r) + y * cos(r))

#define PI (double)3.141592654

#define rand_float(a, b) (a) + (rand() / (float) RAND_MAX) * ((b) - (a))

#define msec(sec) (double)((sec) * 1.0e+3)
#define usec(sec) (double)((sec) * 1.0e+6)

double walltime(void)
{
    static struct timeval t;
    gettimeofday(&t, NULL);
    return ((double)t.tv_sec + (double)t.tv_usec * 1.0e-06);
}

extern int window_w;
extern int window_h;
extern int world_w;
extern int world_h;

#endif /* UTILS_H */
