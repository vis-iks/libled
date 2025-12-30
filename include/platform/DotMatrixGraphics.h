#pragma once
#include "platform/IGraphicsHAL.h"

#ifdef RPI
#include "utils/Configuration.h"
#include <led-matrix.h>

class DotMatrixGraphics final : public virtual IGraphicsHAL
{
private:
	// Matrix display
	rgb_matrix::RGBMatrix* display;

	// The buffer to which we convert our rasterized image.
	rgb_matrix::FrameCanvas* displaycanvas;

	// Brightness (0-100)
	int brightness;

public:
	DotMatrixGraphics(const Configuration& cfg);
	virtual ~DotMatrixGraphics();

	// Methods
	virtual void Present(Canvas& sourcecanvas) override final;
	virtual void SetBrightness(int b) override final;
	virtual int GetBrightness() const override final;
    virtual int GetKeyPress() override final { return 0; }
};
#endif
