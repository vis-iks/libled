#include "effects/CoreEffects.h"
#include <cmath>

namespace libled {

void SolidColorEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    // Canvas::Clear fills the whole buffer
    canvas.Clear(color);
}

void GradientEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    int width = canvas.Width();
    int height = canvas.Height();

    if (type == GradientType::LinearHorizontal) {
        for (int x = 0; x < width; ++x) {
            float t = (float)x / (width - 1);
            Color c = Color::Gradient(startColor, endColor, t);
            for (int y = 0; y < height; ++y) {
                 canvas.SetPixel(x, y, c);
            }
        }
    } else if (type == GradientType::LinearVertical) {
        for (int y = 0; y < height; ++y) {
            float t = (float)y / (height - 1);
            Color c = Color::Gradient(startColor, endColor, t);
            for (int x = 0; x < width; ++x) {
                 canvas.SetPixel(x, y, c);
            }
        }
    } else if (type == GradientType::Radial) {
        float centerX = width / 2.0f;
        float centerY = height / 2.0f;
        float maxDist = std::sqrt(centerX * centerX + centerY * centerY);
        
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float dist = std::sqrt(std::pow(x - centerX, 2) + std::pow(y - centerY, 2));
                float t = std::min(dist / maxDist, 1.0f);
                Color c = Color::Gradient(startColor, endColor, t);
                canvas.SetPixel(x, y, c);
            }
        }
    }
}



void PlasmaEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    float t = timeMs / 1000.0f;
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            float v1 = sin(x / 10.0f + t);
            float v2 = sin((y / 10.0f + t) / 2.0f);
            float v3 = sin((x / 10.0f + y / 10.0f + t) / 2.0f);
            float v = (sin(v1 + v2 + v3) + 1.0f) / 2.0f;
            
            // Map to color (e.g. Purple to Cyan)
            byte r = (byte)(v * 255);
            byte g = (byte)((1.0f - v) * 255);
            byte b = 255;
            canvas.SetPixel(x, y, Color(r, g, b));
        }
    }
}

CompositeEffect::CompositeEffect(std::shared_ptr<IEffect> bottom, std::shared_ptr<IEffect> top)
{
    AddEffect(bottom);
    AddEffect(top);
}

void CompositeEffect::AddEffect(std::shared_ptr<IEffect> effect)
{
    if (effect) {
        effects.push_back(effect);
    }
}

void CompositeEffect::ClearEffects()
{
    effects.clear();
}

void CompositeEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    for (auto& effect : effects) {
        effect->Render(canvas, timeMs);
    }
}

}
