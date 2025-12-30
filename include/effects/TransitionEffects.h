#pragma once
#include "IEffect.h"
#include "core/Graphics.h"
#include "core/Canvas.h"
#include <iostream>
#include <vector>

namespace libled {

class CrossFadeEffect : public IEffect
{
private:
    std::shared_ptr<IEffect> sourceA;
    std::shared_ptr<IEffect> sourceB;
    int durationMs;
    int elapsed;
    Canvas canvasA;
    Canvas canvasB;
    bool finished;

public:
    CrossFadeEffect(std::shared_ptr<IEffect> a, std::shared_ptr<IEffect> b, int duration) 
        : sourceA(a), sourceB(b), durationMs(duration), elapsed(0), finished(false)
    {
    }

    virtual void Render(Canvas& canvas, uint32_t timeMs) override
    {
        // Calculate progress (Ping-pong loop 0..1..0)
        int cycleTime = durationMs * 2;
        int tMs = timeMs % cycleTime;
        float progress;
        if (tMs < durationMs) {
            progress = (float)tMs / (float)durationMs;
        } else {
            progress = 1.0f - ((float)(tMs - durationMs) / (float)durationMs);
        }

        // Render A
        canvasA.Clear(Color(0,0,0,0));
        if (sourceA) sourceA->Render(canvasA, timeMs);
        
        // Render B
        canvasB.Clear(Color(0,0,0,0));
        if (sourceB) sourceB->Render(canvasB, timeMs);
        
        // Manual blend: interpolate between A and B based on progress
        // progress = 0 means show A, progress = 1 means show B
        for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
            for (int x = 0; x < DISPLAY_WIDTH; ++x) {
                Color a = canvasA.GetPixel(x, y);
                Color b = canvasB.GetPixel(x, y);
                
                // Linear interpolation
                byte r = (byte)((1.0f - progress) * a.r + progress * b.r);
                byte g = (byte)((1.0f - progress) * a.g + progress * b.g);
                byte bal = (byte)((1.0f - progress) * a.b + progress * b.b);
                
                canvas.SetPixel(x, y, Color(r, g, bal));
            }
        }
    }
};

class DissolveEffect : public IEffect
{
private:
    std::shared_ptr<IEffect> sourceA;
    std::shared_ptr<IEffect> sourceB;
    int durationMs;
    Canvas canvasA;
    Canvas canvasB;
    
public:
    DissolveEffect(std::shared_ptr<IEffect> a, std::shared_ptr<IEffect> b, int duration) 
        : sourceA(a), sourceB(b), durationMs(duration) {}
        
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
};

class MeltTransitionEffect : public IEffect
{
private:
    std::shared_ptr<IEffect> source;
    std::shared_ptr<IEffect> dest;
    uint32_t duration;
    uint32_t startTime;
    bool started;
    
    Canvas sourceCanvas;
    Canvas destCanvas;
    std::vector<int> columnOffsets; // Random time offset for each column (ms)
    bool initialized;

public:
    MeltTransitionEffect(std::shared_ptr<IEffect> src, std::shared_ptr<IEffect> dst, uint32_t duration);
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
    virtual void Reset() override;
};

enum class TransitionDirection { Left, Right, Up, Down };

class SlideTransitionEffect : public IEffect
{
    std::shared_ptr<IEffect> sourceA;
    std::shared_ptr<IEffect> sourceB;
    int durationMs;
    TransitionDirection dir;
    Canvas canvasA;
    Canvas canvasB;
public:
    SlideTransitionEffect(std::shared_ptr<IEffect> a, std::shared_ptr<IEffect> b, int duration, TransitionDirection d = TransitionDirection::Left)
        : sourceA(a), sourceB(b), durationMs(duration), dir(d) {}
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
};

class WipeTransitionEffect : public IEffect
{
    std::shared_ptr<IEffect> sourceA;
    std::shared_ptr<IEffect> sourceB;
    int durationMs;
    TransitionDirection dir;
    Canvas canvasA;
    Canvas canvasB;
public:
    WipeTransitionEffect(std::shared_ptr<IEffect> a, std::shared_ptr<IEffect> b, int duration, TransitionDirection d = TransitionDirection::Left)
        : sourceA(a), sourceB(b), durationMs(duration), dir(d) {}
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
};

class ZoomTransitionEffect : public IEffect
{
    std::shared_ptr<IEffect> sourceA;
    std::shared_ptr<IEffect> sourceB;
    int durationMs;
    bool zoomIn; // True = B zooms in, False = A zooms out
    Canvas canvasA;
    Canvas canvasB;
public:
    ZoomTransitionEffect(std::shared_ptr<IEffect> a, std::shared_ptr<IEffect> b, int duration, bool pixelated = true)
        : sourceA(a), sourceB(b), durationMs(duration), zoomIn(true) {}
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
};

}
