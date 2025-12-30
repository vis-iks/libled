#include "ImpressiveDemo.h"
#include <cmath>

ImpressiveDemo::ImpressiveDemo(const Font& f, const Image& tex) 
    : font(f), texture(tex), flashCount(0)
{
    // Create individual character Text objects
    const char* text = "IMPRESSIVE";
    for (int i = 0; i < 10; i++) {
        char str[2] = {text[i], '\0'};
        characters[i] = Text(f, HorizontalAlign::Left, VerticalAlign::Top);
        characters[i].SetText(str);
        flashProgress[i] = 0.0f;
    }
    
    // Calculate character positions (centered)
    Text fullWord(f, HorizontalAlign::Center, VerticalAlign::Middle);
    fullWord.SetText("IMPRESSIVE");
    Rect wordRect = fullWord.GetTextRect(Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2));
    int x = wordRect.x;
    for (int i = 0; i < 10; i++) {
        charPos[i] = Point(x, wordRect.y);
        x += characters[i].GetTextSize().width;
    }
    
    // Create particle system
    particles = std::make_shared<ParticleSystemEffect>();
    particles->SetBaseColor(Color(255, 255, 200));
    particles->SetGravity(false);
    
    Start();
}

void ImpressiveDemo::Start() 
{
    startTime = Clock::now();
    lastStepTime = Clock::now();
    nextFlashTime = startTime + ch::milliseconds(FLASH_INTERVAL_MS);
    offset = tweeny::from(80).to(-160).during(TOTAL_DURATION_MS);
    flashCount = 0;
    for (int i = 0; i < 10; i++) {
        flashProgress[i] = 0.0f;
    }
}

void ImpressiveDemo::Render(Canvas& canvas, uint32_t timeMs) 
{
    TimePoint now = Clock::now();
    
    // Check if animation finished or not started - restart
    if (!ch::IsTimeSet(startTime) || (now > (startTime + ch::milliseconds(TOTAL_DURATION_MS)))) {
        Start();
    }
    
    // Advance time
    int dt = static_cast<int>(ch::ToMilliseconds(now - lastStepTime));
    lastStepTime = now;
    int textOffset = offset.step(dt);
    
    // Flash characters one by one
    if ((flashCount < 10) && (now > nextFlashTime)) {
        flashProgress[flashCount] = 1.0f;
        flashCount++;
        nextFlashTime += ch::milliseconds(FLASH_INTERVAL_MS);
        
        // Spawn particles from this character
        for (int i = 0; i < PARTICLES_PER_CHAR; i++) {
            float angle = (rand() % 360) * 3.14159f / 180.0f;
            float speed = 0.3f + (rand() % 100) / 100.0f * 0.7f;
            
            int px = charPos[flashCount - 1].x + characters[flashCount - 1].GetTextSize().width / 2 + textOffset;
            int py = charPos[flashCount - 1].y + characters[flashCount - 1].GetTextSize().height / 2;
            
            // Draw a particle manually
            if (px >= 0 && px < DISPLAY_WIDTH && py >= 0 && py < DISPLAY_HEIGHT) {
                canvas.SetPixel(px, py, Color(255, 255, rand() % 255));
            }
        }
    }
    
    // Render particles effect
    particles->Render(canvas, timeMs);
    
    // Calculate texture offset (scrolling)
    Point texOffset = Point(
        static_cast<int>(ch::ToMilliseconds(now - startTime)) / 32,
        static_cast<int>(ch::ToMilliseconds(now - startTime)) / 32
    );
    
    // Render revealed characters
    for (int i = 0; i < flashCount; i++) {
        Point p = charPos[i];
        p.x += textOffset;
        // Wave motion
        p.y += static_cast<int>(roundf(sin(static_cast<float>(p.x) / 4.0f) * 2.0f));
        
        // Draw outline
        characters[i].DrawOutlineMask(canvas, p, 2, BLACK);
        // Draw textured text
        characters[i].DrawTexturedMask(canvas, p, texture, texOffset);
        
        // Simple flash effect - draw white overlay that fades
        if (flashProgress[i] > 0.0f) {
            byte alpha = static_cast<byte>(flashProgress[i] * 255.0f);
            characters[i].DrawMask(canvas, p, Color(255, 255, 255, alpha));
            flashProgress[i] -= 0.1f;
            if (flashProgress[i] < 0.0f) flashProgress[i] = 0.0f;
        }
    }
}
