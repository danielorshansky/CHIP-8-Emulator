#pragma once

#include <stdio.h>
#include <math.h>

#include "audio.h"
#include "graphics.h"
#include "keyboard.h"

struct CPU {
    uint8_t* memory;
    uint8_t* v;
    uint16_t* stack;
    uint16_t* sp;
    int delay_timer;
    int sound_timer;
    int pc;
    int paused;
    int speed;
    int i;
    int x;
    int y;
    struct Display* display;
    struct Keyboard* keyboard;
    struct AudioData* audio;
};

void init_cpu(struct CPU* cpu, struct Display* display, struct Keyboard* keyboard, struct AudioData* audio) {
    cpu->memory = (uint8_t*)malloc(4096);
    cpu->v = (uint8_t*)malloc(16);
    cpu->stack = (uint16_t*)malloc(sizeof(uint16_t) * 16);
    cpu->sp = cpu->stack;
    cpu->delay_timer = 0;
    cpu->sound_timer = 0;
    cpu->pc = 0x200;
    cpu->paused = 0;
    cpu->speed = 10;
    cpu->i = 0;
    cpu->display = display;
    cpu->keyboard = keyboard;
    cpu->audio = audio;
}

void load_sprites(struct CPU* cpu) {
    int sprites[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    for (int i = 0; i < 80; i++) {
        cpu->memory[i] = sprites[i];
    }
}

void load_rom(struct CPU* cpu, char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open ROM");
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    if (size > 4096 - 0x200) {
        fprintf(stderr, "ROM is too large");
        exit(1);
    }
    fread(cpu->memory + 0x200, 1, size, file);
    fclose(file);
}

void update_timers(struct CPU* cpu) {
    if (cpu->delay_timer > 0) {
        cpu->delay_timer--;
    }
    if (cpu->sound_timer > 0) {
        cpu->sound_timer--;
    }
}

void play_sound(struct CPU* cpu) {
    if (cpu->sound_timer > 0) {
        play(cpu->audio);
    }
    else {
        pause(cpu->audio);
    }
}

void execute(struct CPU* cpu, int opcode) {
    cpu->pc += 2;
    cpu->x = (opcode & 0x0F00) >> 8;
    cpu->y = (opcode & 0x00F0) >> 4;
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode) {
                case 0x00E0:
                    clear(cpu->display);
                    break;
                case 0x00EE:
                    cpu->pc = *cpu->sp;
                    cpu->sp--;
                    break;
            }
            break;
        case 0x1000:
            cpu->pc = opcode & 0xFFF;
            break;
        case 0x2000:
            cpu->sp++;
            *cpu->sp = cpu->pc;
            cpu->pc = opcode & 0xFFF;
            break;
        case 0x3000:
            if (cpu->v[cpu->x] == (opcode & 0xFF)) {
                cpu->pc += 2;
            }
            break;
        case 0x4000:
            if (cpu->v[cpu->x] != (opcode & 0xFF)) {
                cpu->pc += 2;
            }
            break;
        case 0x5000:
            if (cpu->v[cpu->x] == cpu->v[cpu->y]) {
                cpu->pc += 2;
            }
            break;
        case 0x6000:
            cpu->v[cpu->x] = opcode & 0xFF;
            break;
        case 0x7000:
            cpu->v[cpu->x] += opcode & 0xFF;
            break;
        case 0x8000:
            switch (opcode & 0xF) {
                case 0x0:
                    cpu->v[cpu->x] = cpu->v[cpu->y];
                    break;
                case 0x1:
                    cpu->v[cpu->x] = cpu->v[cpu->x] | cpu->v[cpu->y];
                    break;
                case 0x2:
                    cpu->v[cpu->x] = cpu->v[cpu->x] & cpu->v[cpu->y];
                    break;
                case 0x3:
                    cpu->v[cpu->x] = cpu->v[cpu->x] ^ cpu->v[cpu->y];
                    break;
                case 0x4:
                    cpu->v[0xF] = 0;
                    if (cpu->v[cpu->x] + cpu->v[cpu->y] > 0xFF) {
                        cpu->v[0xF] = 1;
                    }
                    cpu->v[cpu->x] += cpu->v[cpu->y];
                    break;
                case 0x5:
                    cpu->v[0xF] = 0;
                    if (cpu->v[cpu->x] > cpu->v[cpu->y]) {
                        cpu->v[0xF] = 1;
                    }
                    cpu->v[cpu->x] -= cpu->v[cpu->y];
                    break;
                case 0x6:
                    cpu->v[0xF] = cpu->v[cpu->x] & 0x1;
                    cpu->v[cpu->x] = cpu->v[cpu->x] >> 1;
                    break;
                case 0x7:
                    cpu->v[0xF] = 0;
                    if (cpu->v[cpu->x] < cpu->v[cpu->y]) {
                        cpu->v[0xF] = 1;
                    }
                    cpu->v[cpu->x] = cpu->v[cpu->y] - cpu->v[cpu->x];
                    break;
                case 0xE:
                    cpu->v[0xF] = cpu->v[cpu->x] & 0x80;
                    cpu->v[cpu->x] = cpu->v[cpu->x] << 1;
                    break;
            }
            break;
        case 0x9000:
            if (cpu->v[cpu->x] != cpu->v[cpu->y]) {
                cpu->pc += 2;
            }
            break;
        case 0xA000:
            cpu->i = opcode & 0xFFF;
            break;
        case 0xB000:
            cpu->pc = (opcode & 0xFFF) + cpu->v[0];
            break;
        case 0xC000:
            cpu->v[cpu->x] = (rand() % 256) & (opcode & 0xFF);
            break;
        case 0xD000:
            cpu->v[0xF] = 0;
            int width = 8;
            int height = opcode & 0xF;
            for (int y = 0; y < height; y++) {
                int sprite = cpu->memory[cpu->i + y];
                for (int x = 0; x < width; x++) {
                    if ((sprite & 0x80) > 0) {
                        cpu->v[0xF] = cpu->v[0xF] | toggle_pixel(cpu->display, cpu->v[cpu->x] + x, cpu->v[cpu->y] + y);
                    }
                    sprite = sprite << 1;
                }
            }
            break;
        case 0xE000:
            switch (opcode & 0xFF) {
                case 0x9E:
                    if (key_pressed(cpu->keyboard, cpu->v[cpu->x])) {
                        cpu->pc += 2;
                    }
                    break;
                case 0xA1:
                    if (!key_pressed(cpu->keyboard, cpu->v[cpu->x])) {
                        cpu->pc += 2;
                    }
                    break;
            }
            break;
        case 0xF000:
            switch (opcode & 0xFF) {
                case 0x07:
                    cpu->v[cpu->x] = cpu->delay_timer;
                    break;
                case 0x0A:
                    cpu->paused = 1;
                    cpu->keyboard->on_next_press = 1;
                    break;
                case 0x15:
                    cpu->delay_timer = cpu->v[cpu->x];
                    break;
                case 0x18:
                    cpu->sound_timer = cpu->v[cpu->x];
                    break;
                case 0x1E:
                    cpu->i += cpu->v[cpu->x];
                    break;
                case 0x29:
                    cpu->i = cpu->v[cpu->x] * 5;
                    break;
                case 0x33:
                    cpu->memory[cpu->i] = floor(cpu->v[cpu->x] / 100);
                    cpu->memory[cpu->i + 1] = floor((cpu->v[cpu->x] % 100)  / 10);
                    cpu->memory[cpu->i + 2] = floor(cpu->v[cpu->x] % 10);
                    break;
                case 0x55:
                    for (int i = 0; i <= cpu->x; i++) {
                        cpu->memory[cpu->i + i] = cpu->v[i];
                    }
                    break;
                case 0x65:
                    for (int i = 0; i <= cpu->x; i++) {
                        cpu->v[i] = cpu->memory[cpu->i + i];
                    }
                    break;
            }
            break;
        default:
        fprintf(stderr, "Invalid opcode: %X\n", opcode);
    }
}

void cycle(struct CPU* cpu) {
    for (int i = 0; i < cpu->speed; i++) {
        if (!cpu->paused) {
            int opcode = (cpu->memory[cpu->pc] << 8) | cpu->memory[cpu->pc + 1];
            execute(cpu, opcode);
        }
    }
    if (!cpu->paused) {
        update_timers(cpu);
    }
    play_sound(cpu);
    render(cpu->display);
}

void clean(struct CPU* cpu) {
    destroy_audio(cpu->audio);
    destroy_graphics(cpu->display);
    free(cpu->memory);
    free(cpu->v);
    free(cpu->stack);
}
