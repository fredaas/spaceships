#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "utils.h"

#define SPACESHIP_POINTS (int)4
#define SPACESHIP_MAX_SPEED (float)1
#define SPACESHIP_MIN_SPEED (float)0.0001

typedef struct Spaceship Spaceship;

enum { SPACESHIP_DEAD, SPACESHIP_ALIVE, SPACESHIP_IDLE };

struct Spaceship {
    float x[SPACESHIP_POINTS];
    float y[SPACESHIP_POINTS];
    float tx[SPACESHIP_POINTS];
    float ty[SPACESHIP_POINTS];
    float cx;
    float cy;
    float alpha;
    int dead;
    int state;
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
    void (*set_dead)(Spaceship *self);
};

void set_dead(Spaceship *self);
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
    self->h = rand_float(0.025, 0.05);
    self->state = SPACESHIP_ALIVE;
    self->dead = 0;
    self->alpha = 1.0;
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

/* Rotates the shape of the object by 'h' radians relative to the objects
   current direction */
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

int faded(Spaceship *self)
{
    self->alpha *= 0.9997;
    if (self->alpha < 0.01)
    {
        self->alpha = 0;
        return 1;
    }
    return 0;
}

void update(Spaceship *self)
{
    if (self->state == SPACESHIP_IDLE)
        return;

    if (self->state == SPACESHIP_ALIVE)
    {
        if (self->dead)
        {
            for (int i = 0; i < SPACESHIP_POINTS; i++)
            {
                self->tx[i] = self->cx;
                self->ty[i] = self->cy;
            }
            self->state = SPACESHIP_DEAD;
        }
        else
        {
            self->cx += cos(self->r)* self->acceleration;
            self->cy += sin(self->r)* self->acceleration;
        }
    }
    else if (self->state == SPACESHIP_DEAD)
    {
        double r[SPACESHIP_POINTS] = {
            -(1 / 4.0) * PI + self->r,
            -(3 / 4.0) * PI + self->r,
             (3 / 4.0) * PI + self->r,
             (1 / 4.0) * PI + self->r,
        };
        for (int i = 0; i < SPACESHIP_POINTS; i++)
        {
            self->tx[i] += cos(r[i]) * 0.01;
            self->ty[i] += sin(r[i]) * 0.01;
        }
    }
}

void draw(Spaceship *self)
{
    if (self->state == SPACESHIP_IDLE)
        return;

    glColor4f(self->color[0], self->color[1], self->color[2], self->alpha);
    glLineWidth(1.5f);
    glBegin(GL_LINES);

    if (self->state == SPACESHIP_ALIVE)
    {
        for (int i = 0; i < SPACESHIP_POINTS; i++)
        {
            glVertex2f(
                self->x[i] + self->cx,
                self->y[i] + self->cy
            );
            glVertex2f(
                self->x[(i + 1) % SPACESHIP_POINTS] + self->cx,
                self->y[(i + 1) % SPACESHIP_POINTS] + self->cy
            );
        }
    }
    else if (self->state == SPACESHIP_DEAD)
    {
        if (faded(self))
            self->state = SPACESHIP_IDLE;

        for (int i = 0; i < SPACESHIP_POINTS; i++)
        {
            glVertex2f(
                self->x[i] + self->tx[i],
                self->y[i] + self->ty[i]
            );
            glVertex2f(
                self->x[(i + 1) % SPACESHIP_POINTS] + self->tx[i],
                self->y[(i + 1) % SPACESHIP_POINTS] + self->ty[i]
            );
        }
    }

    glEnd();
}

#endif /* SPACESHIP_H */
