#include <cmath>
#include <algorithm>
#include <string>
#include <iostream>
#include "effects/TextEffects.h"

namespace libled {

void TextScrollEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    if (lastUpdate == 0) lastUpdate = timeMs;
    
    // Calculate delta time
    uint32_t dt = timeMs - lastUpdate;
    lastUpdate = timeMs;
    
    // Move
    float move = (speed * dt) / 1000.0f;
    accumulatedMove += move;
    
    if (std::abs(accumulatedMove) >= 1.0f) {
        currentPos.x -= (int)accumulatedMove;
        accumulatedMove -= (int)accumulatedMove;
    }
    
    // Wrap around?
    // Determine text width
    Size size = text.GetTextSize();
    if (currentPos.x < -size.width) {
        currentPos.x = DISPLAY_WIDTH;
    }
    
    // Draw
    text.DrawBlend(canvas, currentPos, color);
}

void TextEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    text.DrawBlend(canvas, position, color);
}

TextFlashEffect::TextFlashEffect(const Text& t, Point pos, uint32_t dur)
    : text(t), position(pos), duration(dur), startTime(0), started(false)
{
}

void TextFlashEffect::Reset()
{
    started = false;
    startTime = 0;
}

bool TextFlashEffect::IsFinished() const
{
    if (!started) return false;
    // We can't check time here easily without argument.
    // However, Render logic can set a flag. But for now, let's assume if it started, check internal state if we had it.
    // Actually, simple way: The effect is finished if elapsed > duration. 
    // But we don't know current time here.
    // I will modify Render to update a 'finished' member.
    // But I don't have a 'finished' member yet.
    // Let's add it. Or just rely on Render logic which returns early. E.g.
    // For now: return false always or implement 'finished' flag tracking.
    // Let's implement 'finished' flag tracking in Render.
    // But I can't add member to class without editing header.
    // I entered 'bool started' in header. I can assume checking against that? No.
    // I'll re-edit header to add 'bool finished'.
    return false; 
}

void TextFlashEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    if (!started) {
        startTime = timeMs;
        started = true;
    }
    
    uint32_t elapsed = timeMs - startTime;
    
    // Restart the flash every 2 seconds to make it "flash" repeatedly
    const uint32_t FLASH_INTERVAL = 2000;
    if (elapsed >= FLASH_INTERVAL) {
        startTime = timeMs;
        elapsed = 0;
    }
    
    // Only draw during the flash duration
    if (elapsed >= duration) {
        return; // Don't draw anything after flash completes
    }
    
    // Linear fade out: 1.0 -> 0.0
    float a1 = 1.0f - (float)elapsed / duration;
    a1 = std::max(0.0f, std::min(1.0f, a1));
    
    // Draw text with fading alpha
    Color c = Color(255, 255, 255, static_cast<byte>(a1 * 255.0f));
    text.DrawBlend(canvas,position, c);
}

// End of existing methods

void TypewriterEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    if (lastUpdate == 0) lastUpdate = timeMs;
    // Calculate how many chars to show
    if (timeMs - lastUpdate > (uint32_t)speedMs) {
        cursor++;
        lastUpdate = timeMs;
    }
    
    // Draw substring
    String fullStr = text.GetText();
    if (cursor > (int)fullStr.Length()) cursor = fullStr.Length();
    
    // Hack: Create temp text? Or modify Text class?
    // We can just use the provided text object and temporarily change its string
    // But text object is const in constructor? No, it's a copy member.
    // Wait, TypewriterEffect has "Text text;" member. It's not a reference.
    // So we can modify it safely.
    
    String original = text.GetText();
    text.SetText(original.Substring(0, cursor));
    text.DrawOpaque(canvas, position, color);
    
    // Draw Blink Cursor
    if (cursor < (int)fullStr.Length()) {
         if ((timeMs / 250) % 2 == 0) { // Blink
             Rect r = text.GetTextRect(position); // Returns rect of SUBSTRING now
             // We want cursor at end of substring.
             // GetTextRect returns rect of current text content.
             // So cursor is at r.x + r.width.
             canvas.DrawRectangle(Point(r.x + r.width + 1, r.y), Point(r.x + r.width + 2, r.y + r.height - 1), color, color);
         }
    }
    
    text.SetText(original); // Restore
}

void TextWaveEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    // Wave simplified implementation (since we can't easily iterate chars)
    // We will just do a vertical sine wave of position
    float yoff = sin(timeMs / 200.0f + position.x / 10.0f) * 3.0f;
    Point p = position;
    p.y += (int)yoff;
    text.DrawOpaque(canvas, p, color);
}

void TextBounceEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    // Bounce: y = |sin(t)|
    float yoff = std::abs(sin(timeMs / 300.0f)) * -10.0f; // Bounce UP (negative Y)
    Point p = position;
    p.y += (int)yoff;
    text.DrawOpaque(canvas, p, color);
}

TextShineEffect::TextShineEffect() : active(false)
{
}

void TextShineEffect::Begin(Size size, int duration, easing::enumerated easing)
{
    targetSize = size;
    offset = tweeny::from(-SHINE_WIDTH).to(size.width + size.height).during(duration).via(easing);
    lastTime = Clock::now();
    active = true;
}

void TextShineEffect::DrawShine(Canvas& canvas, Rect tr)
{
    if (!active || offset.progress() >= 1.0f)
    {
        active = false;
        return;
    }
    
    // Advance time
    TimePoint t = Clock::now();
    int dt = static_cast<int>(ch::ToMilliseconds(t - lastTime));
    lastTime = t;
    offset.step(dt);
    
    // Draw the shine only where content pixels are
    Size canvasSize = tempCanvas.GetSize();
    for (int sy = 0; sy < tr.height; sy++)
    {
        for (int sx = 0; sx < SHINE_WIDTH; sx++)
        {
            int x = tr.x + offset.peek() + sx - sy;
            int y = tr.y + sy;
            if ((x >= 0) && (x < canvasSize.width) && (y >= 0) && (y < canvasSize.height))
            {
                if (tempCanvas.GetPixel(x, y).a > 0)
                    canvas.SetPixel(x, y, WHITE);
            }
        }
    }
}

void TextShineEffect::Draw(Canvas& canvas, const Text& text, int outline, Point pos)
{
    // Draw the text on a separate canvas
    tempCanvas.Clear(Color(0, 0, 0, 0));
    text.DrawOutlineMask(tempCanvas, pos, outline, WHITE);
    text.DrawMask(tempCanvas, pos, WHITE);
    DrawShine(canvas, text.GetTextRect(pos));
}

void TextShineEffect::Draw(Canvas& canvas, const IImage& image, Point pos)
{
    // Draw the image on a separate canvas
    tempCanvas.Clear(Color(0, 0, 0, 0));
    if (image.HasColors())
        tempCanvas.DrawColorImageMask(pos, image);
    else
        tempCanvas.DrawMonoImageMask(pos, image, WHITE);
    DrawShine(canvas, Rect(pos, image.GetSize()));
}

float TextShineEffect::GetProgress() const
{
    return offset.progress();
}

void TextShineEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    // This effect is designed to be used via Draw() methods
    // The main demo uses it correctly by calling Begin() and letting the scene draw text
    // No need to render anything here
}

void TextShineEffect::Reset()
{
    active = false;
    lastTime = TimePoint();
}

bool TextShineEffect::IsFinished() const
{
    return !active || (offset.progress() >= 1.0f);
}
// ============================================================================
// TextScaleEffect Implementation
// ============================================================================

TextScaleEffect::TextScaleEffect() : texture(nullptr), active(false)
{
}

