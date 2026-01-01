#include "effects/AnimationEffect.h"
#include "stb_image.h"
#include <iostream>

namespace libled {

AnimatedImage::AnimatedImage() : loopCount(0) {}

AnimatedImage::~AnimatedImage() {
  for (Image *img : frames) {
    delete img;
  }
  frames.clear();
}

bool AnimatedImage::LoadGif(const String &filename) {
  FILE *f = fopen(filename.c_str(), "rb");
  if (!f) {
    std::cerr << "Failed to open GIF file: " << filename.c_str() << std::endl;
    return false;
  }

  // Read file content
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  fseek(f, 0, SEEK_SET);

  unsigned char *buffer = new unsigned char[size];
  if (fread(buffer, 1, size, f) != size) {
    std::cerr << "Failed to read full GIF file: " << filename.c_str()
              << std::endl;
    fclose(f);
    delete[] buffer;
    return false;
  }
  fclose(f);

  int *delays_data = nullptr;
  int x, y, z, comp;
  // req_comp = 4 for RGBA
  unsigned char *data = stbi_load_gif_from_memory(buffer, size, &delays_data,
                                                  &x, &y, &z, &comp, 4);

  if (!data) {
    std::cerr << "stb_image failed to load GIF: " << stbi_failure_reason()
              << std::endl;
    delete[] buffer;
    return false;
  }

  int frameBytes = x * y * 4;
  for (int i = 0; i < z; ++i) {
    // Copy data for this frame
    // Image expects loading from file directly usually, but we have raw data.
    // We need a helper in Image or just construct it manually via friend or new
    // constructor OR we just hack it by creating empty Image and setting
    // fields. Since Image fields are private, we need 'LoadFromMemory' or
    // similar. For MVP, assume we can add public method to Image or use
    // subclass. Wait, Image has no SetData. IMPORTANT: We need to modify Image
    // class to support setting data from memory!

    // For now, let's just assume we modified Image (I will do that next).
    Image *img = new Image();
    img->SetData(x, y, true, data + (i * frameBytes)); // Copy

    frames.push_back(img);
    delays.push_back(delays_data[i]);
  }

  stbi_image_free(data);
  stbi_image_free(delays_data); // Wait, stbi_load_gif allocates delays array?
                                // yes doc says key is passed
  delete[] buffer;

  return true;
}

const Image *AnimatedImage::GetFrame(int index) const {
  if (index < 0 || index >= frames.size())
    return nullptr;
  return frames[index];
}

int AnimatedImage::GetDelay(int index) const {
  if (index < 0 || index >= delays.size())
    return 100; // Default
  return delays[index];
}

void AnimatedImage::SetTransparentColor(Color color) {
  hasTransparency = true;
  for (Image *img : frames) {
    if (!img->HasColors())
      continue;

    // We need access to modify the image data.
    for (int y = 0; y < img->Height(); ++y) {
      for (int x = 0; x < img->Width(); ++x) {
        Color c = img->GetColor(x, y);
        if (c.r == color.r && c.g == color.g && c.b == color.b) {
          // Set alpha to 0
          c.a = 0;
          img->SetPixel(x, y, c);
        }
      }
    }
  }
}

// AnimationEffect Implementation
AnimationEffect::AnimationEffect(std::shared_ptr<AnimatedImage> image)
    : anim(image), currentFrame(0), lastUpdate(0), accumulator(0),
      playing(true) {}

void AnimationEffect::Render(Canvas &canvas, uint32_t timeMs) {
  if (!anim || anim->GetFrameCount() == 0)
    return;

  if (lastUpdate == 0)
    lastUpdate = timeMs;
  uint32_t dt = timeMs - lastUpdate;
  lastUpdate = timeMs;

  if (playing) {
    accumulator += dt;
    int delay = anim->GetDelay(currentFrame);
    // Minimum delay safety
    if (delay < 10)
      delay = 10;

    if (accumulator >= delay) {
      accumulator -= delay;
      currentFrame = (currentFrame + 1) % anim->GetFrameCount();
    }
  }

  const Image *frame = anim->GetFrame(currentFrame);
  if (frame) {
    if (anim->HasTransparency()) {
      canvas.DrawColorImageBlend(Point(0, 0), *frame);
    } else {
      canvas.DrawColorImage(Point(0, 0), *frame);
    }
  }
}

} // namespace libled
