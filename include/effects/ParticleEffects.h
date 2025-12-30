#pragma once
#include "IEffect.h"
#include <vector>
#include "core/Particle.h"

namespace libled {

// Particle System
struct SimpleParticle {
    float x, y;
    float vx, vy;
    float life; // 0.0 to 1.0 (or seconds)
    Color color;
};

class ParticleSystemEffect : public IEffect
{
private:
    std::vector<SimpleParticle> particles;
    uint32_t lastUpdate;
    
    // Emitter settings
    float emissionRate; // particles per second
    float emissionAccumulator;
    
    // Config
    Color baseColor;
    bool gravity;
    
public:
    ParticleSystemEffect();
    
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
    
    void SetGravity(bool g) { gravity = g; }
    void SetBaseColor(Color c) { baseColor = c; }
};

class ExplosionEffect : public IEffect
{
    struct Particle { float x, y, vx, vy, life, maxLife; Color color; };
    std::vector<Particle> particles;
    Point center;
    Color color;
    int count;
    bool triggered;
    uint32_t lastUpdate;
public:
    ExplosionEffect(Point c, Color col, int numParticles = 100)
        : center(c), color(col), count(numParticles), triggered(false), lastUpdate(0) {}
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
    void Trigger();
};

class FireworksEffect : public IEffect
{
    struct Rocket { float x, y, vy; Color color; bool exploded; };
    std::vector<Rocket> rockets;
    std::vector<std::shared_ptr<ExplosionEffect>> explosions;
    uint32_t lastUpdate;
    uint32_t nextRocketTime;
public:
    FireworksEffect() : lastUpdate(0), nextRocketTime(0) {}
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
};

class CometEffect : public IEffect
{
    struct Trail { float x, y, life; };
    float x, y, vx, vy;
    std::vector<Trail> trails;
    uint32_t lastUpdate;
public:
    CometEffect();
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
};

class FallingPixelsEffect : public IEffect
{
    struct Pixel { float x, y, vy; Color c; bool active; };
    std::vector<Pixel> pixels;
    bool initialized;
    uint32_t lastUpdate;
    const IImage& sourceImage;
    float speed;
    float rate; // Threshold for random activation (0-1000)
public:
    FallingPixelsEffect(const IImage& img) : sourceImage(img), initialized(false), lastUpdate(0), speed(1.0f), rate(5.0f) {}
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
    void SetSpeed(float s) { speed = s; }
    void SetRate(float r) { rate = r; }
};

}
