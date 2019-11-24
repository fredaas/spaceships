#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "utils.h"

#define SPACESHIP_POINTS (int)4
#define SPACESHIP_MAX_SPEED (float)1
#define SPACESHIP_MIN_SPEED (float)0.0001

typedef struct Spaceship Spaceship;

struct Spaceship {
    float shape[SPACESHIP_POINTS * 2];
    float cx;
    float cy;
    float dx;
    float dy;
    double r;
    double h;
    float color[3];
    float acceleration;
    void (*draw)(Spaceship *self);
    void (*rotate_left)(Spaceship *self);
    void (*rotate_right)(Spaceship *self);
    void (*rotate_toward)(Spaceship *self, float x, float y);
    void (*update)(Spaceship *self);
    void (*set_direction)(Spaceship *self, float x, float y);
};

void draw(Spaceship *self);
void rotate(Spaceship *self, double h);
void rotate_left(Spaceship *self);
void rotate_right(Spaceship *self);
void rotate_toward(Spaceship *self, float x, float y);
void set_angle(Spaceship *self, double r);
void set_direction(Spaceship *self, float x, float y);
void set_shape(Spaceship *self);
void update(Spaceship *self);

Spaceship * init_spaceship(float cx, float cy, double r)
{
    Spaceship *self = (Spaceship *)malloc(sizeof(Spaceship));

    self->r = PI / 2.0;
    self->cx = cx;
    self->cy = cy;
    self->dx = 0.0;
    self->dy = 0.0;
    self->h = rand_float(0.025, 0.05);
    self->acceleration = rand_float(0.1, 0.15);

    self->color[0] = rand_float(0, 1);
    self->color[1] = rand_float(0, 1);
    self->color[2] = rand_float(0, 1);

    set_shape(self);
    set_angle(self, r);

    self->draw = draw;
    self->rotate_right = rotate_right;
    self->rotate_left = rotate_left;
    self->update = update;
    self->rotate_toward = rotate_toward;
    self->set_direction = set_direction;

    return self;
}

void set_shape(Spaceship *self)
{
    float shape[SPACESHIP_POINTS * 2] = {
        0, 15.0, 10.0, -15.0, 0, -10.0, -10.0, -15.0
    };
    memcpy(self->shape, shape, SPACESHIP_POINTS * 2 * sizeof(float));
}

/* Rotates the shape of the object by 'h' radians relative to the objects
   current direction */
void rotate(Spaceship *self, double h)
{
    float prev_shape[SPACESHIP_POINTS * 2];
    memcpy(prev_shape, self->shape, SPACESHIP_POINTS * 2 * sizeof(float));

    for (int x = 0, y = 1; y < SPACESHIP_POINTS * 2; x += 2, y += 2)
    {
        self->shape[x] = rotate_x(prev_shape[x], prev_shape[y], h);
        self->shape[y] = rotate_y(prev_shape[x], prev_shape[y], h);
    }

    self->r += h;

    if (self->r > 2 * PI)
        self->r -= 2 * PI;
    else if (self->r < 0)
        self->r += 2 * PI;
}

/* Sets the current direction of the object to point to (x, y) */
void set_direction(Spaceship *self, float x, float y)
{
    double tv[2] = { x - self->cx, y - self->cy };
    double d = 1.0 / sqrt(tv[0] * tv[0] + tv[1] * tv[1]);
    tv[0] *= d;
    tv[1] *= d;

    double sv[2] = { cos(self->r), sin(self->r) };

    double h = acos(sv[0] * tv[0] + sv[1] * tv[1]);

    if (h < 1 * PI / 180.0)
        return;

    int k = ((tv[0] * sv[1] - tv[1] * sv[0]) < 0) ? 1 : -1;

    rotate(self, k * h);
}

/* Rotates the object towards (x, y) by 'self->h' radians */
void rotate_toward(Spaceship *self, float x, float y)
{
    double tv[2] = { x - self->cx, y - self->cy };
    double d = 1.0 / sqrt(tv[0] * tv[0] + tv[1] * tv[1]);
    tv[0] *= d;
    tv[1] *= d;

    double sv[2] = { cos(self->r), sin(self->r) };

    double h = acos(sv[0] * tv[0] + sv[1] * tv[1]);

    if (h < 1 * PI / 180.0)
        return;

    int k = ((tv[0] * sv[1] - tv[1] * sv[0]) < 0) ? 1 : -1;

    rotate(self, k * self->h);
}

/* Sets the current direction of the object to 'r' radians */
void set_angle(Spaceship *self, double r)
{
    rotate(self, r - self->r);
}

void rotate_left(Spaceship *self)
{
    rotate(self, self->h);
}

void rotate_right(Spaceship *self)
{
    rotate(self, -self->h);
}

void update(Spaceship *self)
{
    self->cx += self->dx;
    self->cy += self->dy;

    float abs_dx = fabs(self->dx);
    float abs_dy = fabs(self->dy);

    if (abs_dx > SPACESHIP_MAX_SPEED)
        self->dx = SPACESHIP_MAX_SPEED;
    else if (abs_dx < SPACESHIP_MIN_SPEED)
        self->dx = 0.0;
    if (abs_dy > SPACESHIP_MAX_SPEED)
        self->dy = SPACESHIP_MAX_SPEED;
    else if (abs_dy < SPACESHIP_MIN_SPEED)
        self->dy = 0.0;
}

void draw(Spaceship *self)
{
    float shape[SPACESHIP_POINTS * 2];
    memcpy(shape, self->shape, SPACESHIP_POINTS * 2 * sizeof(float));
    for (int x = 0, y = 1; y < SPACESHIP_POINTS * 2; x += 2, y += 2)
    {
        shape[x] += self->cx;
        shape[y] += self->cy;
    }

    glColor3fv(self->color);
    glVertexPointer(2, GL_FLOAT, 0, shape);
    glDrawArrays(GL_LINE_LOOP, 0, SPACESHIP_POINTS);
}

#endif /* SPACESHIP_H */
