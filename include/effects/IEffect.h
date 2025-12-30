#pragma once
#include "core/Canvas.h"
#include <vector>

namespace libled {

class IEffect
{
public:
	virtual ~IEffect() = default;

	// Render the effect to the canvas.
	// timeMs: Current time in milliseconds.
	virtual void Render(Canvas& canvas, uint32_t timeMs) = 0;

	// Optional: Reset the effect state
	virtual void Reset() {}
    
    // Optional: Set duration/check if finished
    virtual bool IsFinished() const { return false; }
};

}
