#pragma once
#include "utils/String.h"
#include <fmod.hpp>

class Sound
{
private:
    FMOD::Sound* sound;
    FMOD::Channel* channel;
    float volume;
    bool stream;

public:
    Sound(const String& filename, bool stream, float volume);
    ~Sound();

    void Play(bool loop = false);
    void Stop();
    bool IsPlaying() const;
    void SetVolume(float v);
};
