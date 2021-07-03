#include <time.h>

#include "SDL2/SDL.h"

#include "graphics.h"
#include "keyboard.h"
#include "audio.h"
#include "cpu.h"

int main(int argc, char *argv[]) {
    srand(time(0));

    struct Display display;
    init_graphics(&display, 10);

    struct Keyboard keyboard;
    init_keyboard(&keyboard);

    struct AudioData audio;
    init_audio(&audio);

    struct CPU cpu;

    float frame = 1000 / 60.0;
    uint32_t last_time = SDL_GetTicks();
    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }
        handle_input(&keyboard, cpu.paused, cpu.v[cpu.x]);

        uint32_t time = SDL_GetTicks();
        if (time - last_time > frame) {
            render(&display); // TODO: maybe put outside of frame controlled area? well see
            last_time = time;
        }
    }

    destroy_graphics(&display);

    return 0;
}
