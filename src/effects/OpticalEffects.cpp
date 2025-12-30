#include "effects/OpticalEffects.h"
#include <vector>

namespace libled {

void FadeEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    if (source) source->Render(canvas, timeMs);
    
    // Apply brightness
    if (brightness >= 1.0f) return;
    
    int width = canvas.Width();
    int height = canvas.Height();
    
    byte b = (byte)(brightness * 255.0f);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Color c = canvas.GetPixel(x, y);
            c.ModulateRGBA(b);
            canvas.SetPixel(x, y, c);
        }
    }
}

void BlurEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    if (source) source->Render(canvas, timeMs);
    
    // Copy canvas to temp buffer
    int width = canvas.Width();
    int height = canvas.Height();
    std::vector<Color> buffer(width * height);
    
    const Color* src = canvas.GetBuffer();
    std::copy(src, src + (width * height), buffer.begin());
    
    // Box blur 3x3
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int r = 0, g = 0, b = 0;
            int count = 0;
            
            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    int ny = y + ky;
                    int nx = x + kx;
                    
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        Color c = buffer[ny * width + nx];
                        r += c.r;
                        g += c.g;
                        b += c.b;
                        count++;
                    }
                }
            }
            
            canvas.SetPixel(x, y, Color(r / count, g / count, b / count));
        }
    }
}

// End of previous methods

void FlashEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    if (source) source->Render(canvas, timeMs);
    
    int t = timeMs % period;
    float intensity = 0.0f;
    if (t < 200) { 
        intensity = 1.0f - (t / 200.0f);
    }
    
    if (intensity > 0) {
        Color white(255, 255, 255);
        white.ModulateA((byte)(intensity * 255));
        canvas.DrawRectangleBlend(Point(0,0), Point(DISPLAY_WIDTH, DISPLAY_HEIGHT), white, white);
    }
}

}
