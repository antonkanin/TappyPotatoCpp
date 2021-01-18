#include "AudioSystem.hpp"

#include "Constants.hpp"
#include "Exceptions.hpp"

#include <SDL.h>

namespace tp
{
AudioSystem::AudioSystem()
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
        throw Exception("Could not initialize SDL Audio" + std::string(SDL_GetError()));

    SDL_RWops* file = SDL_RWFromFile(Resources::TAP_AUDIO_FILE, "rb");
    if (nullptr == file)
    {
        throw Exception("Cannot open file: " + std::string(Resources::TAP_AUDIO_FILE));
    }

    SDL_AudioSpec audioSpecFromFile{};

    SDL_AudioSpec* audioSpec =
        SDL_LoadWAV_RW(file, 0, &audioSpecFromFile, &audioBuffer_, &audioBufferLength_);

    if (nullptr == audioSpec)
        throw Exception(
            "Could not load audio samples from file: " + std::string{ Resources::TAP_AUDIO_FILE } +
            ", error: " + std::string{ SDL_GetError() });

    audioDeviceId_ = SDL_OpenAudioDevice(nullptr, 0, &audioSpecFromFile, nullptr, 0);
}

void AudioSystem::playClickSound()
{
    if (-1 == SDL_QueueAudio(audioDeviceId_, audioBuffer_, audioBufferLength_))
        throw Exception("Could not send audio buffer to the device");

    SDL_PauseAudioDevice(audioDeviceId_, SDL_FALSE);
}

AudioSystem::~AudioSystem()
{
    SDL_CloseAudioDevice(audioDeviceId_);
    SDL_FreeWAV(audioBuffer_);
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

} // namespace tp