#pragma once

namespace tp
{
class AudioSystem final
{
public:
    AudioSystem();
    ~AudioSystem();

    AudioSystem(const AudioSystem&) = delete;
    AudioSystem& operator=(const AudioSystem&) = delete;

    AudioSystem(AudioSystem&&) = delete;
    AudioSystem& operator=(AudioSystem&&) = delete;

    void playClickSound();

private:
    unsigned char* audioBuffer_{};
    unsigned int   audioBufferLength_{};
    unsigned int   audioDeviceId_{};
};

} // namespace tp