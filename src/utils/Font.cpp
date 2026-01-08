#include "utils/Font.h"
#include "stb_truetype.h"
#include "utils/File.h"

// Helper to get texture filename
String Font::GetTextureFilename(const String &filename) {
  String fontinfo = File::ReadAsText(filename);
  vector<String> lines;
  fontinfo.Split(lines, '\n');

  // Expected format check could be here
  if (lines.size() < 3)
    return "";

  char imagefilename[256];
  int scancount = std::sscanf(lines[2], "page id=0 file=%255s", imagefilename);
  if (scancount != 1)
    return "";

  String cleanimagefilename = imagefilename;
  cleanimagefilename.Replace("\"", "");
  return cleanimagefilename;
}

// Constructor
Font::Font(const String &filename, const Image *img)
    : image(img), lineheight(0), linebase(0) {
  // Read the font file as text and split into lines
  String fontinfo = File::ReadAsText(filename);
  vector<String> lines;
  fontinfo.Split(lines, '\n');

  // Parse general font information
  int scancount = std::sscanf(lines[1], "common lineHeight=%i base=%i",
                              &lineheight, &linebase);
  ENSURE(scancount == 2);

  // Parse character information
  int numchars, ln = 3;
  scancount = std::sscanf(lines[ln++], "chars count=%i", &numchars);
  ENSURE(scancount == 1);
  for (int i = 0; i < numchars; i++) {
    FontChar fc;
    uint id;
    scancount = std::sscanf(lines[ln++],
                            "char id=%i x=%i y=%i width=%i height=%i "
                            "xoffset=%i yoffset=%i xadvance=%i",
                            &id, &fc.imgrect.x, &fc.imgrect.y,
                            &fc.imgrect.width, &fc.imgrect.height, &fc.offset.x,
                            &fc.offset.y, &fc.advance);
    ENSURE(scancount == 8);
    charmap.insert(FontCharPair(id, fc));
  }

  // Parse kerning information, if available
  if (lines.size() > static_cast<size_t>(ln)) {
    int numkernings;
    scancount = std::sscanf(lines[ln++], "kernings count=%i", &numkernings);
    if (scancount == 1) {
      for (int i = 0; i < numkernings; i++) {
        KerningChars chars;
        int amount;
        scancount =
            std::sscanf(lines[ln++], "kerning first=%i second=%i amount=%i",
                        &chars.first, &chars.second, &amount);
        ENSURE(scancount == 3);
        kernmap.insert(KerningPair(chars, amount));
      }
    }
  }
}

// New constructor
Font::Font(const Image *img, bool takeOwnership)
    : image(img), ownsImage(takeOwnership), lineheight(8), linebase(6) {}

// Destructor
Font::~Font() {
  if (ownsImage && image) {
    delete image;
  }
  image = nullptr;
}

