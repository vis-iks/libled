#include "core/Sound.h"
#include "utils/Tools.h"
#include <iostream>

// We need access to the FMOD system. 
// Since we are decoupling, we might need a static accessor or pass it in.
// For now, assuming Audio handles system and we might need to get it.
// Issue: Audio is currently in Pandemic.
// Temporary fix: Assume a global or static getter for FMOD System if possible, 
// OR simpler: Move Audio to libled.
// I will proceed assuming Audio will be moved to libled/core/Audio.h and exposes a static GetSystem or similar?
// Or better: Sound takes system in constructor. 
// But Resources creates Sound. Resources needs System.
// Resources needs Audio.

// Let's implement Sound assuming it gets the system from somewhere. 
// For this step, I'll include Audio.h (which I will move to libled).
#include "core/Audio.h"

Sound::Sound(const String& filename, bool stream, float vol) :
    sound(nullptr),
    channel(nullptr),
    volume(vol),
    stream(stream)
{
    FMOD::System* sys = Audio::GetSystemInstance(); // Need to add this static method to Audio
    if (sys)
    {
        FMOD_MODE mode = FMOD_DEFAULT;
        if (stream) mode |= FMOD_CREATESTREAM;
        
        FMOD_RESULT result = sys->createSound(filename.c_str(), mode, nullptr, &sound);
        if (result != FMOD_OK)
        {
            std::cout << "Failed to load sound: " << filename << " Error: " << result << std::endl;
        }
    }
}

Sound::~Sound()
{
    if (sound)
    {
        sound->release();
        sound = nullptr;
    }
}

void Sound::Play(bool loop)
{
    FMOD::System* sys = Audio::GetSystemInstance();
    if (sys && sound)
    {
        sys->playSound(sound, nullptr, true, &channel);
        if (channel)
        {
            channel->setVolume(volume);
            if (loop)
            {
                channel->setMode(FMOD_LOOP_NORMAL);
                channel->setLoopCount(-1);
            }
            else
            {
                channel->setMode(FMOD_LOOP_OFF);
            }
            channel->setPaused(false);
        }
    }
}

void Sound::Stop()
{
    if (channel)
    {
        channel->stop();
        channel = nullptr;
    }
}

bool Sound::IsPlaying() const
{
    if (!channel) return false;
    bool playing = false;
    channel->isPlaying(&playing);
    return playing;
}

void Sound::SetVolume(float v)
{
    volume = v;
    if (channel)
    {
        channel->setVolume(volume);
    }
}
