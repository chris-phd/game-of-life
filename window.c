#include "window.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Global window
struct Window window;

static void framebufferSizeCallback(GLFWwindow *handle, int width, int height) {
    glViewport(0, 0, width, height);
    window.size_x = width;
    window.size_y = height;
}

static void cursorPosCallback(GLFWwindow *handle, double xp, double yp) {
    double dx = xp - window.mouse.pos_x;
    double dy = yp - window.mouse.pos_y;
    window.mouse.dx = dx;
    window.mouse.dy = dy;
    window.mouse.pos_x = xp;
    window.mouse.pos_y = yp;
}   

static void mouseButtonCallback(GLFWwindow *handle, int button, int action, int mods) {
    if (button < 0)
        return;

    fprintf(stderr, "window::mouseButtonCallback: button = %d, mouse pos x = %f, mouse pos y = %f\n", button, window.mouse.pos_x, window.mouse.pos_y);

    switch (action) {
        case GLFW_PRESS:
            window.mouse.buttons[button].pressed = 1;
            break;
        case GLFW_RELEASE:
            window.mouse.buttons[button].pressed = 0;
            window.mouse.buttons[button].held = 0;
            break;
        default:
            break;
    }
}

static void mouseScrollCallback(GLFWwindow *handle, double xoffset, double yoffset)
{
    fprintf(stderr, "window::mouseScrollCallback: scroll = %f \n", window.mouse.scroll);
    window.mouse.scroll += yoffset;

    if (window.mouse.scroll > 100.0)
        window.mouse.scroll = 100.0;
    else if (window.mouse.scroll < 1.0)
        window.mouse.scroll = 1.0;
        
}

static void keyCallback(GLFWwindow *handle, int key, int scancode, int action, int mods) 
{
    if (key < 0)
        return;

    fprintf(stderr, "window::keyboardCallback: \n");

    switch (action) {
        case GLFW_PRESS:
            window.keyboard.keys[key].pressed = 1;
            break;
        case GLFW_RELEASE:
            window.keyboard.keys[key].pressed = 0;
            window.keyboard.keys[key].held = 0;
            break;
        default:
            break;
    }
}

static void mouseInit() 
{
    for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; ++i) {
        window.mouse.buttons[i].pressed = 0;
        window.mouse.buttons[i].held = 0;
    }
    window.mouse.scroll = 50.0;
    window.mouse.pos_x = 0.0;
    window.mouse.pos_y = 0.0;
    window.mouse.dx = 0.0;
    window.mouse.dy = 0.0;
}

static void keyboardInit()
{
    for (int i = 0; i < GLFW_KEY_LAST; ++i) {
        window.keyboard.keys[i].pressed = 0;
        window.keyboard.keys[i].held = 0;
    }
}

int windowInit() 
{
    fprintf(stderr, "window::windowInit: \n");
    
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialise GLEW.\n");
        return 0;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA (anti-aliasing)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Use OpenGL 3.3
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window handle
    window.fps.ticks_per_sec = 60.0f;
    window.fps.last_tick = timeNow();
    window.size_x = 1024;
    window.size_y = 768;
    window.handle = glfwCreateWindow(window.size_x, window.size_y, "Game of Life", NULL, NULL);
    if (!window.handle) {
        fprintf(stderr, "Failed to create GLFW window.\n");
        return 0;
    }
    glfwMakeContextCurrent(window.handle);

    // Load the GLFW functions for this OS
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialise GLAD.\n");
        return 0;
    }

    // Finish setting up the window
    glViewport(0, 0, 1024, 768);
    glfwSetFramebufferSizeCallback(window.handle, framebufferSizeCallback);
    glfwSetCursorPosCallback(window.handle, cursorPosCallback);
    glfwSetKeyCallback(window.handle, keyCallback);
    glfwSetMouseButtonCallback(window.handle, mouseButtonCallback);
    glfwSetScrollCallback(window.handle, mouseScrollCallback);

    mouseInit();
    keyboardInit();

    return 1;
}

void windowCleanup() {
    fprintf(stderr, "window::windowCleanup: \n");

    glfwTerminate();
}

void windowProcessInput() {
    if (glfwGetKey(window.handle, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window.handle, 1);
}

void windowLoop(struct Renderer *renderer, struct World *world) {
    fprintf(stderr, "window::windowLoop: \n");

    while (!glfwWindowShouldClose(window.handle)) {
        
        windowProcessInput();
        renderClear(renderer);
        renderWorld(renderer, world);
        glfwSwapBuffers(window.handle);

        if (hasNextTickPassed(&world->update_rate))
            worldUpdate(world);

        sleepTillNextTick(&window.fps);
        glfwPollEvents();
    }

}



