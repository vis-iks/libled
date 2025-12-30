#include "effects/MotionEffects.h"
#include <cmath>

namespace libled {

void ScrollEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    if (!source) return;

    if (lastUpdate == 0) lastUpdate = timeMs;
    uint32_t dt = timeMs - lastUpdate;
    lastUpdate = timeMs;

    posX += (speedX * dt) / 1000.0f;
    posY += (speedY * dt) / 1000.0f;

    // Create temp buffer
    static Canvas tempCanvas; 
    
    // Clear temp
    tempCanvas.Clear(BLACK);
    
    // Render source to temp
    source->Render(tempCanvas, timeMs);
    
    // Copy temp to simple buffer to handle wrap
    const Color* src = tempCanvas.GetBuffer();
    
    int shiftX = (int)posX % DISPLAY_WIDTH;
    int shiftY = (int)posY % DISPLAY_HEIGHT;
    if (shiftX < 0) shiftX += DISPLAY_WIDTH;
    if (shiftY < 0) shiftY += DISPLAY_HEIGHT;
    
    for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
        for (int x = 0; x < DISPLAY_WIDTH; ++x) {
            // Source coordinates
            int sx = (x - shiftX + DISPLAY_WIDTH) % DISPLAY_WIDTH;
            int sy = (y - shiftY + DISPLAY_HEIGHT) % DISPLAY_HEIGHT;
            
            canvas.SetPixel(x, y, src[sy * DISPLAY_WIDTH + sx]);
        }
    }
}

// End of previous methods
// Remove intermediate closing brace

void ShakeEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    if (!source) return;
    
    // Render source to temp
    static Canvas temp;
    temp.Clear(Color(0,0,0,0));
    source->Render(temp, timeMs);
    
    int dx = (rand() % (intensity * 2)) - intensity;
    int dy = (rand() % (intensity * 2)) - intensity;
    
    // Draw temp to canvas with offset
    canvas.DrawColorImage(Point(dx, dy), temp);
}

void JitterEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    if (!source) return;
    
    static Canvas temp;
    temp.Clear(Color(0,0,0,0));
    source->Render(temp, timeMs);
    
    // Scanline jitter
    for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
        int off = (rand() % 3) - 1; // -1, 0, 1
        for (int x = 0; x < DISPLAY_WIDTH; ++x) {
             // Read from x,y, write to x+off,y
             if (x+off >= 0 && x+off < DISPLAY_WIDTH) {
                 Color c = temp.GetPixel(x, y);
                 canvas.SetPixel(x + off, y, c);
             }
        }
    }
}

}
