#include "utils.h"
#include "spaceship.h"

#define FPS 60
#define UPDATE_RATE 1000 / (float)FPS
#define NUM_SPACESHIPS 200

enum {
    MOUSE_LEFT,
    MOUSE_RIGHT
};

int mouse_down[2];
int mouse_released[2];
int mouse_pressed[2];

float mx = 0.0,
      my = 0.0;

int window_w = 1500,
    window_h = 1000,
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
            for (int i = 0; i < NUM_SPACESHIPS; i++)
            {
                Spaceship *s = spaceships[i];
                s->dead = 1;
            }
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
        NULL, NULL);

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

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
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

    initialize();

    double start = walltime();

    while (!glfwWindowShouldClose(window))
    {
        glfwGetFramebufferSize(window, &window_w, &window_h);
        glViewport(0, 0, window_w, window_h);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0, window_w, 0, window_h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double delay = (walltime() - start) * 1.0e+3;
        if (delay > UPDATE_RATE)
        {
            start = walltime();
            for (int i = 0; i < NUM_SPACESHIPS; i++)
            {
                Spaceship *s = spaceships[i];
                if (!s->dead)
                    s->rotate_toward(s, mx, my);
            }
        }

        for (int i = 0; i < NUM_SPACESHIPS; i++)
        {
            Spaceship *s = spaceships[i];
            s->update(s);
            s->draw(s);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
