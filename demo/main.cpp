// Includes
#include <iostream>
#include <unistd.h>
#include <vector>
#include <memory>
#include <functional>
#include <core/Graphics.h>
#include <utils/Configuration.h>
#include <effects/CoreEffects.h>
#include <effects/TextEffects.h>
#include <effects/ParticleEffects.h>
#include <effects/OpticalEffects.h>
#include <effects/MotionEffects.h>
#include <effects/AnimationEffect.h>
#include <effects/TransitionEffects.h>
#include <utils/Font.h>
#include "Resources.h"
#include <utils/Tools.h>
#include <utils/ShaderUtils.h>
#include "Shaders.h"
#include "HotShotEffect.h"
#include "ImpressiveDemo.h"
#include "ExcellentDemo.h"
#include <effects/PixelShaderEffect.h>
#include <effects/PixelShaderEffect.h>
#include <X11/keysym.h>
#include <core/Audio.h>

using namespace libled;

// --- Data Structures ---

struct Scene {
    String name;
    std::shared_ptr<IEffect> effect;
};

// --- Helper Effects ---

// Custom effect for Text Shine demo
class TextShineDemo : public IEffect {
private:
    Text text;
    TextShineEffect shine;
    Point pos;
public:
    TextShineDemo(const Font& f) : text(f, HorizontalAlign::Center, VerticalAlign::Middle) {
        text.SetText("SHINE!");
        pos = Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2);
        shine.Begin(text.GetTextSize(), 1500, easing::enumerated::cubicInOut);
    }
    
    void Render(Canvas& canvas, uint32_t timeMs) override {
        if (shine.IsFinished()) {
            shine.Begin(text.GetTextSize(), 1500, easing::enumerated::cubicInOut);
        }
        text.DrawBlend(canvas, pos, Color(255, 215, 0));
        shine.Draw(canvas, text, 0, pos);
    }
};

class TextScaleDemo : public IEffect {
private:
    Text text;
    TextScaleEffect scale;
    Point pos;
public:
    TextScaleDemo(const Font& f) : text(f, HorizontalAlign::Center, VerticalAlign::Middle) {
        text.SetText("SCALE ME");
        pos = Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2);
        scale.SetPosition(pos);
        // Start scaling up (10% to 100%)
        scale.Begin(text, nullptr, 0.1f, 1.0f, 2000, 1000, easing::enumerated::elasticOut);
    }
    
    void Render(Canvas& canvas, uint32_t timeMs) override {
        if (scale.IsFinished()) {
             // Loop: just restart same animation
             scale.Begin(text, nullptr, 0.1f, 1.0f, 2000, 1000, easing::enumerated::elasticOut);
        }
        scale.Render(canvas, timeMs);
    }
};

// --- Scene Setup Functions ---

void AddBasicScenes(std::vector<Scene>& scenes) {
    scenes.push_back({"Plasma", std::make_shared<PlasmaEffect>()});
    scenes.push_back({"Vertical Gradient", std::make_shared<GradientEffect>(Color(255,0,0), Color(0,0,255), GradientType::LinearVertical)});
}

void AddParticleScenes(std::vector<Scene>& scenes, const Image* fallingPixelSource = nullptr) {
    // Fire
    auto fire = std::make_shared<ParticleSystemEffect>();
    fire->SetBaseColor(Color(255, 100, 0));
    fire->SetGravity(false);
    scenes.push_back({"Fire Particles", fire});
    
    // Sparks
    auto sparks = std::make_shared<ParticleSystemEffect>();
    sparks->SetBaseColor(Color(100, 200, 255));
    sparks->SetGravity(true);
    scenes.push_back({"Sparks", sparks});
    
    // Fireworks & Comet
    scenes.push_back({"Fireworks", std::make_shared<FireworksEffect>()});
    scenes.push_back({"Comet", std::make_shared<CometEffect>()});
    
    // Falling Pixels (if image provided)
    if (fallingPixelSource) {
         auto fallingEffect = std::make_shared<FallingPixelsEffect>(*fallingPixelSource);
         fallingEffect->SetSpeed(2.0f);
         fallingEffect->SetRate(50.0f);
         scenes.push_back({"Falling Pacman", fallingEffect});
    }
}

