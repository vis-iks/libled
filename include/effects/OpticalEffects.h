#pragma once
#include "IEffect.h"

namespace libled {

// Applies post-processing to another effect
class PostProcessEffect : public IEffect
{
protected:
    std::shared_ptr<IEffect> source;
public:
    PostProcessEffect(std::shared_ptr<IEffect> src) : source(src) {}
    virtual void Render(Canvas& canvas, uint32_t timeMs) override = 0;
};

// Brightness/Fade effect
class FadeEffect : public PostProcessEffect
{
private:
    float brightness; // 0.0 to 1.0
public:
    FadeEffect(std::shared_ptr<IEffect> src, float b = 1.0f) : PostProcessEffect(src), brightness(b) {}
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
    void SetBrightness(float b) { brightness = b; }
};

// Simple Blur (Box Blur 3x3)
class BlurEffect : public PostProcessEffect
{
public:
    BlurEffect(std::shared_ptr<IEffect> src) : PostProcessEffect(src) {}
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
};

class FlashEffect : public IEffect
{
private:
    std::shared_ptr<IEffect> source;
    int period;
public:
    FlashEffect(std::shared_ptr<IEffect> src, int p = 1000) : source(src), period(p) {}
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
};

}
