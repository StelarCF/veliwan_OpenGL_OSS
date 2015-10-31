#if defined(_WIN32)
#include <SDL.h>
#pragma once
#elif defined(__linux__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#elif defined(__APPLE__)
#error Apples in Bloom
#endif

#include <GL/glew.h>