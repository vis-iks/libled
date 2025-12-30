#pragma once
#include "IEffect.h"
#include "core/Color.h"
#include <functional>

namespace libled {

/**
 * PixelShaderEffect - A wrapper effect that applies a pixel shader function to every pixel.
 * The shader function receives normalized UV coordinates (0-1) and time in seconds.
 */
class PixelShaderEffect : public IEffect
{
public:
    using ShaderFunction = std::function<Color(float u, float v, float time)>;

private:
    ShaderFunction shader;
    uint32_t startTime;
    bool started;

public:
    /**
     * Construct a PixelShaderEffect with a custom shader function.
     * @param fn The shader function: Color(float u, float v, float time)
     */
    PixelShaderEffect(ShaderFunction fn);

    virtual void Reset() override;
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
};

}
