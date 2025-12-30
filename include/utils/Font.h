#pragma once
#include <unordered_map>
#include "core/Image.h"
#include "utils/FontChar.h"



typedef std::unordered_map<uint, FontChar> FontCharMap;
typedef FontCharMap::value_type FontCharPair;
typedef FontCharMap::const_iterator FontCharCIt;

typedef std::pair<uint, uint> KerningChars;
struct KerningCharsHasher
{
	std::size_t operator()(const KerningChars& k) const
	{
		return ((std::hash<uint>()(k.first) ^ (std::hash<uint>()(k.second) << 1)) >> 1);
	}
};

typedef std::unordered_map<KerningChars, int, KerningCharsHasher> KerningMap;
typedef KerningMap::value_type KerningPair;
typedef KerningMap::const_iterator KerningCIt;

class Font
{
private:

	// Members
	const Image* image;
	int lineheight;
	int linebase;
	FontChar unknownchar;
	FontCharMap charmap;
	KerningMap kernmap;

public:

	// Constructor / destructor
	// Constructor: Reads font metadata from file. 
    // Requires the texture image to be pre-loaded and passed in.
	Font(const String& filename, const Image* image);
    Font(const Image* img, bool takeOwnership); // Internal constructor
	virtual ~Font();
    
    // Helper to find the texture filename from the font file without loading the font
    static String GetTextureFilename(const String& filename);

    static Font* CreateDefault();
    static Font* CreateBold();
    
    bool ownsImage = false;

	// Access
	inline const Image& GetImage() const { return *image; }
	inline int LineHeight() const { return lineheight; }
	inline int LineBase() const { return linebase; }
	inline const FontChar& GetCharacter(uint charid) const
	{
		FontCharCIt it = charmap.find(charid);
		return (it == charmap.end()) ? unknownchar : it->second;
	}
	inline int GetKerningOffset(uint previouscharid, uint currentcharid) const
	{
		KerningCIt it = kernmap.find(KerningChars(previouscharid, currentcharid));
		return (it == kernmap.end()) ? 0 : it->second;
	}
};
