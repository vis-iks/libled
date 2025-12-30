#pragma once
#include "core/Defines.h"
#include "core/Point.h"

struct FrameData
{
public:

	// Position in the image
	Point imagepos;

	// Frame duration in milliseconds
	uint duration;

	// Constructor
	FrameData() :
		duration(0)
	{ }

	// Constructor
	FrameData(Point imagepos, uint duration) :
		imagepos(imagepos),
		duration(duration)
	{ }
};
