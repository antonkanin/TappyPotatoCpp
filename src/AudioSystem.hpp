#pragma once

namespace tp
{

struct AudioBuffer
{
    unsigned char* data{};
    unsigned int   length{};
};

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
    void playHitGroundSound();

private:
    void playAudio(const AudioBuffer& audioBuffer);

    AudioBuffer tapBuffer_;
    AudioBuffer deathBuffer_;

    unsigned int audioDeviceId_{};
};

} // namespace tp