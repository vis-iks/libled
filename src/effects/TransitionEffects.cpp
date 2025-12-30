#include "effects/TransitionEffects.h"
#include <cstdlib>

namespace libled {

void DissolveEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    // Ping-pong progress logic same as crossfade
    int cycleTime = durationMs * 2;
    int tMs = timeMs % cycleTime;
    float progress;
    if (tMs < durationMs) {
        progress = (float)tMs / (float)durationMs;
    } else {
        progress = 1.0f - ((float)(tMs - durationMs) / (float)durationMs);
    }

    canvasA.Clear(Color(0,0,0,0));
    if (sourceA) sourceA->Render(canvasA, timeMs);
    
    canvasB.Clear(Color(0,0,0,0));
    if (sourceB) sourceB->Render(canvasB, timeMs);
    
    // Draw A
    canvas.DrawColorImage(Point(0,0), canvasA);
    
    // Dissolve B over A
    // Use a pseudo-random threshold based on x,y
    for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
        for (int x = 0; x < DISPLAY_WIDTH; ++x) {
             // Deterministic noise
             int noise = (x * 37 + y * 17) % 100; // 0-99
             float threshold = progress * 100.0f;
             
             if (noise < threshold) {
                 Color c = canvasB.GetPixel(x, y);
                 // Alpha blend if c has transparency
                 canvas.BlendPixel(x, y, c);
             }
        }
    }
}

MeltTransitionEffect::MeltTransitionEffect(std::shared_ptr<IEffect> src, std::shared_ptr<IEffect> dst, uint32_t dur)
    : source(src), dest(dst), duration(dur), startTime(0), started(false), 
      columnOffsets(DISPLAY_WIDTH, 0), initialized(false)
{
}

void MeltTransitionEffect::Reset()
{
    started = false;
    startTime = 0;
    initialized = false;
    std::fill(columnOffsets.begin(), columnOffsets.end(), 0);
}

void MeltTransitionEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    if (!started) {
        startTime = timeMs;
        started = true;
        
        // Initialize random column offsets (matches original MAX_OFFSET_MS and MAX_DELTA_OFFSET_MS)
        const int MAX_OFFSET_MS = 400;
        const int MAX_DELTA_OFFSET_MS = 100;
        
        int off = (rand() % MAX_OFFSET_MS);
        for (int x = 0; x < DISPLAY_WIDTH; ++x) {
            int delta = (rand() % (MAX_DELTA_OFFSET_MS * 2)) - MAX_DELTA_OFFSET_MS;
            off = std::max(0, std::min(MAX_OFFSET_MS, off + delta));
            columnOffsets[x] = off;
        }
        
        // Capture the source into sourceCanvas
        sourceCanvas.Clear(Color(0,0,0,0));
        if (source) source->Render(sourceCanvas, timeMs);
        initialized = true;
    }
    
    uint32_t elapsed = timeMs - startTime;
    float progress = (float)elapsed / (float)duration;
    
    // Loop the effect when it finishes
    if (progress >= 1.0f) {
        started = false; // Reset for next cycle
        initialized = false;
        return;
    }
    
    // Render destination as background
    destCanvas.Clear(Color(0,0,0,0));
    if (dest) dest->Render(destCanvas, timeMs);
    destCanvas.CopyTo(canvas);
    
    // Melt the source down, column by column
    // Original: offset = max(dt - offsets[x], 0) / MELT_SPEED
    const int MELT_SPEED = 30; // Milliseconds per pixel
    
    for (int x = 0; x < DISPLAY_WIDTH; ++x) {
        int dt = (int)elapsed - columnOffsets[x];
        if (dt < 0) dt = 0;
        int offset = dt / MELT_SPEED;
        
        // Copy pixels from source with vertical offset
        for (int y = 0; y < DISPLAY_HEIGHT - offset; ++y) {
            Color c = sourceCanvas.GetPixel(x, y);
            if (c.a > 0) {
                canvas.SetPixel(x, y + offset, c);
            }
        }
    }
}


// --- Helper: Get 0..1 progress from timeMs % duration ---
float GetProgress(uint32_t timeMs, int durationMs) {
    int t = timeMs % (durationMs * 2);
    if (t < durationMs) return (float)t / durationMs;
    return 1.0f - (float)(t - durationMs) / durationMs;
}

