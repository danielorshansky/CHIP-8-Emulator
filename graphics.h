#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "SDL2/SDL.h"

struct Display {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int scale;
    int width;
    int height;
    int size;
    int* pixels;
};

void init_graphics(struct Display* display, int scale) {
    display->scale = scale;
    display->width = 64 * scale;
    display->height = 32 * scale;
    display->size = display->width * display->height;
    display->pixels = calloc(display->size, sizeof(int));
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "SDL Initialization Error: %s", SDL_GetError());
        exit(1);
    }
    display->window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, display->width, display->height, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if (display->window == NULL) {
        fprintf(stderr, "Window Creation Error: %s", SDL_GetError());
        exit(1);
    }
    display->renderer = SDL_CreateRenderer(display->window, -1, SDL_RENDERER_ACCELERATED);
    if (display->renderer == NULL) {
        fprintf(stderr, "Renderer Creation Error: %s", SDL_GetError());
        exit(1);
    }
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);
    SDL_ShowWindow(display->window);
}

void clear(struct Display* display) {
    memset(display->pixels, 0, sizeof(int) * display->size);
}

void render(struct Display* display) {
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);
    SDL_RenderClear(display->renderer);

    SDL_SetRenderDrawColor(display->renderer, 255, 255, 255, 255);

    for (int i = 0; i < display->size; i++) {
        int x = (i % 64) * display->scale;
        int y = floor(i / 64) * display->scale;
        if (display->pixels[i]) {
            SDL_Rect rect = {(i % 64) * display->scale, floor(i / 64) * display->scale, display->scale, display->scale};
            SDL_RenderDrawRect(display->renderer, &rect);
        }
    }

    SDL_RenderPresent(display->renderer);
}

int toggle_pixel(struct Display* display, int x, int y) {
    if (x > display->width || x < 0) {
        x = x % display->width;
    }
    if (y > display->height || y < 0) {
        y = y % display->height;
    }
    display->pixels[y * display->width + x] = display->pixels[y * display->width + x] ^ 1;
    return display->pixels[y * display->width + x] ^ 1;
}

void destroy_graphics(struct Display* display) {
    free(display->pixels);
    SDL_DestroyRenderer(display->renderer);
    SDL_DestroyWindow(display->window);
    SDL_Quit();
}
