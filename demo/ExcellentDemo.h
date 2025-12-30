#pragma once
#include <effects/IEffect.h>
#include <utils/Font.h>
#include <utils/Text.h>
#include <core/Canvas.h>
#include <utils/Tools.h>
#include <core/Image.h>
#include "Shaders.h"
#include <memory>

using namespace libled;

/**
 * ExcellentDemo - Recreated EXCELLENT animation effect
 * Displays "EXCELLENT" text with burning effect that progresses from top to bottom
 */
class ExcellentDemo : public IEffect
{
private:
    const Font& font;
    Text text;
    Canvas tempCanvas;
    TweenFloat burnProgress;
    TimePoint startTime;
    TimePoint lastStepTime;
    const Image& burnMap;
    const Image& burnTex;
    const Image& burnColors;
    
    static constexpr float BURN_LENGTH = 0.1f;
    
    void Start();

public:
    ExcellentDemo(const Font& f, const Image& bm, const Image& bt, const Image& bc);
    virtual ~ExcellentDemo() = default;

    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
};
