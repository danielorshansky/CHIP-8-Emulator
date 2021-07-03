#include <math.h>

#include <SDL2/SDL.h>

struct AudioData {
    int samples_per_second;
    int bytes_per_sample;
    int tone;
    int volume;
    int wave_period;
    uint32_t length;
    uint16_t* buffer;
    SDL_AudioDeviceID device;
};

void audio_callback(void* user_data, uint8_t* stream, int length) {
    struct AudioData* data = (struct AudioData*)user_data;
    static uint32_t count = 0;

    uint16_t* sample = (uint16_t*)stream;
    int samples = length / data->bytes_per_sample;
    for(int i = 0; i < samples; i++) {
        uint16_t tone_value = round((data->volume * sin(2 * M_PI * (float)count / (float)data->wave_period)));
        *sample++ = tone_value;
        *sample++ = tone_value;
        ++count;
    }
}

void init_audio(struct AudioData* data) {
    data->samples_per_second = 48000;
    data->bytes_per_sample = 2 * sizeof(int16_t);
    data->volume = 3000;
    data->tone = 440;
    data->wave_period = data->samples_per_second / data->tone;
    SDL_AudioSpec desired, obtained;
    desired.freq = data->samples_per_second;
    desired.format = AUDIO_S16;
    desired.channels = 2;
    desired.samples = 4096;
    desired.callback = &audio_callback;
    desired.userdata = data;
    data->device = SDL_OpenAudioDevice(0, 0, &desired, &obtained, 0);
}

void play(struct AudioData* data) {
    SDL_PauseAudioDevice(data->device, 0);
}

void pause(struct AudioData* data) {
    SDL_PauseAudioDevice(data->device, 1);
}

void destroy_audio(struct AudioData* data) {
    SDL_CloseAudioDevice(data->device);
}
