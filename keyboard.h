#pragma once

#include <stdlib.h>

#include "SDL2/SDL.h"

#define NUM_KEYS 16

int keys[NUM_KEYS] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
int keybinds[NUM_KEYS] = {SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8, SDL_SCANCODE_9, SDL_SCANCODE_A, SDL_SCANCODE_B, SDL_SCANCODE_C, SDL_SCANCODE_D, SDL_SCANCODE_E, SDL_SCANCODE_F};

struct Keyboard {
    int* pressed;
    int on_next_press;
};

void init_keyboard(struct Keyboard* keyboard) {
    keyboard->pressed = calloc(NUM_KEYS, sizeof(int));
    keyboard->on_next_press = 0;
}

int key_pressed(struct Keyboard* keyboard, int index) {
    return keyboard->pressed[index];
}

void handle_input(struct Keyboard* keyboard, int* paused, uint8_t* vx) {
    const uint8_t* keystate = SDL_GetKeyboardState(NULL);
    for (int i = 0; i < NUM_KEYS; i++) {
        if (keyboard->pressed[i] != keystate[keybinds[i]]) {
            keyboard->pressed[i] = keystate[keybinds[i]];
            if (keyboard->pressed[i]) {
                if (keyboard->on_next_press) {
                    *paused = 0;
                    *vx = keys[i];
                    keyboard->on_next_press = 0;
                }
            }
        }
    }
}
