
#include "window.h"

#include <SDL2/SDL.h>
#include <GL/glx.h>
#include "opengl_local.h"
#include "video.h"

SDL_Window    *window;
SDL_GLContext *gl;
SDL_DisplayMode mode;

bool window_init(int width, int height, bool fullscreen)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow("Wolfenstein 3D",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              width,
                              height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!window) {
        printf("Error while creating the window: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetWindowGrab(window, SDL_TRUE);

    gl = SDL_GL_CreateContext(window);

    if (!gl) {
        printf("Error initializing OpenGL: %s\n", SDL_GetError());
        return false;
    }

    SDL_GL_MakeCurrent(window, gl);
    SDL_GL_SetSwapInterval(1);

    if (SDL_GetWindowDisplayMode(window, &mode) < 0) {
        printf("Error while getting the display mode: %s\n", SDL_GetError());
    }

    SDL_ShowCursor(0);

    viddef.height = height;
    viddef.width  = width;

    return true;
}

bool window_set_fullscreen()
{
    if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) < 0) {
        printf("Error while setting fullscreen: %s\n",
                         SDL_GetError());
    }
}

/*
void window_set_windowed()
{
    if (SDL_SetWindowFullscreen(window, 0) < 0) {

    }
    SDL_SetWindowSize(window, 1024, 768);
    SDL_GetWindowSize(window, &w, &h);
}
*/

bool window_set_fullscreen_windowed()
{
    if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) < 0)
        printf("Error while setting fullscreen window: %s\n", SDL_GetError());
}

void window_buffer_swap(void)
{
    glFlush();
    SDL_GL_SwapWindow(window);
}