void AddTextScenes(std::vector<Scene>& scenes, const Font& font, int width, int height) {
    Text text(font, HorizontalAlign::Center, VerticalAlign::Middle);
    text.SetText("HELLO!");
    
    // Typewriter
    auto typewriter = std::make_shared<TypewriterEffect>(text, Color(0, 255, 0), 200);
    typewriter->SetPosition(Point(width / 2, height / 2));
    scenes.push_back({"Typewriter", typewriter});
    
    // Shine
    scenes.push_back({"Text Shine", std::make_shared<TextShineDemo>(font)});
    
    // Flash
    Text flashText(font, HorizontalAlign::Center, VerticalAlign::Middle);
    flashText.SetText("FLASH!");
    auto textFlasher = std::make_shared<TextFlashEffect>(flashText, Point(width/2, height/2), 1000);
    scenes.push_back({"Text Flash", textFlasher});

    // Shadow
    Text shadowText(font, HorizontalAlign::Center, VerticalAlign::Middle);
    shadowText.SetText("SHADOW");
    auto shadowEffect = std::make_shared<ShadowTextEffect>(shadowText, Color(255, 215, 0));
    shadowEffect->SetPosition(Point(width / 2, height / 2));
    scenes.push_back({"Shadow", shadowEffect});
    
    // Bounce
    Text bounceText(font, HorizontalAlign::Center, VerticalAlign::Middle);
    bounceText.SetText("BOUNCE");
    auto bounceEffect = std::make_shared<TextBounceEffect>(bounceText, Color(255, 215, 0));
    bounceEffect->SetPosition(Point(width / 2, height / 2));
    scenes.push_back({"BOUNCE", bounceEffect});
    
    // Scale
    scenes.push_back({"Text Scale", std::make_shared<TextScaleDemo>(font)});

    // Shaky text
    Text shakyText(font, HorizontalAlign::Center, VerticalAlign::Middle);
    shakyText.SetText("SHAKY");
    auto shakyTextEffect = std::make_shared<TextEffect>(shakyText, Color(255, 215, 0));
    shakyTextEffect->SetPosition(Point(width / 2, height / 2));
    scenes.push_back({"Shaky text", std::make_shared<ShakeEffect>(shakyTextEffect, 1)});

    // Scroll text
    Text scrollText(font, HorizontalAlign::Center, VerticalAlign::Middle);
    scrollText.SetText("SCROLL");
    auto scrollEffect = std::make_shared<TextScrollEffect>(scrollText, Color(255, 215, 0));
    scrollEffect->SetY(height / 2);
    scenes.push_back({"Scroll text", scrollEffect});
}

void AddShaderScenes(std::vector<Scene>& scenes, const std::shared_ptr<IEffect>& sourceForEffects) {
    scenes.push_back({"Sine Ripple", std::make_shared<PixelShaderEffect>(SineShader)});
    scenes.push_back({"Fire", std::make_shared<PixelShaderEffect>(FireShader)});
    scenes.push_back({"Voronoi", std::make_shared<PixelShaderEffect>(VoronoiShader)});

    // Flash Effect (on source)
    if (sourceForEffects) {
        scenes.push_back({"Flashing Plasma", std::make_shared<FlashEffect>(sourceForEffects, 2000)});
    }
}

void AddMotionScenes(std::vector<Scene>& scenes, const std::shared_ptr<IEffect>& source) {
    if (!source) return;
    
    // Scroll
    auto scroll = std::make_shared<ScrollEffect>(source, 20, 10);
    scenes.push_back({"Scroll Effect", scroll});
    
    // Jitter
    auto jitter = std::make_shared<JitterEffect>(source);
    scenes.push_back({"Jitter Effect", jitter});
}

void AddOpticalScenes(std::vector<Scene>& scenes, const std::shared_ptr<IEffect>& source) {
    if (!source) return;
    
    // Blur
    auto blur = std::make_shared<BlurEffect>(source);
    scenes.push_back({"Blur Effect", blur});
}

// Returns loaded pacman gif for reuse if needed, or nullptr
std::shared_ptr<AnimatedImage> AddAnimationScenes(std::vector<Scene>& scenes, const Resources& resources, const Font& font) {
    // Legacy Demos
    const Image& yellowTexture = resources.GetImage("yellow12s.dds");
    scenes.push_back({"Impressive", std::make_shared<ImpressiveDemo>(font, yellowTexture)});
    
    const Image& burnMap = resources.GetImage("burnmap.dds");
    const Image& burnTex = resources.GetImage("burntex.dds");
    const Image& burnColors = resources.GetImage("burncolors.dds");
    scenes.push_back({"Excellent", std::make_shared<ExcellentDemo>(font, burnMap, burnTex, burnColors)});
    
    scenes.push_back({"HotShot", std::make_shared<HotShotEffect>(font)});
    
    // GIFs
    auto gif = resources.GetGif("test.gif");
    scenes.push_back({"GIF Animation", std::make_shared<AnimationEffect>(gif)});
    
    auto pacman = resources.GetGif("pacman.gif");
    pacman->SetTransparentColor(Color(0, 0, 0));
    
    auto bgSparks = std::make_shared<ParticleSystemEffect>();
    bgSparks->SetBaseColor(Color(100, 200, 255)); 
    bgSparks->SetGravity(true);
    
    auto pacmanEffect = std::make_shared<AnimationEffect>(pacman);
    scenes.push_back({"Pacman + Sparks", std::make_shared<CompositeEffect>(bgSparks, pacmanEffect)});
    
    auto transitionGif = resources.GetGif("transition.gif");
    transitionGif->SetTransparentColor(Color(0, 0, 0));
    auto transEffect = std::make_shared<AnimationEffect>(transitionGif);
    scenes.push_back({"Transition GIF", std::make_shared<CompositeEffect>(bgSparks, transEffect)});

    auto cubeGif = resources.GetGif("cube_rotate.gif");
    cubeGif->SetTransparentColor(Color(0, 0, 0));
    auto cubeEffect = std::make_shared<AnimationEffect>(cubeGif);
    scenes.push_back({"Cube GIF", std::make_shared<CompositeEffect>(bgSparks, cubeEffect)});
    
    return pacman;
}

