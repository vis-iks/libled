#pragma once
#include <effects/IEffect.h>
#include <utils/Font.h>
#include <utils/Text.h>
#include <core/Canvas.h>
#include <utils/Tools.h>
#include <core/Image.h>
#include <effects/ParticleEffects.h>
#include <memory>

using namespace libled;

/**
 * ImpressiveDemo - Recreated IMPRESSIVE animation effect
 * Displays "IMPRESSIVE" text character by character with scrolling, 
 * wave motion, textured rendering, and particle effects
 */
class ImpressiveDemo : public IEffect
{
private:
    const Font& font;
    Text characters[10];
    Point charPos[10];
    TweenInt offset;
    TimePoint startTime;
    TimePoint lastStepTime;
    TimePoint nextFlashTime;
    int flashCount;
    const Image& texture;
    std::shared_ptr<ParticleSystemEffect> particles;
    float flashProgress[10];  // Simple flash tracking
    
    // Constants (can't be static in local class)
    const int FLASH_INTERVAL_MS = 80;
    const int TOTAL_DURATION_MS = 2500;
    const int PARTICLES_PER_CHAR = 12;
    
    void Start();

public:
    ImpressiveDemo(const Font& f, const Image& tex);
    virtual ~ImpressiveDemo() = default;

    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
};
