#include "ExcellentDemo.h"
#include <cmath>
#include <algorithm>

ExcellentDemo::ExcellentDemo(const Font& f, const Image& bm, const Image& bt, const Image& bc)
    : font(f), burnMap(bm), burnTex(bt), burnColors(bc)
{
    text = Text(f, HorizontalAlign::Center, VerticalAlign::Middle);
    text.SetText("EXCELLENT");
    
    Start();
}

void ExcellentDemo::Start()
{
    startTime = Clock::now();
    lastStepTime = Clock::now();
    burnProgress = tweeny::from(1.0f + BURN_LENGTH).to(-BURN_LENGTH * 2.0f).during(2000).via(easing::quarticOut).wait(400);
}

void ExcellentDemo::Render(Canvas& canvas, uint32_t timeMs)
{
    TimePoint now = Clock::now();
    
    // Check if animation finished - restart it
    if (!ch::IsTimeSet(startTime) || (now > (startTime + ch::milliseconds(burnProgress.duration())))) {
        Start();
    }
    
    // Advance time
    int dt = static_cast<int>(ch::ToMilliseconds(now - lastStepTime));
    lastStepTime = now;
    float burnDist = burnProgress.step(dt);
    
    // Draw text to temp canvas
    tempCanvas.Clear(Color(0, 0, 0, 0));
    text.DrawOutlineMask(tempCanvas, Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), 2, BLACK);
    text.DrawMask(tempCanvas, Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), WHITE);
    
    // Apply burning effect - turn white pixels into glowy fire
    float timeSec = static_cast<float>(ch::ToMilliseconds(now - startTime)) / 3000.0f;
    float scaleX = 1.0f / static_cast<float>(DISPLAY_WIDTH * 0.5f);
    float scaleY = 1.0f / static_cast<float>(DISPLAY_HEIGHT);
    Size canvasSize = tempCanvas.GetSize();
    float burnColorsWidthF = static_cast<float>(burnColors.Width() - 1);
    
    for (int y = 0; y < canvasSize.height; y++) {
        for (int x = 0; x < canvasSize.width; x++) {
            Color tc = tempCanvas.GetPixel(x, y);
            if (tc.a > 0) {
                // Get burn map value and texture
                float bm = 1.0f - static_cast<float>(burnMap.GetByte(x, y)) / 255.0f;
                byte bt = burnTex.GetByte(x, y);
                
                // Calculate edge position and glow
                float edgePos = std::clamp((burnDist + bm) - 1.0f, -BURN_LENGTH, 0.0f) + BURN_LENGTH;
                float edgeGlow = edgePos / BURN_LENGTH;
                
                Color c;
                if (edgeGlow < 0.999f) {
                    // Get edge color from burn colors palette
                    Color edgeColor = burnColors.GetColor(
                        static_cast<int>(std::clamp(roundf(edgeGlow * burnColorsWidthF), 0.0f, burnColorsWidthF)), 
                        0
                    );
                    
                    // Apply fire shader
                    float u = static_cast<float>(x) * scaleX;
                    float v = static_cast<float>(y) * scaleY;
                    c = FireShader(u, v, timeSec);
                    
                    // Modulate with burn texture and original alpha
                    c.ModulateRGB(bt);
                    c.ModulateRGBA(tc);
                    c.Add(edgeColor);
                } else {
                    c = Color(0, 0, 0, 0);
                }
                
                tempCanvas.SetPixel(x, y, c);
            }
        }
    }
    
    canvas.DrawColorImageMask(Point(0, 0), tempCanvas);
}