// Simple 3x5 font data for alphanumeric (very basic fallback)
static const uint8_t font5x7[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, // Space
    0x00, 0x00, 0x5F, 0x00, 0x00, // !
    0x00, 0x07, 0x00, 0x07, 0x00, // "
    0x14, 0x7F, 0x14, 0x7F, 0x14, // #
    0x24, 0x2A, 0x7F, 0x2A, 0x12, // $
    0x23, 0x13, 0x08, 0x64, 0x62, // %
    0x36, 0x49, 0x55, 0x22, 0x50, // &
    0x00, 0x05, 0x03, 0x00, 0x00, // '
    0x00, 0x1C, 0x22, 0x41, 0x00, // (
    0x00, 0x41, 0x22, 0x1C, 0x00, // )
    0x14, 0x08, 0x3E, 0x08, 0x14, // *
    0x08, 0x08, 0x3E, 0x08, 0x08, // +
    0x00, 0x50, 0x30, 0x00, 0x00, // ,
    0x08, 0x08, 0x08, 0x08, 0x08, // -
    0x00, 0x60, 0x60, 0x00, 0x00, // .
    0x20, 0x10, 0x08, 0x04, 0x02, // /
    0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
    0x00, 0x42, 0x7F, 0x40, 0x00, // 1
    0x42, 0x61, 0x51, 0x49, 0x46, // 2
    0x21, 0x41, 0x45, 0x4B, 0x31, // 3
    0x18, 0x14, 0x12, 0x7F, 0x10, // 4
    0x27, 0x45, 0x45, 0x45, 0x39, // 5
    0x3C, 0x4A, 0x49, 0x49, 0x30, // 6
    0x01, 0x71, 0x09, 0x05, 0x03, // 7
    0x36, 0x49, 0x49, 0x49, 0x36, // 8
    0x06, 0x49, 0x49, 0x29, 0x1E, // 9
    0x00, 0x36, 0x36, 0x00, 0x00, // :
    0x00, 0x56, 0x36, 0x00, 0x00, // ;
    0x08, 0x14, 0x22, 0x41, 0x00, // <
    0x14, 0x14, 0x14, 0x14, 0x14, // =
    0x00, 0x41, 0x22, 0x14, 0x08, // >
    0x02, 0x01, 0x51, 0x09, 0x06, // ?
    0x32, 0x49, 0x79, 0x41, 0x3E, // @
    0x7E, 0x11, 0x11, 0x11, 0x7E, // A
    0x7F, 0x49, 0x49, 0x49, 0x36, // B
    0x3E, 0x41, 0x41, 0x41, 0x22, // C
    0x7F, 0x41, 0x41, 0x22, 0x1C, // D
    0x7F, 0x49, 0x49, 0x49, 0x41, // E
    0x7F, 0x09, 0x09, 0x09, 0x01, // F
    0x3E, 0x41, 0x49, 0x49, 0x7A, // G
    0x7F, 0x08, 0x08, 0x08, 0x7F, // H
    0x00, 0x41, 0x7F, 0x41, 0x00, // I
    0x20, 0x40, 0x41, 0x3F, 0x01, // J
    0x7F, 0x08, 0x14, 0x22, 0x41, // K
    0x7F, 0x40, 0x40, 0x40, 0x40, // L
    0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
    0x7F, 0x04, 0x08, 0x10, 0x7F, // N
    0x3E, 0x41, 0x41, 0x41, 0x3E, // O
    0x7F, 0x09, 0x09, 0x09, 0x06, // P
    0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
    0x7F, 0x09, 0x19, 0x29, 0x46, // R
    0x46, 0x49, 0x49, 0x49, 0x31, // S
    0x01, 0x01, 0x7F, 0x01, 0x01, // T
    0x3F, 0x40, 0x40, 0x40, 0x3F, // U
    0x1F, 0x20, 0x40, 0x20, 0x1F, // V
    0x3F, 0x40, 0x38, 0x40, 0x3F, // W
    0x63, 0x14, 0x08, 0x14, 0x63, // X
    0x07, 0x08, 0x70, 0x08, 0x07, // Y
    0x61, 0x51, 0x49, 0x45, 0x43, // Z
    0x00, 0x7F, 0x41, 0x41, 0x00, // [
    0x02, 0x04, 0x08, 0x10, 0x20, // (Backslash)
    0x00, 0x41, 0x41, 0x7F, 0x00, // ]
    0x04, 0x02, 0x01, 0x02, 0x04, // ^
    0x40, 0x40, 0x40, 0x40, 0x40, // _
    0x00, 0x01, 0x02, 0x04, 0x00, // `
    0x20, 0x54, 0x54, 0x54, 0x78, // a
    0x7F, 0x48, 0x44, 0x44, 0x38, // b
    0x38, 0x44, 0x44, 0x44, 0x20, // c
    0x38, 0x44, 0x44, 0x48, 0x7F, // d
    0x38, 0x54, 0x54, 0x54, 0x18, // e
    0x08, 0x7E, 0x09, 0x01, 0x02, // f
    0x0C, 0x52, 0x52, 0x52, 0x3E, // g
    0x7F, 0x08, 0x04, 0x04, 0x78, // h
    0x00, 0x44, 0x7D, 0x40, 0x00, // i
    0x20, 0x40, 0x44, 0x3D, 0x00, // j
    0x7F, 0x10, 0x28, 0x44, 0x00, // k
    0x00, 0x41, 0x7F, 0x40, 0x00, // l
    0x7C, 0x04, 0x18, 0x04, 0x78, // m
    0x7C, 0x08, 0x04, 0x04, 0x78, // n
    0x38, 0x44, 0x44, 0x44, 0x38, // o
    0x7C, 0x14, 0x14, 0x14, 0x08, // p
    0x08, 0x14, 0x14, 0x18, 0x7C, // q
    0x7C, 0x08, 0x04, 0x04, 0x08, // r
    0x48, 0x54, 0x54, 0x54, 0x20, // s
    0x04, 0x3F, 0x44, 0x40, 0x20, // t
    0x3C, 0x40, 0x40, 0x20, 0x7C, // u
    0x1C, 0x20, 0x40, 0x20, 0x1C, // v
    0x3C, 0x40, 0x30, 0x40, 0x3C, // w
    0x44, 0x28, 0x10, 0x28, 0x44, // x
    0x0C, 0x50, 0x50, 0x50, 0x3C, // y
    0x44, 0x64, 0x54, 0x4C, 0x44  // z
};

