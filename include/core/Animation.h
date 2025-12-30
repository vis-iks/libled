#pragma once
#include "core/Image.h"
#include "core/FrameData.h"



class Animation
{
private:

	// Image which contains all frames
	const Image* image;

	// Size of a single frame in the image
	Size framesize;

	// Frame information
	vector<FrameData> frames;

	// Total duration in milliseconds
	uint duration;

public:

	// Constructor
	// Constructor
	Animation(const String& filename, const Image* image);
    // Helper to find the texture filename from the animation file without loading it
    static String GetTextureFilename(const String& filename);
	Animation(const Image& image);
	~Animation();

	// Access
	inline const Image& GetImage() const { return *image; }
	inline Size GetFrameSize() const { return framesize; }
	inline const vector<FrameData>& GetFrames() const { return frames; }
	inline uint GetDuration() const { return duration; }
};

