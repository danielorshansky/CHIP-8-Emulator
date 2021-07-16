#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "SDL2/SDL.h"

#define BASE_WIDTH 64
#define BASE_HEIGHT 32
#define DISPLAY_SIZE BASE_WIDTH * BASE_HEIGHT

struct Display {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int scale;
    int width;
    int height;
    int* pixels;
};

void init_graphics(struct Display* display, int scale) {
    display->scale = scale;
    display->width = BASE_WIDTH * scale;
    display->height = BASE_HEIGHT * scale;
    display->pixels = calloc(DISPLAY_SIZE, sizeof(int));
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "SDL initialization error: %s", SDL_GetError());
        exit(1);
    }
    display->window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, display->width, display->height, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if (display->window == NULL) {
        fprintf(stderr, "Window creation error: %s", SDL_GetError());
        exit(1);
    }
    display->renderer = SDL_CreateRenderer(display->window, -1, SDL_RENDERER_ACCELERATED);
    if (display->renderer == NULL) {
        fprintf(stderr, "Renderer creation error: %s", SDL_GetError());
        exit(1);
    }
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);
    SDL_ShowWindow(display->window);
}

void clear(struct Display* display) {
    memset(display->pixels, 0, sizeof(int) * DISPLAY_SIZE);
}

void render(struct Display* display) {
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);
    SDL_RenderClear(display->renderer);

    SDL_SetRenderDrawColor(display->renderer, 255, 255, 255, 255);

    for (int i = 0; i < DISPLAY_SIZE; i++) {
        int x = i % 64;
        int y = floor(i / 64);
        if (display->pixels[i]) {
            SDL_Rect rect = {x * display->scale, y * display->scale, display->scale, display->scale};
            SDL_RenderFillRect(display->renderer, &rect);
        }
    }

    SDL_RenderPresent(display->renderer);
}

int toggle_pixel(struct Display* display, int x, int y) {
    if (x > BASE_WIDTH || x < 0) {
        x = x % BASE_WIDTH;
    }
    if (y > BASE_HEIGHT || y < 0) {
        y = y % BASE_HEIGHT;
    }
    display->pixels[y * BASE_WIDTH + x] = display->pixels[y * BASE_WIDTH + x] ^ 1;
    return display->pixels[y * BASE_WIDTH + x] ^ 1;
}

void destroy_graphics(struct Display* display) {
    free(display->pixels);
    SDL_DestroyRenderer(display->renderer);
    SDL_DestroyWindow(display->window);
    SDL_Quit();
}