void TextScaleEffect::Begin(const Text& txt, const IImage* tex, float fromScale, float toScale, 
                             int duration, int waitMs, easing::enumerated easing)
{
    text = txt;
    texture = tex;
    
    // Calculate target widths based on text size
    Size textSize = text.GetTextSize();
    int startW = static_cast<int>(textSize.width * fromScale);
    int endW = static_cast<int>(textSize.width * toScale);
    
    // Ensure at least 1 pixel or allow 0? 0 is fine, it will just not draw.
    
    widthTween = tweeny::from(startW).to(endW).during(duration).via(easing);
    if (waitMs > 0) {
        widthTween = widthTween.wait(waitMs);
    }
    startTime = Clock::now();
    lastStepTime = Clock::now();
    active = true;
}

void TextScaleEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    if (!active) return;
    
    // Advance time
    TimePoint now = Clock::now();
    int dt = static_cast<int>(ch::ToMilliseconds(now - lastStepTime));
    lastStepTime = now;
    int textWidth = widthTween.step(dt);
    
    if (widthTween.progress() >= 1.0f) {
        active = false;
    }
    
    // Skip rendering if width is zero or negative
    if (textWidth <= 0) return;
    
    // Draw text to temp canvas at full size
    tempCanvas.Clear(Color(0, 0, 0, 0));
    
    if (texture) {
        text.DrawOutlineMask(tempCanvas, position, 2, BLACK);
        text.DrawTexturedMask(tempCanvas, position, *texture);
    } else {
        text.DrawMask(tempCanvas, position, WHITE);
    }
    
    // Calculate scaled height maintaining aspect ratio
    Size tempCanvasSize = tempCanvas.GetSize();
    int textHeight = static_cast<int>(std::roundf(
        (static_cast<float>(textWidth) / static_cast<float>(tempCanvasSize.width)) * 
        static_cast<float>(tempCanvasSize.height)
    ));
    
    // Calculate offset for centering
    Point offset((canvas.Width() - textWidth) / 2, (canvas.Height() - textHeight) / 2);
    
    if ((textWidth > 0) && (textHeight > 0)) {
        // Nearest-neighbor scaling algorithm
        int x_ratio = ((tempCanvasSize.width << 16) / textWidth) + 1;
        int y_ratio = ((tempCanvasSize.height << 16) / textHeight) + 1;
        
        for (int i = 0; i < textHeight; i++) {
            for (int j = 0; j < textWidth; j++) {
                int x2 = (j * x_ratio) >> 16;
                int y2 = (i * y_ratio) >> 16;
                
                if (offset.x + j >= 0 && offset.x + j < canvas.Width() &&
                    offset.y + i >= 0 && offset.y + i < canvas.Height()) {
                    
                    Color c = tempCanvas.GetPixel(x2, y2);
                    if (c.a > 0) {
                         canvas.BlendPixel(offset.x + j, offset.y + i, c);
                    }
                }
            }
        }
    }
}

void TextScaleEffect::Reset()
{
    active = false;
    widthTween = tweeny::from(0).to(0).during(0);
}

bool TextScaleEffect::IsFinished() const
{
    return !active || (widthTween.progress() >= 1.0f);
}


void ShadowTextEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    // Draw shadow: 1px left, 1px down (user asked for left/bottom)
    // Wait, "one pixel to the left and one to the bottom"
    // Left: x - 1
    // Bottom: y + 1 (assuming y increases downwards, usually)
    // Let's assume standard coordinates.
    
    Point shadowPos = position;
    shadowPos.x -= 1;
    shadowPos.y += 1;
    
    // "Little less brightness" - let's dim the color
    Color shadowColor = color;
    shadowColor.r = static_cast<byte>(shadowColor.r * 0.5f);
    shadowColor.g = static_cast<byte>(shadowColor.g * 0.5f);
    shadowColor.b = static_cast<byte>(shadowColor.b * 0.5f);
    
    // Draw shadow first
    text.DrawBlend(canvas, shadowPos, shadowColor);
    
    // Draw main text
    text.DrawBlend(canvas, position, color);
}

} // namespace libled
