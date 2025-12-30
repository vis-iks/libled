#pragma once
#include "utils/Tools.h"
#include "core/Color.h"
#include "core/Size.h"
#include "core/IImage.h"

class Image final : public virtual IImage
{
private:

	// Fields
	bool hascolors;
	int width;
	int height;
	byte* data;

public:

	Image();
	Image(const String& filename);
    Image(int w, int h); // New
	virtual ~Image();

	// Loading
	void Load(const String& filename);
	void Unload();
    
    // Manual data setting (for GIFs etc)
    void SetPixel(int x, int y, Color c); // New
    void SetData(int w, int h, bool hasColor, byte* newData);

	// Properties
	virtual bool HasColors() const override final { return hascolors; }
	virtual int Width() const override final { return width; }
	virtual int Height() const override final { return height; }
	virtual Size GetSize() const override final { return Size(width, height); }

	virtual const byte* ByteData() const override final
	{
		#ifdef DEBUG
			REQUIRE(!hascolors);
		#endif
		return data;
	}

	virtual const Color* ColorData() const override final
	{
		#ifdef DEBUG
			REQUIRE(hascolors);
		#endif
		return reinterpret_cast<Color*>(data);
	}

	inline byte GetByte(int x, int y) const
	{
		#ifdef DEBUG
			REQUIRE(!hascolors);
		#endif
		return *(data + y * width + x);
	}

	inline Color GetColor(int x, int y) const
	{
		#ifdef DEBUG
			REQUIRE(hascolors);
		#endif
		return *(reinterpret_cast<Color*>(data) + y * width + x);
	}

	virtual MonoSampler GetMonoSampler() const override final
	{
		#ifdef DEBUG
			REQUIRE(!hascolors);
		#endif
		return MonoSampler(data, width);
	}

	virtual ColorSampler GetColorSampler() const override final
	{
		#ifdef DEBUG
			REQUIRE(hascolors);
		#endif
		return ColorSampler(reinterpret_cast<Color*>(data), width);
	}
};
