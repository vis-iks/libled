#pragma once
#include "IEffect.h"

namespace libled {

class ScrollEffect : public IEffect
{
private:
    std::shared_ptr<IEffect> source;
    int speedX;
    int speedY;
    float posX;
    float posY;
    uint32_t lastUpdate;

public:
    ScrollEffect(std::shared_ptr<IEffect> src, int sx, int sy) 
        : source(src), speedX(sx), speedY(sy), posX(0), posY(0), lastUpdate(0) {}
        
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
    
    void SetSpeed(int sx, int sy) { speedX = sx; speedY = sy; }
};

class ShakeEffect : public IEffect
{
private:
    std::shared_ptr<IEffect> source;
    int intensity;
public:
    ShakeEffect(std::shared_ptr<IEffect> src, int i = 5) : source(src), intensity(i) {}
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
};

class JitterEffect : public IEffect
{
private:
    std::shared_ptr<IEffect> source;
public:
    JitterEffect(std::shared_ptr<IEffect> src) : source(src) {}
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
};

}
