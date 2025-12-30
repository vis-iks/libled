#pragma once
#include "IEffect.h"
#include "utils/Text.h"
#include "utils/Tools.h"
#include "core/IImage.h"

namespace libled {

class TextScrollEffect : public IEffect
{
private:
    Text text;
    Color color;
    Point currentPos;
    int speed; // pixels per second
    uint32_t lastUpdate;
    float accumulatedMove;

public:
    TextScrollEffect(const Text& t, Color c, int s = 20) 
        : text(t), color(c), speed(s), currentPos(DISPLAY_WIDTH, 0), lastUpdate(0), accumulatedMove(0.0f)
    {
        // Default verify alignment
        // Vertical align middle?
        // text.SetVerticalAlign(VerticalAlign::Middle);
    }
    
    // Set text position y?
    void SetY(int y) { currentPos.y = y; }
    
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
    
    virtual void Reset() override {
        currentPos.x = DISPLAY_WIDTH;
        lastUpdate = 0;
        accumulatedMove = 0.0f;
    }
};

class TextEffect : public IEffect
{
private:
    Text text;
    Color color;
    Point position;

public:
    TextEffect(const Text& t, Color c, Point pos = Point(0, 0)) : text(t), color(c), position(pos) {}
    
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
    
    void SetText(const String& str) { text.SetText(str); }
    void SetColor(Color c) { color = c; }
    void SetPosition(Point p) { position = p; }
};

class TypewriterEffect : public IEffect
{
private:
    Text text;
    Color color;
    Point position;
    int cursor; // Current char index
    uint32_t lastUpdate;
    int speedMs; // Ms per char

public:
    TypewriterEffect(const Text& t, Color c, int speed = 100) 
        : text(t), color(c), position(Point(0,0)), cursor(0), lastUpdate(0), speedMs(speed) {}
    
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
    virtual void Reset() override { cursor = 0; lastUpdate = 0; }
    void SetPosition(Point p) { position = p; }
};

class TextWaveEffect : public IEffect
{
private:
    Text text;
    Color color;
    Point position;
    
public:
    TextWaveEffect(const Text& t, Color c) : text(t), color(c), position(Point(0,0)) {}
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
    void SetPosition(Point p) { position = p; }
};

class TextBounceEffect : public IEffect
{
private:
    Text text;
    Color color;
    Point position;
    
public:
    TextBounceEffect(const Text& t, Color c) : text(t), color(c), position(Point(0,0)) {}
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
    void SetPosition(Point p) { position = p; }
};

class TextFlashEffect : public IEffect
{
private:
    Text text;
    Point position;
    uint32_t duration;
    uint32_t startTime;
    bool started;

public:
    TextFlashEffect(const Text& t, Point pos, uint32_t duration = 400);
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
    virtual void Reset() override;
    virtual bool IsFinished() const override;
};

/**
 * TextShineEffect - Creates an animated diagonal "shine" that sweeps across text or images
 * 
 * This effect renders a bright highlight that moves diagonally across the rendered content,
 * creating a glossy/shiny appearance. Commonly used for title screens and highlighting.
 */
class TextShineEffect : public IEffect
{
private:
    static constexpr int SHINE_WIDTH = 32;
    
    TimePoint lastTime;
    TweenInt offset;
    Canvas tempCanvas;
    Size targetSize;
    bool active;
    
    // Helper method to draw the shine effect
    void DrawShine(Canvas& canvas, Rect targetRect);
    
public:
    TextShineEffect();
    
    /**
     * Begin a new shine animation
     * @param size The size of the content to shine over
     * @param duration Duration of the shine animation in milliseconds (default: 300ms)
     * @param easing The easing function to use (default: cubicInOut)
     */
    void Begin(Size size, int duration = 300, easing::enumerated easing = easing::enumerated::cubicInOut);
    
    /**
     * Draw the shine effect over text
     * @param canvas Target canvas
     * @param text Text to shine
     * @param outline Outline width of the text
     * @param pos Position of the text
     */
    void Draw(Canvas& canvas, const Text& text, int outline, Point pos);
    
    /**
     * Draw the shine effect over an image
     * @param canvas Target canvas
     * @param image Image to shine
     * @param pos Position of the image
     */
    void Draw(Canvas& canvas, const IImage& image, Point pos);
    
    /**
     * Get the current progress of the animation (0.0 to 1.0)
     */
    float GetProgress() const;
    
    /**
     * Check if the shine animation is currently active
     */
    bool IsActive() const { return active; }
    
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
    virtual void Reset() override;
    virtual bool IsFinished() const override;
};

/**
 * TextScaleEffect - Scales text from zero to full size or vice versa
 * Uses aspect-ratio-preserving scaling with nearest-neighbor algorithm
 */
class TextScaleEffect : public IEffect
{
private:
    Text text;
    Canvas tempCanvas;
    const IImage* texture;  // Optional textured rendering
    Point position;
    TweenInt widthTween;
    TimePoint startTime;
    TimePoint lastStepTime;
    bool active;
    
public:
    /**
     * Constructor
     */
    TextScaleEffect();
    
    /**
     * Begin scaling animation
     * @param text Text object to scale
     * @param texture Optional texture for rendering (nullptr for solid color)
     * @param fromScale Starting scale (1.0 = 100%)
     * @param toScale Ending scale (1.0 = 100%)
     * @param duration Animation duration in milliseconds
     * @param waitMs Time to wait at target size before finishing (milliseconds)
     * @param easing Easing function
     */
    void Begin(const Text& text, const IImage* texture, float fromScale, float toScale, 
               int duration, int waitMs = 0, easing::enumerated easing = easing::enumerated::linear);
    
    /**
     * Set the position where scaled text will be drawn
     */
    void SetPosition(Point pos) { position = pos; }
    
    /**
     * Get current progress (0.0 to 1.0)
     */
    float GetProgress() const { return widthTween.progress(); }
    
    /**
     * Check if animation is active
     */
    bool IsActive() const { return active; }
    
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
    virtual void Reset() override;
    virtual bool IsFinished() const override;
};


/**
 * ShadowTextEffect - Draws text with a shadow offset
 */
class ShadowTextEffect : public IEffect
{
private:
    Text text;
    Color color;
    Point position;
    
public:
    ShadowTextEffect(const Text& t, Color c, Point pos = Point(0, 0)) : text(t), color(c), position(pos) {}
    
    virtual void Render(Canvas& canvas, uint32_t timeMs) override;
    
    void SetText(const String& str) { text.SetText(str); }
    void SetColor(Color c) { color = c; }
    void SetPosition(Point p) { position = p; }
};
}
