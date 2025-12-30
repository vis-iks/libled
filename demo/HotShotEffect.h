#pragma once
#include <effects/IEffect.h>
#include <utils/Font.h>
#include <utils/Text.h>
#include <core/Canvas.h>
#include <utils/Tools.h>
#include <vector>
#include <glm/vec2.hpp>

using namespace libled;

/**
 * HotShotEffect - Animated text effect with fire shader and particles
 */
class HotShotEffect : public IEffect
{
private:
    const Font& font;
    Text text1;
    Text text2;
    Canvas maskCanvas;
    
    uint32_t startTime;
    bool started;
    
    struct SimpleParticle {
        glm::vec2 pos;
        glm::vec2 vel;
        Color color;
        float life;
    };
    std::vector<SimpleParticle> particles;

    TweenXY text1Tween;
    TweenXY text2Tween;

public:
    HotShotEffect(const Font& f);
    virtual ~HotShotEffect();

    virtual void Reset() override;
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
};
