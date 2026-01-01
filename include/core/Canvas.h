#pragma once
#include "core/GraphicsConstants.h"
#include "core/Image.h"
#include "core/Rect.h"

class Canvas final : public virtual IImage {
private:
  // The buffer to which we draw
  std::vector<Color> renderbuffer;

  // Helper method to prepare for image drawing. This clips input coordinates,
  // modifies the image rect and determines the drawing rect. Returns false when
  // the image is completely outside the display, otherwise returns true.
  bool PrepareImageDraw(Point pos, const IImage &img, Rect &imgrect,
                        Rect &drawrect);

public:
  // Constructor/destructor
  Canvas();
  ~Canvas();

  // Resize buffer
  void Resize(int width, int height);

  // Direct buffer access
  inline const Color *GetBuffer() const { return renderbuffer.data(); }

  // IImage implementation
  virtual bool HasColors() const override final { return true; }
  virtual int Width() const override final { return DISPLAY_WIDTH; }
  virtual int Height() const override final { return DISPLAY_HEIGHT; }
  virtual Size GetSize() const override final {
    return Size(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  }
  virtual const byte *ByteData() const override final {
    return reinterpret_cast<const byte *>(renderbuffer.data());
  }
  virtual const Color *ColorData() const override final {
    return renderbuffer.data();
  }
  virtual MonoSampler GetMonoSampler() const override final {
    NOT_SUPPORTED;
    return MonoSampler(nullptr, 0);
  }
  virtual ColorSampler GetColorSampler() const override final {
    return ColorSampler(renderbuffer.data(), DISPLAY_WIDTH);
  }

  // Rasterizing methods
  void Clear(Color color);
  void CopyTo(Canvas &canvas) const {
    if (canvas.renderbuffer.size() != renderbuffer.size())
      canvas.renderbuffer.resize(renderbuffer.size());
    memcpy(canvas.renderbuffer.data(), renderbuffer.data(),
           renderbuffer.size() * sizeof(Color));
  }
  void CopyRegion(const Canvas &source, Rect sourceRect, Point destPoint);
  void WriteToFile(String filename) const;
  inline void SetPixel(int x, int y, Color c) {
    if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT)
      renderbuffer[y * DISPLAY_WIDTH + x] = c;
  }
  inline Color GetPixel(int x, int y) const {
    if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT)
      return renderbuffer[y * DISPLAY_WIDTH + x];
    return Color(0, 0, 0, 0);
  }
  inline void BlendPixel(int x, int y, Color c) {
    if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT)
      renderbuffer[y * DISPLAY_WIDTH + x].Blend(c);
  }
  inline void AddPixel(int x, int y, Color c) {
    if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT)
      renderbuffer[y * DISPLAY_WIDTH + x].Add(c);
  }
  inline void MaskPixel(int x, int y, Color c) {
    if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT)
      renderbuffer[y * DISPLAY_WIDTH + x].Mask(c);
  }
  void DrawLine(Point p1, Point p2, Color color);
  void DrawLineBlend(Point p1, Point p2, Color color);
  void DrawRectangle(Point p1, Point p2, Color linecolor, Color fillcolor);
  void DrawRectangleBlend(Point p1, Point p2, Color linecolor, Color fillcolor);
  void DrawColorImage(Point pos, const IImage &img) {
    DrawColorImage(pos, img, Rect(Point(0, 0), img.GetSize()));
  }
  void DrawColorImage(Point pos, const IImage &img, Rect imgrect);
  void DrawColorImageBlend(Point pos, const IImage &img) {
    DrawColorImageBlend(pos, img, Rect(Point(0, 0), img.GetSize()));
  }
  void DrawColorImageBlend(Point pos, const IImage &img, Rect imgrect);
  void DrawColorImageAdd(Point pos, const IImage &img) {
    DrawColorImageAdd(pos, img, Rect(Point(0, 0), img.GetSize()));
  }
  void DrawColorImageAdd(Point pos, const IImage &img, Rect imgrect);
  void DrawColorImageMask(Point pos, const IImage &img) {
    DrawColorImageMask(pos, img, Rect(Point(0, 0), img.GetSize()));
  }
  void DrawColorImageMask(Point pos, const IImage &img, Rect imgrect);

  // Modulated drawing for transitions
  void DrawColorImageMod(Point pos, const IImage &img, Color mod) {
    DrawColorImageMod(pos, img, mod, Rect(Point(0, 0), img.GetSize()));
  }
  void DrawColorImageMod(Point pos, const IImage &img, Color mod, Rect imgrect);
  void DrawMonoImage(Point pos, const IImage &img, Color color) {
    DrawMonoImage(pos, img, color, Rect(Point(0, 0), img.GetSize()));
  }
  void DrawMonoImage(Point pos, const IImage &img, Color color, Rect imgrect);
  void DrawMonoImageBlend(Point pos, const IImage &img, Color color) {
    DrawMonoImageBlend(pos, img, color, Rect(Point(0, 0), img.GetSize()));
  }
  void DrawMonoImageBlend(Point pos, const IImage &img, Color color,
                          Rect imgrect);
  void DrawMonoImageAdd(Point pos, const IImage &img, Color color) {
    DrawMonoImageAdd(pos, img, color, Rect(Point(0, 0), img.GetSize()));
  }
  void DrawMonoImageAdd(Point pos, const IImage &img, Color color,
                        Rect imgrect);
  void DrawMonoImageMask(Point pos, const IImage &img, Color color) {
    DrawMonoImageMask(pos, img, color, Rect(Point(0, 0), img.GetSize()));
  }
  void DrawMonoImageMask(Point pos, const IImage &img, Color color,
                         Rect imgrect);
  void DrawMonoTextured(Point pos, const IImage &img, const IImage &tex,
                        Point texoffset = Point()) {
    DrawMonoTextured(pos, img, tex, texoffset,
                     Rect(Point(0, 0), img.GetSize()));
  }
  void DrawMonoTextured(Point pos, const IImage &img, const IImage &tex,
                        Point texoffset, Rect imgrect);
  void DrawMonoTexturedMask(Point pos, const IImage &img, const IImage &tex,
                            Point texoffset = Point()) {
    DrawMonoTexturedMask(pos, img, tex, texoffset,
                         Rect(Point(0, 0), img.GetSize()));
  }
  void DrawMonoTexturedMask(Point pos, const IImage &img, const IImage &tex,
                            Point texoffset, Rect imgrect);
  void DrawMonoTexturedBlend(Point pos, const IImage &img, const IImage &tex,
                             Point texoffset = Point()) {
    DrawMonoTexturedBlend(pos, img, tex, texoffset,
                          Rect(Point(0, 0), img.GetSize()));
  }
  void DrawMonoTexturedBlend(Point pos, const IImage &img, const IImage &tex,
                             Point texoffset, Rect imgrect);
  void DrawMonoTexturedAdd(Point pos, const IImage &img, const IImage &tex,
                           Point texoffset = Point()) {
    DrawMonoTexturedAdd(pos, img, tex, texoffset,
                        Rect(Point(0, 0), img.GetSize()));
  }
  void DrawMonoTexturedAdd(Point pos, const IImage &img, const IImage &tex,
                           Point texoffset, Rect imgrect);
  void DrawMonoTexturedMod(Point pos, const IImage &img, const IImage &tex,
                           Color mod, Point texoffset = Point()) {
    DrawMonoTexturedMod(pos, img, tex, mod, texoffset,
                        Rect(Point(0, 0), img.GetSize()));
  }
  void DrawMonoTexturedMod(Point pos, const IImage &img, const IImage &tex,
                           Color mod, Point texoffset, Rect imgrect);
  void DrawMonoTexturedModMask(Point pos, const IImage &img, const IImage &tex,
                               Color mod, Point texoffset = Point()) {
    DrawMonoTexturedModMask(pos, img, tex, mod, texoffset,
                            Rect(Point(0, 0), img.GetSize()));
  }
  void DrawMonoTexturedModMask(Point pos, const IImage &img, const IImage &tex,
                               Color mod, Point texoffset, Rect imgrect);
  void DrawMonoTexturedModBlend(Point pos, const IImage &img, const IImage &tex,
                                Color mod, Point texoffset = Point()) {
    DrawMonoTexturedModBlend(pos, img, tex, mod, texoffset,
                             Rect(Point(0, 0), img.GetSize()));
  }
  void DrawMonoTexturedModBlend(Point pos, const IImage &img, const IImage &tex,
                                Color mod, Point texoffset, Rect imgrect);
  void DrawMonoTexturedModAdd(Point pos, const IImage &img, const IImage &tex,
                              Color mod, Point texoffset = Point()) {
    DrawMonoTexturedModAdd(pos, img, tex, mod, texoffset,
                           Rect(Point(0, 0), img.GetSize()));
  }
  void DrawMonoTexturedModAdd(Point pos, const IImage &img, const IImage &tex,
                              Color mod, Point texoffset, Rect imgrect);
};
