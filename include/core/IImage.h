#pragma once
#include "core/Color.h"
#include "core/Samplers.h"
#include "core/Size.h"

class IImage
{
	// Make this an interface, do not allow instantiation
public:
	virtual ~IImage() = default;
protected:
	IImage() { }
	IImage(const IImage&) { }
	IImage& operator = (const IImage&) { return *this; }
public:

	// Methods
	virtual bool HasColors() const = 0;
	virtual int Width() const = 0;
	virtual int Height() const = 0;
	virtual Size GetSize() const = 0;
	virtual const byte* ByteData() const = 0;
	virtual const Color* ColorData() const = 0;
	virtual MonoSampler GetMonoSampler() const = 0;
	virtual ColorSampler GetColorSampler() const = 0;
};
