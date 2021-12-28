#include "fileio.h"

#include <stdio.h>
#include <glad/glad.h>  // OpenGL loading library. Must be included before glfw
#include <GLFW/glfw3.h> // Multiplatform library for OpenGL


#define MAX_SHADER_LEN 4096

GLuint createProgram(const char *vs_path, const char *fs_path);
static GLuint createShader(const char *shader_source, GLenum shader_type);
GLuint createShaderProgram(const char *vs_path, const char *fs_path);
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

int main(int argc, char *argv[]) {
    fprintf(stderr, "game-of-life::main: \n");

    // Initialise GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialise GLFW\n");
        return -1;
    }

    // Sets hints for the next call to createWindow
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA (multi-sample anti aliassing)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Use openGL 3.3
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Often make this a global for convinience
    GLFWwindow *window = glfwCreateWindow(1024, 768, "Game of Life", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        return -1;
    }
    glfwMakeContextCurrent(window);

    // GLAD mangaes the function pointers for different OS
    // Do not need to use GLAD, but must use something to load everything
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialise GLAD\n");
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // A program contains the shaders that make up the graphics pipeline.
    // The vertex and fragment shaders are the minimu shaders that the programmer must specify
    GLuint program_id = createShaderProgram("../basic_color.vs", "../basic_color.fs");
    glUseProgram(program_id);

    glClearColor(0.2, 0.3, 0.3, 1.0);

    // Loop until the the window is closed by the user
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Clear the window using the color set in glClearColor
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Swap the fronty back buffer. We draw on the back buffer and then
        // swap to the front for rendering
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // clean up
    glDeleteProgram(program_id);

    glfwTerminate();
    return 0;
}

GLuint createShaderProgram(const char *vs_path, const char *fs_path) { // todo, add attributes as an input var...

    // Create the vertex shader
    char vs_shader_source[MAX_SHADER_LEN];
    readFile(vs_path, MAX_SHADER_LEN, vs_shader_source);
    GLuint vs_id = createShader(vs_shader_source, GL_VERTEX_SHADER);
    if (!vs_id) {
        fprintf(stderr, "Failed to create vertex shader: %s\n", vs_path);
        return 0;
    }

    // Create the fragment shader
    char fs_shader_source[MAX_SHADER_LEN];
    readFile(fs_path, MAX_SHADER_LEN, fs_shader_source);
    GLuint fs_id = createShader(fs_shader_source, GL_FRAGMENT_SHADER);
    if (!fs_id) {
        fprintf(stderr, "Failed to create fragment shader: %s\n", fs_path);
        return 0;
    }

    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vs_id);
    glAttachShader(program_id, fs_id);

    // Bind the attributes here when you need to

    glLinkProgram(program_id);
    GLuint success;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        // Should really find a way to check the length of the info log
        // glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infoLogLength);
        char infoLog[512];
        glGetProgramInfoLog(program_id, 512, NULL, infoLog);
        fprintf(stderr, "Failed to link shader: vs = %s, fs = %s)\n", vs_path, fs_path);
        fprintf(stderr, "    %s\n", infoLog);
        return 0;
    }

    glDetachShader(program_id, vs_id);
    glDetachShader(program_id, fs_id);
    glDeleteShader(vs_id);
    glDeleteShader(fs_id);

    return program_id;
}

/// Returns the handle to the created shader. Returns 0 if failed to create and compile.
/// GLenum = GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
static GLuint createShader(const char *shader_source, GLenum shader_type) {
    unsigned int shader_id = glCreateShader(shader_type);
    glShaderSource(shader_id, 1, &shader_source, NULL);
    glCompileShader(shader_id);

    int success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader_id, 512, NULL, infoLog);
        fprintf(stderr, "Failed to compile shader: \n%s\n", infoLog);
        return 0;
    }

    return shader_id;
}

/// Callback to resize the window
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}


/// Deal with key inputs in the main loop
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}
