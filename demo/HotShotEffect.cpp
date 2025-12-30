#include "HotShotEffect.h"
#include <utils/ShaderUtils.h>
#include "Shaders.h"
#include <utils/Configuration.h>
#include <cmath>
#include <algorithm>
#include <glm/vec3.hpp>

HotShotEffect::HotShotEffect(const Font& f)
    : font(f),
      text1(font, HorizontalAlign::Left, VerticalAlign::Middle),
      text2(font, HorizontalAlign::Right, VerticalAlign::Middle),
      startTime(0),
      started(false)
{
    text1.SetText("HOT");
    text2.SetText("SHOT!");
}

HotShotEffect::~HotShotEffect()
{
    // Font is now a reference, no need to delete
}

void HotShotEffect::Reset()
{
    startTime = 0;
    started = false;
    particles.clear();
}

void HotShotEffect::Render(Canvas& canvas, uint32_t timeMs)
{
    if (!started) {
        startTime = timeMs;
        started = true;
    }
    
    uint32_t elapsed = timeMs - startTime;

    // Calculate centering
    int w1 = text1.GetTextSize().width;
    int w2 = text2.GetTextSize().width;
    int gap = 8;
    int totalW = w1 + gap + w2;
    int startX = (DISPLAY_WIDTH - totalW) / 2;
    
    int t1_x_end = startX;
    int t2_x_end = startX + totalW; // Right aligned text
    
    int t1_x_start = -w1 - 10;
    int t2_x_start = DISPLAY_WIDTH + w2 + 10;
    
    int t1_x = t1_x_end;
    int t2_x = t2_x_end;
    
    // Initialize tweens on first render
    if (elapsed == 0) {
        text1Tween = tweeny::from(t1_x_start, 15)
            .to(t1_x_end, 15)
            .during(1000)
            .via(easing::cubicOut, easing::linear);
        text2Tween = tweeny::from(t2_x_start, 15)
            .to(t2_x_end, 15)
            .during(1000)
            .via(easing::cubicOut, easing::linear);
    }
    
    if (elapsed < 1000) {
        auto t1 = text1Tween.step((int)elapsed);
        auto t2 = text2Tween.step((int)elapsed);
        t1_x = t1[0];
        t2_x = t2[0];
    }
    
    maskCanvas.Clear(Color(0,0,0,0));
    text1.DrawOpaque(maskCanvas, Point(t1_x, 15), Color(255,255,255));
    text2.DrawOpaque(maskCanvas, Point(t2_x, 15), Color(255,255,255));
    
    float timesec = timeMs / 1000.0f;
    for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
        for (int x = 0; x < DISPLAY_WIDTH; ++x) {
            Color m = maskCanvas.GetPixel(x, y);
            if (m.r > 0) {
                 float u = x / (float)DISPLAY_WIDTH;
                 float v = y / (float)DISPLAY_HEIGHT;
                 Color fire = FireShader(u, v, timesec);
                 canvas.SetPixel(x, y, fire);
                 
                 if (elapsed > 500 && (rand() % 100) < 5) {
                     SimpleParticle p;
                     p.pos = glm::vec2(x, y);
                     p.vel = glm::vec2(
                         ((rand() % 100) / 100.0f - 0.5f) * 0.5f,
                         -0.2f - ((rand() % 100) / 100.0f) * 0.2f
                     );
                     p.life = 1.0f;
                     p.color = fire;
                     particles.push_back(p);
                 }
            }
        }
    }
    
    for (auto it = particles.begin(); it != particles.end(); ) {
        it->pos += it->vel;
        it->life -= 0.05f;
        
        if (it->life <= 0) {
            it = particles.erase(it);
        } else {
            Color c = it->color;
            if (it->life < 0.5f) c.ModulateA((byte)(it->life * 2.0f * 255));
            
            canvas.BlendPixel((int)it->pos.x, (int)it->pos.y, c);
            ++it;
        }
    }
}
