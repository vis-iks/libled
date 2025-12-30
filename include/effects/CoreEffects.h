#pragma once
#include "IEffect.h"
#include "core/Color.h"

namespace libled {

class SolidColorEffect : public IEffect
{
private:
    Color color;

public:
    SolidColorEffect(Color c) : color(c) {}
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
    void SetColor(Color c) { color = c; }
};

enum class GradientType {
    LinearHorizontal,
    LinearVertical,
    Radial
};

class GradientEffect : public IEffect
{
private:
    Color startColor;
    Color endColor;
    GradientType type;

public:
    GradientEffect(Color start, Color end, GradientType type = GradientType::LinearHorizontal) 
        : startColor(start), endColor(end), type(type) {}
        
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
    
    void SetColors(Color start, Color end) { startColor = start; endColor = end; }
};

class PlasmaEffect : public IEffect
{
public:
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
};

class CompositeEffect : public IEffect
{
private:
    std::vector<std::shared_ptr<IEffect>> effects;

public:
    CompositeEffect() = default;
    CompositeEffect(std::shared_ptr<IEffect> bottom, std::shared_ptr<IEffect> top);

    void AddEffect(std::shared_ptr<IEffect> effect);
    void ClearEffects();

    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
};

}
