#include "utils.h"
#include "spaceship.h"

#define FPS 60
#define UPDATE_RATE 1000 / (float)FPS
#define NUM_SPACESHIPS 1000

enum {
    MOUSE_LEFT,
    MOUSE_RIGHT
};

int mouse_down[2];
int mouse_released[2];
int mouse_pressed[2];

float mx = 0.0,
      my = 0.0;

int window_w = 1920,
    window_h = 1080,
    world_w  = 1500,
    world_h  = 1500;

GLFWwindow* window;

Spaceship *spaceships[NUM_SPACESHIPS];


/*******************************************************************************
 *
 * I/O
 *
 ******************************************************************************/


static void key_callback(GLFWwindow* window, int key, int scancode, int action,
    int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_Q:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        case GLFW_KEY_W:
            break;
        case GLFW_KEY_A:
            break;
        case GLFW_KEY_S:
            break;
        case GLFW_KEY_D:
            break;
        }
    }
}

static void cursor_position_callback(GLFWwindow *window, double x, double y)
{
    mx = x;
    my = window_h - y;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
            mouse_down[MOUSE_RIGHT] = 1;
        if (button == GLFW_MOUSE_BUTTON_LEFT)
            mouse_pressed[MOUSE_LEFT] = 1;
    }
    if (action == GLFW_RELEASE)
    {
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
            mouse_down[MOUSE_RIGHT] = 0;
        if (button == GLFW_MOUSE_BUTTON_LEFT)
            mouse_released[MOUSE_LEFT] = 1;
    }
}

/* Returns 1 once when 'key' is pressed, 0 otherwise */
int is_mouse_pressed(int key)
{
    int key_state = 0;
    switch (key)
    {
    case MOUSE_LEFT:
        key_state = mouse_pressed[MOUSE_LEFT];
        mouse_pressed[MOUSE_LEFT] = 0;
        return key_state;
    }

    return -1;
}

/* Returns 1 once when 'key' is released, 0 otherwise */
int is_mouse_released(int key)
{
    int key_state = 0;
    switch (key)
    {
    case MOUSE_LEFT:
        key_state = mouse_released[MOUSE_LEFT];
        mouse_released[MOUSE_LEFT] = 0;
        return key_state;
    }

    return -1;
}

/* Returns 1 as long as 'key' is down, 0 otherwise */
int is_mouse_down(int key)
{
    switch (key)
    {
    case MOUSE_RIGHT:
        return mouse_down[MOUSE_RIGHT];
    }

    return -1;
}


/*******************************************************************************
 *
 * Utils
 *
 ******************************************************************************/


void center_window(GLFWwindow *window)
{
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();

    if (!monitor)
        return;

    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    if (!mode)
        return;

    int monitor_x, monitor_y;
    glfwGetMonitorPos(monitor, &monitor_x, &monitor_y);

    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);

    glfwSetWindowPos(
        window,
        monitor_x + (mode->width - window_width) / 2,
        monitor_y + (mode->height - window_height) / 2
    );
}

double walltime(void)
{
    static struct timeval t;
    gettimeofday(&t, NULL);
    return ((double)t.tv_sec + (double)t.tv_usec * 1.0e-06);
}

void initialize(void)
{
    /* Configure GLFW */

    if (!glfwInit())
    {
        printf("[ERROR] Failed to initialize glfw\n");
        exit(1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(window_w, window_h, "GLFW Window",
        glfwGetPrimaryMonitor(), NULL);

    if (!window)
    {
        printf("[ERROR] Failed to initialize window\n");
        exit(1);
    }
    center_window(window);
    glfwSwapInterval(1);
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    /* Configure OpenGL */

    glEnable(GL_MULTISAMPLE);

    glfwGetFramebufferSize(window, &window_w, &window_h);
    glViewport(0, 0, window_w, window_h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, window_w, 0, window_h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    /* Init spaceships */

    for (int i = 0; i < NUM_SPACESHIPS; i++)
    {
        Spaceship *s = init_spaceship(
            rand_float(0, window_w),
            rand_float(0, window_h),
            0.0
        );
        s->set_direction(s, mx, my);
        spaceships[i] = s;
    }
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    int n_threads = 0;
    #pragma omp parallel
    {
        #pragma omp atomic
        n_threads++;
    }
    printf("%d\n", n_threads);

    initialize();

    double start = walltime();

    double tx = mx,
           ty = my;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        tx = mx;
        ty = my;

        double delay = (walltime() - start) * 1.0e+3;
        if (delay > UPDATE_RATE)
        {
            start = walltime();
            #pragma omp parallel for
            for (int i = 0; i < NUM_SPACESHIPS; i++)
            {
                Spaceship *s = spaceships[i];
                s->rotate_toward(s, tx, ty);
                s->dx = s->acceleration * cos(s->r);
                s->dy = s->acceleration * sin(s->r);
            }
        }

        #pragma omp parallel for
        for (int i = 0; i < NUM_SPACESHIPS; i++)
        {
            Spaceship *s = spaceships[i];
            s->update(s);
        }

        for (int i = 0; i < NUM_SPACESHIPS; i++)
        {
            Spaceship *s = spaceships[i];
            s->draw(s);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
