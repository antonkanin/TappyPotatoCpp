#include "AudioSystem.hpp"

#include "Constants.hpp"
#include "Exceptions.hpp"
#include "Utils.hpp"

#include <SDL.h>
#include <cassert>

namespace tp
{
AudioSystem::AudioSystem()
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
        throw Exception("Could not initialize SDL Audio" + std::string(SDL_GetError()));

    auto audioFile = getData(Resources::TAP_AUDIO_FILE);
    assert(!audioFile.empty());

    SDL_AudioSpec audioSpecFromFile{};

    {
        const int deleteFile = 1;
        Uint8*    audioBuffer{};
        Uint32    audioLength{ 0 };
        SDL_AudioSpec* audioSpec = SDL_LoadWAV_RW(reinterpret_cast<SDL_RWops*>(audioFile.data()),
            deleteFile, &audioSpecFromFile, &audioBuffer, &audioLength);

        if (nullptr == audioSpec)
            throw Exception(
                "Could not load audio samples from file: " + std::string{ Resources::TAP_AUDIO_FILE } +
                ", error: " + std::string{ SDL_GetError() });
    }

    // SDL_OpenAudio()




}

AudioSystem::~AudioSystem()
{
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
void AudioSystem::playClickSound() {}
} // namespace tp