#include "AudioSystem.hpp"

#include "Constants.hpp"
#include "Exceptions.hpp"
#include <iostream>

#include <SDL.h>

namespace tp
{

//uint8_t* audioChunk;
//uint32_t audioLength;
//uint8_t* audioPosition;
//int pitchIncrease = 0;
//
//void audioCallback(void* data, uint8_t* stream, int length)
//{
//    if (0 == audioLength)
//        return;
//
//    length = (length > audioLength ? audioLength : length);
//    std::cout << length << std::endl;
//
//    SDL_memcpy(stream, audioPosition, length);
//    int16_t* buf = reinterpret_cast<int16_t*>(stream);
//    for (int index = 0; index < length / 2; ++index)
//        *buf += pitchIncrease;
//
//    // SDL_MixAudio(stream, audioPosition, length, SDL_MIX_MAXVOLUME);
//
//    audioPosition += length;
//    audioLength -= length;
//}

AudioBuffer loadAudio(const std::string& fileName)
{
    AudioBuffer result{};

    SDL_RWops* file = SDL_RWFromFile(fileName.c_str(), "rb");
    if (nullptr == file)
    {
        throw Exception("Cannot open file: " + fileName);
    }

    SDL_AudioSpec audioSpecFromFile{};

    SDL_AudioSpec* audioSpec =
        SDL_LoadWAV_RW(file, 0, &audioSpecFromFile, &result.data, &result.length);

    if (nullptr == audioSpec)
        throw Exception("Could not load audio samples from file: " + fileName +
                        ", error: " + std::string{ SDL_GetError() });

    return result;
}

AudioSystem::AudioSystem()
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
        throw Exception("Could not initialize SDL Audio" + std::string(SDL_GetError()));

    tapBuffer_   = loadAudio(Resources::TAP_AUDIO_FILE);
    deathBuffer_ = loadAudio(Resources::DEATH_AUDIO_FILE);

    // clang-format off
    SDL_AudioSpec desiredAudioSpec
    {
        // .freq     = 48000,
        .format   = AUDIO_S16LSB,
        // .channels = 1, // mono
        // .silence  = 0,
        // .samples  = 4096, // must be power of 2
        // .padding  = 0,
        // .size     = tapBuffer_.length,
        //.callback = audioCallback,
        .callback = nullptr,
        .userdata = nullptr
    };
    // clang-format on

    //audioPosition = tapBuffer_.data;
    //audioLength   = tapBuffer_.length;

    audioDeviceId_ = SDL_OpenAudioDevice(nullptr, 0, &desiredAudioSpec, nullptr, 0);

    if (0 == audioDeviceId_ )
        throw Exception("Could not open audio device");
}

void AudioSystem::playAudio(const AudioBuffer& audioBuffer)
{
    if (-1 == SDL_QueueAudio(audioDeviceId_, audioBuffer.data, audioBuffer.length))
        throw Exception("Could not send audio buffer to the device");

    SDL_PauseAudioDevice(audioDeviceId_, SDL_FALSE);
}

void AudioSystem::playClickSound()
{
#ifndef TAPPY_NO_AUDIO
    playAudio(tapBuffer_);
#endif
    //pitchIncrease++;
    //audioPosition = tapBuffer_.data;
    //audioLength = tapBuffer_.length;
    //SDL_PauseAudioDevice(audioDeviceId_, SDL_FALSE);
}

void AudioSystem::playHitGroundSound()
{
#ifndef TAPPY_NO_AUDIO
    playAudio(deathBuffer_);
#endif
}

AudioSystem::~AudioSystem()
{
    SDL_CloseAudioDevice(audioDeviceId_);
    SDL_FreeWAV(tapBuffer_.data);
    SDL_FreeWAV(deathBuffer_.data);
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

} // namespace tp