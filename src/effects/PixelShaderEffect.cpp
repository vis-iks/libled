#include "effects/PixelShaderEffect.h"
#include "core/Defines.h"

namespace libled {

PixelShaderEffect::PixelShaderEffect(ShaderFunction fn)
    : shader(fn), startTime(0), started(false)
{
}

void PixelShaderEffect::Reset()
{
    started = false;
    startTime = 0;
}

void PixelShaderEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    if (!started) {
        startTime = timeMs;
        started = true;
    }

    float time = (timeMs - startTime) / 1000.0f;

    for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
        for (int x = 0; x < DISPLAY_WIDTH; ++x) {
            float u = x / (float)DISPLAY_WIDTH;
            float v = y / (float)DISPLAY_HEIGHT;
            Color c = shader(u, v, time);
            canvas.SetPixel(x, y, c);
        }
    }
}

}
