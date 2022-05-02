#ifndef __GAME_OF_LIFE_WINDOW_H__
#define __GAME_OF_LIFE_WINDOW_H__

#include "renderer.h"
#include "world.h"
#include "time_control.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define SCROLL_MAX 200.0f
#define SCROLL_MIN 1.0f

struct Button {
    unsigned char pressed;
    unsigned char held;
};

struct Mouse {
    struct Button buttons[GLFW_MOUSE_BUTTON_LAST]; 
    double scroll;
    double pos_x;
    double pos_y;
    double dx;
    double dy;
};

struct Keyboard {
    struct Button keys[GLFW_KEY_LAST];
};

struct Window {
    GLFWwindow *handle;
    int size_x;
    int size_y;

    struct TimeControl fps;

    struct Mouse mouse;
    struct Keyboard keyboard;
};

int windowInit();
void windowCleanup();
void windowLoop(struct Renderer *renderer, struct World *world);
void processInput();

#endif // __GAME_OF_LIFE_WINDOW_H__
