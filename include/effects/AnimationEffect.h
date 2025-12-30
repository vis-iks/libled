#pragma once
#include "core/Image.h"
#include "IEffect.h"
#include <vector>

namespace libled {

// Extension of Image that holds multiple frames
class AnimatedImage
{
private:
    std::vector<Image*> frames;
    std::vector<int> delays; // Delay in ms per frame
    int loopCount;
    
public:
    AnimatedImage();
    ~AnimatedImage();
    
    // Load GIF using stb_image
    bool LoadGif(const String& filename);
    
    // Set a color to be transparent (alpha = 0)
    void SetTransparentColor(Color color);
    
    bool HasTransparency() const { return hasTransparency; }
    
    int GetFrameCount() const { return frames.size(); }
    const Image* GetFrame(int index) const;
    int GetDelay(int index) const;

private:
    bool hasTransparency = false;
};

// Effect to play an AnimatedImage
class AnimationEffect : public IEffect
{
private:
    std::shared_ptr<AnimatedImage> anim;
    int currentFrame;
    uint32_t lastUpdate;
    uint32_t accumulator;
    bool playing;
    
public:
    AnimationEffect(std::shared_ptr<AnimatedImage> image);
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
    
    void Play() { playing = true; }
    void Stop() { playing = false; currentFrame = 0; }
};

}