void AddTransitionScenes(std::vector<Scene>& scenes) {
    // Ensure we have enough scenes
    if (scenes.size() < 4) return;
    
    // Pick some source/dest scenes
    auto s0 = scenes[0].effect; // Plasma
    auto s1 = scenes[1].effect; // Gradient probably
    auto s2 = scenes[2].effect; 
    
    scenes.push_back({"Dissolve Trans", std::make_shared<DissolveEffect>(s0, s1, 3000)});
    scenes.push_back({"Screen Melt", std::make_shared<MeltTransitionEffect>(s0, s1, 3000)});
    scenes.push_back({"Slide Transition", std::make_shared<SlideTransitionEffect>(s0, s1, 2000, TransitionDirection::Left)});
    scenes.push_back({"Wipe Transition", std::make_shared<WipeTransitionEffect>(s1, s2, 2000, TransitionDirection::Down)});
    scenes.push_back({"Zoom Transition", std::make_shared<ZoomTransitionEffect>(s2, s0, 2000)});
}

// --- Main ---

int main(int argc, char* argv[])
{
    std::cout << "Starting Demo App..." << std::endl;
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        std::cout << "Current working directory: " << cwd << std::endl;
    }
    
    Configuration config;
    Audio audio(config);
    Graphics graphics(config, true);
    Canvas& canvas = graphics.GetCanvas();
    Resources resources(config, graphics);
    const Font& font = resources.GetFont("boldbitslarge.fnt");
    
    std::vector<Scene> scenes;
    
    // 1. Basic
    AddBasicScenes(scenes);
    
    // 2. Animations (Load these first to get Pacman image for particles)
    auto pacmanAnim = AddAnimationScenes(scenes, resources, font);
    const Image* pacmanFrame = (pacmanAnim && pacmanAnim->GetFrameCount() > 30) ? pacmanAnim->GetFrame(30) : nullptr;

    // 3. Particles
    AddParticleScenes(scenes, pacmanFrame);
    
    // 4. Text
    AddTextScenes(scenes, font, canvas.Width(), canvas.Height());
    
    // 5. Shaders
    // Pass Plasma (scenes[0]) as source for effects like Flash
    auto plasma = scenes.empty() ? nullptr : scenes[0].effect;
    AddShaderScenes(scenes, plasma);
    AddMotionScenes(scenes, plasma);
    AddOpticalScenes(scenes, plasma);
    
    // 6. Transitions
    AddTransitionScenes(scenes);

    // --- Loop ---
    
    int currentScene = 0;
    int lastKey = 0;
    
    std::cout << "Controls: LEFT/RIGHT to switch scenes. ESC to quit." << std::endl;
    if (!scenes.empty()) {
        std::cout << "Current Scene: " << scenes[currentScene].name << std::endl;
    }

    try {
        for (int i = 0; ; ++i) {
            uint32_t timeMs = i * 25;
            
            // Input
            int key = graphics.GetKey();
            if (key != 0 && key != lastKey) {
                if (key == XK_Left) {
                    currentScene--;
                    if (currentScene < 0) currentScene = scenes.size() - 1;
                    std::cout << "Switching to: " << scenes[currentScene].name << std::endl;
                    Resources::GetResources().GetSound("woosh.wav").Play();
                } else if (key == XK_Right) {
                    currentScene++;
                    if (currentScene >= (int)scenes.size()) currentScene = 0;
                     std::cout << "Switching to: " << scenes[currentScene].name << std::endl;
                    Resources::GetResources().GetSound("woosh.wav").Play();
                } else if (key == XK_Escape) {
                    break;
                }
            }
            lastKey = key;
            
            // Render
            canvas.Clear(BLACK);
            if (!scenes.empty()) {
                scenes[currentScene].effect->Render(canvas, timeMs);
            }
            
            graphics.Present(false);
            usleep(25000); 
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    
    return 0;
}
