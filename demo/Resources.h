#pragma once
#include <memory>
#include <unordered_map>
#include "core/Image.h"
#include "core/Sound.h"
#include "utils/Font.h"
#include "core/Animation.h"

#include "utils/Configuration.h"
#include "effects/AnimationEffect.h"

typedef std::unordered_map<std::string, Image*> ImagesMap;
typedef std::unordered_map<std::string, Font*> FontsMap;
typedef std::unordered_map<std::string, Sound*> SoundsMap;
typedef std::unordered_map<std::string, Sound*> MusicMap;
typedef std::unordered_map<std::string, Animation*> AnimationsMap;
typedef std::unordered_map<std::string, std::shared_ptr<libled::AnimatedImage>> GifsMap;

class Graphics;

class Resources final
{
private:


	// Resources
	ImagesMap images;
	FontsMap fonts;
	SoundsMap sounds;
	MusicMap music;
	AnimationsMap animations;
	GifsMap gifs;

	// Common resource shortcuts
	const Font* boldbits;
	const Font* boldbitslarge;
	const Font* smallest;

	// Methods
	void CopyFilenamesByExtension(vector<String>& input, vector<String>& output, const char* extension);

public:

	Resources(const Configuration& config, Graphics& graphics);
	~Resources();

	// Access
	const Image& GetImage(const char* filename) const;
	const Font& GetFont(const char* filename) const;
	Sound& GetSound(const char* filename) const;
	Sound& GetMusic(const char* filename) const;
	const Animation& GetAni(const char* filename) const;
	std::shared_ptr<libled::AnimatedImage> GetGif(const char* filename) const;

	// Common resources
	const Font& BoldBits() { return *boldbits; }
	const Font& BoldBitsLarge() { return *boldbitslarge; }
	const Font& Smallest() { return *smallest; }
};