Font *Font::CreateDefault() {
  // Create an image large enough. 95 chars (32-126). 5 bytes each. 1 pixel
  // vertical separator? No, tightly packed in x. Let's lay them out linearly
  // for simplicity. 95 chars * 6 pixels wide (5 + 1 spacing). Width = 570.
  int w = 600;
  int h = 8;
  Image *img = new Image(w, h);

  // Clear image
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      img->SetPixel(x, y, Color(0, 0, 0, 0));
    }
  }

  Font *font = new Font(img, true);

  int xoff = 0;

  // Iterate ascii 32 to 126
  for (int c = 32; c <= 126; ++c) {
    int idx = (c - 32) * 5;
    if (idx >= sizeof(font5x7))
      break; // Safety

    // Define char rect
    FontChar fc;
    fc.imgrect.x = xoff;
    fc.imgrect.y = 0;
    fc.imgrect.width = 5;
    fc.imgrect.height = 7;
    fc.offset.x = 0;
    fc.offset.y = 0;
    fc.advance = 6;

    font->charmap[c] = fc;

    // Draw pixels
    for (int col = 0; col < 5; ++col) {
      byte colData = font5x7[idx + col];
      for (int row = 0; row < 7; ++row) {
        if ((colData >> row) & 1) {
          img->SetPixel(xoff + col, row, Color(255, 255, 255));
        }
      }
    }
    xoff += 6;
  }

  return font;
}

Font *Font::CreateBold() {
  // Generate Bold font based on the 5x7 data
  // Char width 5 -> 6. Advance 6 -> 7.
  // We thicken by adding a pixel to the right of every set pixel.

  int w = 700; // 95 chars * 7 pixels wide
  int h = 8;
  Image *img = new Image(w, h);

  // Clear image
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      img->SetPixel(x, y, Color(0, 0, 0, 0));
    }
  }

  Font *font = new Font(img, true);
  font->lineheight = 8;

  int xoff = 0;

  // Iterate ascii 32 to 126
  for (int c = 32; c <= 126; ++c) {
    int idx = (c - 32) * 5;
    if (idx >= (int)sizeof(font5x7))
      break;

    // Define char rect
    FontChar fc;
    fc.imgrect.x = xoff;
    fc.imgrect.y = 0;
    fc.imgrect.width = 6; // Thicker
    fc.imgrect.height = 7;
    fc.offset.x = 0;
    fc.offset.y = 0;
    fc.advance = 7;

    font->charmap[c] = fc;

    // Draw pixels
    for (int col = 0; col < 5; ++col) {
      byte colData = font5x7[idx + col];
      for (int row = 0; row < 7; ++row) {
        if ((colData >> row) & 1) {
          img->SetPixel(xoff + col, row, Color(255, 255, 255));
          img->SetPixel(xoff + col + 1, row, Color(255, 255, 255)); // Bold
        }
      }
    }
    xoff += 7;
  }

  return font;
}

Font *Font::LoadFromTTF(const String &filename, float fontSize,
                        int textureWidth, int textureHeight) {
  // Read TTF file content
  std::string buffer;
  try {
    buffer = File::ReadAsText(filename).stl();
  } catch (...) {
    std::cout << "Failed to read TTF file: " << filename.c_str() << std::endl;
    return nullptr;
  }

  if (buffer.empty()) {
    std::cout << "Failed to read TTF file (empty): " << filename.c_str()
              << std::endl;
    return nullptr;
  }

  // Init font info to get metrics
  stbtt_fontinfo info;
  if (!stbtt_InitFont(&info, (const unsigned char *)buffer.data(), 0)) {
    std::cout << "Failed to init font info." << std::endl;
    return nullptr;
  }

  // Get vertical metrics
  int ascent, descent, lineGap;
  stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

  float scale = stbtt_ScaleForPixelHeight(&info, fontSize);
  int baseline = (int)(ascent * scale);
  int fontHeight = (int)((ascent - descent + lineGap) * scale);

  // Prepare bitmap buffer
  unsigned char *bitmap = new unsigned char[textureWidth * textureHeight];
  stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs

  // Bake font
  // 32 is the first char (space), 96 is the count
  int ret =
      stbtt_BakeFontBitmap((unsigned char *)buffer.data(), 0, fontSize, bitmap,
                           textureWidth, textureHeight, 32, 96, cdata);

  if (ret <= 0) {
    std::cout << "Failed to bake font bitmap. Texture might be too small."
              << std::endl;
  }

  // Create Image from bitmap (Monochrome for fonts)
  Image *img = new Image();
  img->SetData(textureWidth, textureHeight, false, bitmap);

  delete[] bitmap;

  // Create Font object
  Font *font = new Font(img, true);
  font->lineheight = fontHeight;
  font->linebase = baseline;

  // Populate charmap
  for (int i = 0; i < 96; ++i) {
    int charCode = 32 + i;
    stbtt_bakedchar &b = cdata[i];

    FontChar fc;
    fc.imgrect.x = b.x0;
    fc.imgrect.y = b.y0;
    fc.imgrect.width = b.x1 - b.x0;
    fc.imgrect.height = b.y1 - b.y0;

    // yoff is offset from baseline to top-left of glyph.
    // We want offset from top-left of line to top-left of glyph.
    // Top-left of line is at y = 0. Baseline is at y = baseline.
    fc.offset.x = (int)b.xoff;
    fc.offset.y = (int)b.yoff + baseline;

    fc.advance = (int)b.xadvance;

    font->charmap[charCode] = fc;
  }

  return font;
}
