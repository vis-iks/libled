#pragma once
#include "platform/IGraphicsHAL.h"
#include "utils/Configuration.h"
#include <iostream>

class DummyGraphics : public IGraphicsHAL
{
public:
	DummyGraphics(const Configuration& config) {}
	virtual ~DummyGraphics() {}

	virtual void Present(Canvas& sourcecanvas) override 
    {
        // Do nothing or maybe sleep to simulate vsync?
        // usleep(16000); 
    }
	virtual void SetBrightness(int b) override {}
	virtual int GetBrightness() const override { return 100; }
    virtual int GetKeyPress() override { return 0; }
};