// --- Slide Transition ---
void SlideTransitionEffect::Render(Canvas& canvas, uint32_t timeMs) {
    float p = GetProgress(timeMs, durationMs);
    
    // Render sources
    canvasA.Clear(Color(0,0,0,0)); if (sourceA) sourceA->Render(canvasA, timeMs);
    canvasB.Clear(Color(0,0,0,0)); if (sourceB) sourceB->Render(canvasB, timeMs);
    
    int w = DISPLAY_WIDTH;
    int h = DISPLAY_HEIGHT;
    int xOffset = 0, yOffset = 0;
    
    if (dir == TransitionDirection::Left) xOffset = (int)(w * p);
    else if (dir == TransitionDirection::Right) xOffset = -(int)(w * p);
    else if (dir == TransitionDirection::Up) yOffset = (int)(h * p);
    else if (dir == TransitionDirection::Down) yOffset = -(int)(h * p);
    
    // Draw A (moving out)
    // If Left: A moves left (-xOffset)
    // Wait, standard slide: B pushes A.
    // Let's implement: A stays, B slides OVER A? Or Push?
    // Push is nicer.
    
    // Logic for Push Left:
    // A starts at 0, ends at -W.
    // B starts at W, ends at 0.
    
    Point posA(0,0), posB(0,0);
    
    if (dir == TransitionDirection::Left) {
         posA.x = -(int)(w * p);
         posB.x = w - (int)(w * p);
    } else if (dir == TransitionDirection::Right) {
        posA.x = (int)(w * p);
        posB.x = -w + (int)(w * p);
    } // ... implement others similarly if needed, sticking to L/R for now as primary
    else if (dir == TransitionDirection::Up) {
        posA.y = -(int)(h * p);
        posB.y = h - (int)(h * p);
    } else {
        posA.y = (int)(h * p);
        posB.y = -h + (int)(h * p);
    }

    canvas.DrawColorImage(posA, canvasA);
    canvas.DrawColorImage(posB, canvasB);
}

// --- Wipe Transition ---
void WipeTransitionEffect::Render(Canvas& canvas, uint32_t timeMs) {
    float p = GetProgress(timeMs, durationMs);
    
    canvasA.Clear(Color(0,0,0,0)); if (sourceA) sourceA->Render(canvasA, timeMs);
    canvasB.Clear(Color(0,0,0,0)); if (sourceB) sourceB->Render(canvasB, timeMs);
    
    // Draw A first
    canvas.DrawColorImage(Point(0,0), canvasA);
    
    // Draw B clipped
    // If wipe left: B appears from Right? Or Wipe Right (reveal B from Left)?
    // Usually Wipe Right means the line moves right, revealing B.
    
    int w = DISPLAY_WIDTH;
    int h = DISPLAY_HEIGHT;
    Rect clipRect(0, 0, w, h);
    
    if (dir == TransitionDirection::Right) {
        // Wipe line moves 0 -> W. B is 0..line
        clipRect.width = (int)(w * p);
    } else if (dir == TransitionDirection::Left) {
        // Wipe line moves W -> 0. B is line..W
        int line = w - (int)(w * p);
        clipRect.x = line;
        clipRect.width = (int)(w * p);
    } else if (dir == TransitionDirection::Down) {
        clipRect.height = (int)(h * p);
    } else {
        int line = h - (int)(h * p);
        clipRect.y = line;
        clipRect.height = (int)(h * p);
    }
    
    // Helper to draw part of B
    // We can use DrawColorImage logic but custom rects
    // Or just manual loop for clipping since we have direct access and it's fast
    
    for (int y = clipRect.y; y < clipRect.y + clipRect.height; ++y) {
        for (int x = clipRect.x; x < clipRect.x + clipRect.width; ++x) {
            if (x >= 0 && x < w && y >= 0 && y < h) {
                canvas.SetPixel(x, y, canvasB.GetPixel(x, y));
            }
        }
    }
}

// --- Zoom Transition ---
void ZoomTransitionEffect::Render(Canvas& canvas, uint32_t timeMs) {
    float p = GetProgress(timeMs, durationMs);
    
    canvasA.Clear(Color(0,0,0,0)); if (sourceA) sourceA->Render(canvasA, timeMs);
    canvasB.Clear(Color(0,0,0,0)); if (sourceB) sourceB->Render(canvasB, timeMs);
    
    // Zoom In: A scales up and fades out? Or B scales in from 0?
    // Let's do: Scale B from 0 to 1 over A.
    
    canvas.DrawColorImage(Point(0,0), canvasA);
    
    // Nearest neighbor scale B
    // Center is (W/2, H/2)
    float scale = p;
    if (scale <= 0.01f) return;
    
    int w = DISPLAY_WIDTH;
    int h = DISPLAY_HEIGHT;
    
    int newW = (int)(w * scale);
    int newH = (int)(h * scale);
    int offX = (w - newW) / 2;
    int offY = (h - newH) / 2;
    
    for (int y = 0; y < newH; ++y) {
        for (int x = 0; x < newW; ++x) {
            // Map to source B coordinates
            int srcX = (int)((x / (float)newW) * w);
            int srcY = (int)((y / (float)newH) * h);
            
            Color c = canvasB.GetPixel(srcX, srcY);
            if (c.a > 0) {
                canvas.SetPixel(x + offX, y + offY, c);
            }
        }
    }
}

}
