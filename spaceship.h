#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "utils.h"

#define SPACESHIP_POINTS (int)4
#define SPACESHIP_MAX_SPEED (float)1
#define SPACESHIP_MIN_SPEED (float)0.0001

typedef struct Spaceship Spaceship;

struct Spaceship {
    float x[SPACESHIP_POINTS];
    float y[SPACESHIP_POINTS];
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
    self->x[0] = 0;
    self->y[0] = 15.0;
    self->x[1] = 10.0;
    self->y[1] = -15.0;
    self->x[2] = 0;
    self->y[2] = -10.0;
    self->x[3] = -10.0;
    self->y[3] = -15.0;
}

void rotate(Spaceship *self, double h)
{
    float prev_x[4];
    float prev_y[4];

    memcpy(prev_x, self->x, 4 * sizeof(float));
    memcpy(prev_y, self->y, 4 * sizeof(float));

    for (int i = 0; i < SPACESHIP_POINTS; i++)
    {
        self->x[i] = rotate_x(prev_x[i], prev_y[i], h);
        self->y[i] = rotate_y(prev_x[i], prev_y[i], h);
    }

    self->r += h;

    if (self->r > 2 * PI)
        self->r -= 2 * PI;
    else if (self->r < 0)
        self->r += 2 * PI;
}

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
    /* TODO: Look into glDrawArrays for more efficient line drawing */
    glColor3f(self->color[0], self->color[1], self->color[2]);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    for (int i = 0; i < SPACESHIP_POINTS; i++)
    {
        glVertex2f(self->x[i] + self->cx, self->y[i] + self->cy);
        glVertex2f(
            self->x[(i + 1) % SPACESHIP_POINTS] + self->cx,
            self->y[(i + 1) % SPACESHIP_POINTS] + self->cy
        );
    }
    glEnd();
}

#endif /* SPACESHIP_H */
