// Includes
#include "ExcellentDemo.h"
#include "HotShotEffect.h"
#include "ImpressiveDemo.h"
#include "Resources.h"
#include "Shaders.h"
#include <X11/keysym.h>
#include <core/Audio.h>
#include <core/Graphics.h>
#include <effects/AnimationEffect.h>
#include <effects/CoreEffects.h>
#include <effects/MotionEffects.h>
#include <effects/OpticalEffects.h>
#include <effects/ParticleEffects.h>
#include <effects/PixelShaderEffect.h>
#include <effects/TextEffects.h>
#include <effects/TransitionEffects.h>
#include <functional>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <utils/Configuration.h>
#include <utils/Font.h>
#include <utils/ShaderUtils.h>
#include <utils/Tools.h>
#include <vector>

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
  TextShineDemo(const Font &f)
      : text(f, HorizontalAlign::Center, VerticalAlign::Middle) {
    text.SetText("SHINE!");
    pos = Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2);
    shine.Begin(text.GetTextSize(), 1500, easing::enumerated::cubicInOut);
  }

  void Render(Canvas &canvas, uint32_t timeMs) override {
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
  TextScaleDemo(const Font &f)
      : text(f, HorizontalAlign::Center, VerticalAlign::Middle) {
    text.SetText("SCALE ME");
    pos = Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2);
    scale.SetPosition(pos);
    // Start scaling up (10% to 100%)
    scale.Begin(text, nullptr, 0.1f, 1.0f, 2000, 1000,
                easing::enumerated::elasticOut);
  }

  void Render(Canvas &canvas, uint32_t timeMs) override {
    if (scale.IsFinished()) {
      // Loop: just restart same animation
      scale.Begin(text, nullptr, 0.1f, 1.0f, 2000, 1000,
                  easing::enumerated::elasticOut);
    }
    scale.Render(canvas, timeMs);
  }
};

// Star Parallax Effect for Grid Composition
class StarParallaxEffect : public IEffect {
  struct Star {
    float x, y;
    float speed;
    float brightness;
    int size;
  };

  std::vector<Star> stars;
  uint32_t lastUpdate;
  int bandHeight;
  int screenWidth, screenHeight;

public:
  StarParallaxEffect(int count, int width, int height, int visibleBandHeight)
      : lastUpdate(0), bandHeight(visibleBandHeight), screenWidth(width),
        screenHeight(height) {
    stars.resize(count);
    int centerY = height / 2;
    int halfBand = bandHeight / 2;

    for (auto &star : stars) {
      InitStar(star, true); // Random x
    }
  }

  void InitStar(Star &star, bool randomX) {
    star.x = randomX ? (float)(rand() % screenWidth)
                     : -5.0f; // Start off-screen left if not random

    // Random Y within center band
    int centerY = screenHeight / 2;
    int halfBand = bandHeight / 2;
    star.y = (float)(centerY - halfBand + (rand() % bandHeight));

    // Parallax: Faster stars are brighter and sometimes larger
    float speedFactor = (float)(rand() % 100) / 100.0f; // 0.0 to 1.0
    star.speed = 10.0f + speedFactor * 40.0f;           // 10 to 50 pixels/sec

    star.brightness = 0.3f + speedFactor * 0.7f; // Brighter if faster
    star.size = (speedFactor > 0.8f && (rand() % 5 == 0))
                    ? 2
                    : 1; // Occasional large star
  }

  void Render(Canvas &canvas, uint32_t timeMs) override {
    if (lastUpdate == 0) {
      lastUpdate = timeMs;
      return;
    }

    float dt = (timeMs - lastUpdate) / 1000.0f;
    lastUpdate = timeMs;

    for (auto &star : stars) {
      // Update position (Left to Right)
      star.x += star.speed * dt;

      // Wrap around
      if (star.x > screenWidth) {
        InitStar(star, false);
      }

      // Draw
      int ix = (int)star.x;
      int iy = (int)star.y;

      byte b = (byte)(star.brightness * 255.0f);
      Color c(b, b, b);

      if (star.size == 1) {
        canvas.SetPixel(ix, iy, c);
      } else {
        // Draw small cross or 2x2 for larger stars
        canvas.SetPixel(ix, iy, c);
        canvas.SetPixel(ix + 1, iy, c);
        canvas.SetPixel(ix, iy + 1, c);
        canvas.SetPixel(ix + 1, iy + 1, c);
      }
    }
  }
};

// Simple Effect to display a static image
class ImageEffect : public IEffect {
private:
  const Image &image;

public:
  ImageEffect(const Image &img) : image(img) {}

  void Render(Canvas &canvas, uint32_t timeMs) override {
    canvas.DrawColorImage(Point(0, 0), image);
  }
};

// Custom effect for MegaJoule demo where text approaches from far with opacity
class TextApproachEffect : public IEffect {
private:
  Text text;
  Color color;
  Canvas tempCanvas;

  TweenInt scaleTween;
  TweenInt opacityTween;

  TimePoint lastTime;
  bool active;
  bool finished;
  Point position;

public:
  TextApproachEffect(const Text &t, Color c)
      : text(t), color(c), active(false), finished(false) {
    position = Point(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2);
  }

  void Start() {
    // From 5% to 100% scale
    // From 0 to 204 opacity (80%)
    Size size = text.GetTextSize();
    int targetW = size.width;
    int startW = targetW / 20; // 5%

    scaleTween = tweeny::from(startW).to(targetW).during(3000).via(
        easing::enumerated::cubicOut);
    opacityTween =
        tweeny::from(0).to(204).during(3000).via(easing::enumerated::cubicOut);

    lastTime = Clock::now();
    active = true;
    finished = false;
  }

  virtual void Reset() override {
    active = false;
    finished = false;
  }

  void Render(Canvas &canvas, uint32_t timeMs) override {
    // Auto-start on first render if not active/finished
    if (!active && !finished) {
      Start();
    }

    int currentW;
    int currentOpacity;

    if (!active) {
      // If finished, draw final state
      if (finished) {
        currentW = text.GetTextSize().width;
        currentOpacity = 204;
      } else {
        return;
      }
    } else {
      TimePoint now = Clock::now();
      int dt = static_cast<int>(ch::ToMilliseconds(now - lastTime));
      lastTime = now;

      currentW = scaleTween.step(dt);
      currentOpacity = opacityTween.step(dt);

      if (scaleTween.progress() >= 1.0f) {
        active = false;
        finished = true;
        currentW = text.GetTextSize().width;
        currentOpacity = 204;
      }
    }

    if (currentW <= 0)
      return;

    // Draw text to temp canvas at full size
    tempCanvas.Clear(Color(0, 0, 0, 0));
    // Draw mask (WHITE)
    text.DrawMask(tempCanvas, position, WHITE);

    // Get the bounding box of the text in the temp canvas
    Rect srcRect = text.GetTextRect(position);
    if (srcRect.width <= 0 || srcRect.height <= 0)
      return;

    // Calculate scaled height maintaining aspect ratio based on Source Rect
    int textHeight = static_cast<int>(
        (static_cast<float>(currentW) / static_cast<float>(srcRect.width)) *
        static_cast<float>(srcRect.height));

    Point offset((canvas.Width() - currentW) / 2,
                 (canvas.Height() - textHeight) / 2);

    if ((currentW > 0) && (textHeight > 0)) {
      // Nearest-neighbor scaling
      // Map 0..currentW to srcRect.x..srcRect.Right
      int x_ratio = ((srcRect.width << 16) / currentW) + 1;
      int y_ratio = ((srcRect.height << 16) / textHeight) + 1;

      for (int i = 0; i < textHeight; i++) {
        for (int j = 0; j < currentW; j++) {
          int x2 = srcRect.x + ((j * x_ratio) >> 16);
          int y2 = srcRect.y + ((i * y_ratio) >> 16);

          if (offset.x + j >= 0 && offset.x + j < canvas.Width() &&
              offset.y + i >= 0 && offset.y + i < canvas.Height()) {

            // Check bounds for tempCanvas access too just in case
            if (x2 >= 0 && x2 < tempCanvas.Width() && y2 >= 0 &&
                y2 < tempCanvas.Height()) {
              Color c = tempCanvas.GetPixel(x2, y2);
              if (c.a > 0) {
                // Apply global opacity
                int finalAlpha = (c.a * currentOpacity) / 255;
                Color drawColor = color;
                drawColor.a = static_cast<byte>(finalAlpha);
                canvas.BlendPixel(offset.x + j, offset.y + i, drawColor);
              }
            }
          }
        }
      }
    }
  }
};

// Port of UnstoppableAnimation from Pandemic

// Port of UnstoppableAnimation from Pandemic
class UnstoppableEffect : public IEffect {
private:
  static constexpr int FADE_ALPHA = 255;
  static constexpr int FADE_FADE = 2;
  static constexpr int BOLT_TIME1 = 400;
  static constexpr int BOLT_TIME2 = 820;
  static constexpr int BOLT_DURATION = 120; // ms

  Text text;
  Canvas tempcanvas;
  const Image &texture;
  const Image &bolt1;
  const Image &bolt2;
  const Image &distort1;
  const Image &electcolors;

  TweenInt textprogress;
  TweenFloat fadeprogress;
  TweenInt flashprogress;

  TimePoint starttime;
  TimePoint laststeptime;
  TimePoint bolttime1;
  TimePoint bolttime2;
  TimePoint bolttimeoff;

  bool started;

public:
  UnstoppableEffect()
      : text("UNSTOPPABLE",
             Resources::GetResources().GetFont("boldbitslarge.fnt"),
             HorizontalAlign::Left, VerticalAlign::Middle, 1),
        texture(Resources::GetResources().GetImage("gray22d.dds")),
        bolt1(Resources::GetResources().GetImage("bolt1.dds")),
        bolt2(Resources::GetResources().GetImage("bolt2.dds")),
        distort1(Resources::GetResources().GetImage("distort1.dds")),
        electcolors(Resources::GetResources().GetImage("electcolors.dds")),
        started(false) {}

  void Start() {
    Resources::GetResources().GetSound("unstoppable.wav").Play();
    starttime = Clock::now();
    laststeptime = starttime;
    // Note: Wait times and durations are in ms
    textprogress = tweeny::from(100)
                       .wait(BOLT_TIME1)
                       .to(-text.GetTextSize().width - 5)
                       .during(2000);
    fadeprogress =
        tweeny::from(0.0f).to(1.0f).during(400).via(easing::quadraticOut);

    // Flash progress: 0 -> 255 -> 0 ...
    flashprogress = tweeny::from(0)
                        .wait(400)
                        .to(255)
                        .during(1)
                        .to(0)
                        .during(200)
                        .via(easing::exponentialOut)
                        .wait(100)
                        .to(255)
                        .during(1)
                        .to(100)
                        .during(120)
                        .to(255)
                        .during(1)
                        .to(100)
                        .during(120)
                        .to(255)
                        .during(1)
                        .to(0)
                        .during(200)
                        .via(easing::exponentialOut);

    bolttime1 = starttime + ch::milliseconds(BOLT_TIME1);
    bolttime2 = starttime + ch::milliseconds(BOLT_TIME2);
    bolttimeoff = TimePoint();
    started = true;
  }

  void Render(Canvas &canvas, uint32_t timeMs) override {
    if (!started) {
      Start();
    }

    TimePoint t = Clock::now();

    // Check if animation ended
    if (!ch::IsTimeSet(starttime) ||
        (t > (starttime + ch::milliseconds(textprogress.duration())))) {
      // Animation ended, maybe restart? Or just stop.
      // For demo loop, we might want to restart or just stay idle.
      // If we want to loop, we can reset starttime.
      // Let's just return for now, effectively freezing or showing last
      // frame? Actually code sets starttime to TimePoint() which resets it.
      // But if we return, nothing is drawn?
      // Adaptation: If finished, let's just restart for demo purposes after a
      // delay? Or just keep started=false and let the scene loop handle it?
      // If I return here without drawing, screen might be black or previous
      // frame. Let's restart immediately for now to keep it active.
      Start();
      t = Clock::now(); // Update t
    }

    // Advance time
    int dt = static_cast<int>(ch::ToMilliseconds(t - laststeptime));
    laststeptime = t;
    int textpos = textprogress.step(dt);
    float backfade = fadeprogress.step(dt);
    byte flashalpha = static_cast<byte>(flashprogress.step(dt));

    tempcanvas.Clear(Color(0, 0, 0, 0));
    if (t > bolttime1) {
      // Draw text on temp canvas
      text.DrawOutlineMask(tempcanvas, Point(textpos, 15), 2, BLACK);
      text.DrawTexturedMask(tempcanvas, Point(textpos, 15), texture);

      // Process pixels for electric effect
      int zt = static_cast<int>(
          ch::ToMilliseconds(t - starttime - ch::milliseconds(BOLT_TIME1)) %
          512);
      if (zt > 255)
        zt = 255 - (zt - 255);
      float colorswidthf = static_cast<float>(electcolors.Width() - 1);

      // OPTIMIZATION: Loop only through relevant area if possible, but for
      // distortion we traverse display However, tempcanvas is full size? Yes.
      for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
          int dw = distort1.Width();
          int dh = distort1.Height();
          if (dw == 0 || dh == 0)
            continue; // Safety

          int dx = (-textpos + x) % dw;
          if (dx < 0)
            dx += dw;
          int dy = y % dh;
          int zoffset = distort1.GetByte(dx, dy);
          float epos =
              std::clamp(static_cast<float>(zoffset - zt), -20.0f, 20.0f) /
                  40.0f +
              0.5f;
          Color ecolor = electcolors.GetColor(
              static_cast<int>(
                  std::clamp(roundf(epos * colorswidthf), 0.0f, colorswidthf)),
              0);
          Color cc = tempcanvas.GetPixel(x, y);
          ecolor.a = cc.a;
          ecolor.Add(cc);
          tempcanvas.SetPixel(x, y, ecolor);
        }
      }
    }

    // Draw the fade and the flashes over the background
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
      for (int x = 0; x < DISPLAY_WIDTH; x++) {
        byte fa = static_cast<byte>(std::clamp(
            static_cast<int>(FADE_ALPHA * backfade) - FADE_FADE * y, 0, 255));
        canvas.BlendPixel(x, y, Color(BLACK, fa));
        canvas.BlendPixel(x, y, Color(WHITE, flashalpha));
      }
    }

    // Draw temp canvas to final canvas
    canvas.DrawColorImageBlend(Point(0, 0), tempcanvas);

    // Draw the lightning bolts
    if (ch::IsTimeSet(bolttime1) && (t > bolttime1) &&
        (!ch::IsTimeSet(bolttimeoff) || (t < bolttimeoff))) {
      bolttimeoff = bolttime1 + ch::milliseconds(BOLT_DURATION);
      canvas.DrawColorImageAdd(Point(60, 0), bolt2);
    } else if (ch::IsTimeSet(bolttime1) && ch::IsTimeSet(bolttimeoff) &&
               (t > bolttimeoff)) {
      bolttime1 = TimePoint();
      bolttimeoff = TimePoint();
    } else if (ch::IsTimeSet(bolttime2) && (t > bolttime2) &&
               (!ch::IsTimeSet(bolttimeoff) || (t < bolttimeoff))) {
      bolttimeoff = bolttime2 + ch::milliseconds(BOLT_DURATION);
      canvas.DrawColorImageAdd(Point(8, 0), bolt1);
    }
  }
};

// --- Scene Setup Functions ---

void AddBasicScenes(std::vector<Scene> &scenes, const Resources &resources) {
  scenes.push_back({"Plasma", std::make_shared<PlasmaEffect>()});
  scenes.push_back({"Vertical Gradient", std::make_shared<GradientEffect>(
                                             Color(255, 0, 0), Color(0, 0, 255),
                                             GradientType::LinearVertical)});

  // Test PNG Scene
  if (resources.HasImage("test.png")) {
    const Image &img = resources.GetImage("test.png");
    scenes.push_back({"Test PNG", std::make_shared<ImageEffect>(img)});
  }
}

void AddParticleScenes(std::vector<Scene> &scenes,
                       const Image *fallingPixelSource = nullptr) {
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
    auto fallingEffect =
        std::make_shared<FallingPixelsEffect>(*fallingPixelSource);
    fallingEffect->SetSpeed(2.0f);
    fallingEffect->SetRate(50.0f);
    scenes.push_back({"Falling Pacman", fallingEffect});
  }
}

void AddTextScenes(std::vector<Scene> &scenes, const Font &font, int width,
                   int height) {
  Text text(font, HorizontalAlign::Center, VerticalAlign::Middle);
  text.SetText("HELLO!");

  // Typewriter
  auto typewriter =
      std::make_shared<TypewriterEffect>(text, Color(0, 255, 0), 200);
  typewriter->SetPosition(Point(width / 2, height / 2));
  scenes.push_back({"Typewriter", typewriter});

  // Shine
  scenes.push_back({"Text Shine", std::make_shared<TextShineDemo>(font)});

  // Flash
  Text flashText(font, HorizontalAlign::Center, VerticalAlign::Middle);
  flashText.SetText("FLASH!");
  auto textFlasher = std::make_shared<TextFlashEffect>(
      flashText, Point(width / 2, height / 2), 1000);
  scenes.push_back({"Text Flash", textFlasher});

  // Shadow
  Text shadowText(font, HorizontalAlign::Center, VerticalAlign::Middle);
  shadowText.SetText("SHADOW");
  auto shadowEffect =
      std::make_shared<ShadowTextEffect>(shadowText, Color(255, 215, 0));
  shadowEffect->SetPosition(Point(width / 2, height / 2));
  scenes.push_back({"Shadow", shadowEffect});

  // Bounce
  Text bounceText(font, HorizontalAlign::Center, VerticalAlign::Middle);
  bounceText.SetText("BOUNCE");
  auto bounceEffect =
      std::make_shared<TextBounceEffect>(bounceText, Color(255, 215, 0));
  bounceEffect->SetPosition(Point(width / 2, height / 2));
  scenes.push_back({"BOUNCE", bounceEffect});

  // Scale
  scenes.push_back({"Text Scale", std::make_shared<TextScaleDemo>(font)});

  // Shaky text
  Text shakyText(font, HorizontalAlign::Center, VerticalAlign::Middle);
  shakyText.SetText("SHAKY");
  auto shakyTextEffect =
      std::make_shared<TextEffect>(shakyText, Color(255, 215, 0));
  shakyTextEffect->SetPosition(Point(width / 2, height / 2));
  scenes.push_back(
      {"Shaky text", std::make_shared<ShakeEffect>(shakyTextEffect, 1)});

  // Scroll text
  Text scrollText(font, HorizontalAlign::Center, VerticalAlign::Middle);
  scrollText.SetText("SCROLL");
  auto scrollEffect =
      std::make_shared<TextScrollEffect>(scrollText, Color(255, 215, 0));
  scrollEffect->SetY(height / 2);
  scenes.push_back({"Scroll text", scrollEffect});
}

void AddShaderScenes(std::vector<Scene> &scenes,
                     const std::shared_ptr<IEffect> &sourceForEffects) {
  scenes.push_back(
      {"Sine Ripple", std::make_shared<PixelShaderEffect>(SineShader)});
  scenes.push_back({"Fire", std::make_shared<PixelShaderEffect>(FireShader)});
  scenes.push_back(
      {"Voronoi", std::make_shared<PixelShaderEffect>(VoronoiShader)});

  // Grid + Stars Composite
  auto grid = std::make_shared<PixelShaderEffect>(GridShader);
  auto stars = std::make_shared<StarParallaxEffect>(
      40, DISPLAY_WIDTH, DISPLAY_HEIGHT, 14); // 40 stars, 14px band
  auto bg = std::make_shared<CompositeEffect>(grid, stars);
  scenes.push_back({"Grid", bg});

  // MegaJoule Scene (background + text)
  // Text
  Text mjText(Resources::GetResources().GetFont("boldbitslarge.fnt"),
              HorizontalAlign::Center, VerticalAlign::Middle);
  mjText.SetText("MEGAJOULE");
  auto mjEffect = std::make_shared<TextApproachEffect>(
      mjText, Color(0, 215, 215)); // Gold color

  scenes.push_back(
      {"MegaJoule", std::make_shared<CompositeEffect>(bg, mjEffect)});

  // Flash Effect (on source)
  if (sourceForEffects) {
    scenes.push_back({"Flashing Plasma",
                      std::make_shared<FlashEffect>(sourceForEffects, 2000)});
  }
}

void AddMotionScenes(std::vector<Scene> &scenes,
                     const std::shared_ptr<IEffect> &source) {
  if (!source)
    return;

  // Scroll
  auto scroll = std::make_shared<ScrollEffect>(source, 20, 10);
  scenes.push_back({"Scroll Effect", scroll});

  // Jitter
  auto jitter = std::make_shared<JitterEffect>(source);
  scenes.push_back({"Jitter Effect", jitter});
}

void AddOpticalScenes(std::vector<Scene> &scenes,
                      const std::shared_ptr<IEffect> &source) {
  if (!source)
    return;

  // Blur
  auto blur = std::make_shared<BlurEffect>(source);
  scenes.push_back({"Blur Effect", blur});
}

// Returns loaded pacman gif for reuse if needed, or nullptr
std::shared_ptr<AnimatedImage> AddAnimationScenes(std::vector<Scene> &scenes,
                                                  const Resources &resources,
                                                  const Font &font) {
  // Legacy Demos
  const Image &yellowTexture = resources.GetImage("yellow12s.dds");
  scenes.push_back(
      {"Impressive", std::make_shared<ImpressiveDemo>(font, yellowTexture)});

  const Image &burnMap = resources.GetImage("burnmap.dds");
  const Image &burnTex = resources.GetImage("burntex.dds");
  const Image &burnColors = resources.GetImage("burncolors.dds");
  scenes.push_back({"Excellent", std::make_shared<ExcellentDemo>(
                                     font, burnMap, burnTex, burnColors)});

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
  scenes.push_back({"Pacman + Sparks",
                    std::make_shared<CompositeEffect>(bgSparks, pacmanEffect)});

  auto transitionGif = resources.GetGif("transition.gif");
  transitionGif->SetTransparentColor(Color(0, 0, 0));
  auto transEffect = std::make_shared<AnimationEffect>(transitionGif);
  scenes.push_back({"Transition GIF",
                    std::make_shared<CompositeEffect>(bgSparks, transEffect)});

  auto cubeGif = resources.GetGif("cube_rotate.gif");
  cubeGif->SetTransparentColor(Color(0, 0, 0));
  auto cubeEffect = std::make_shared<AnimationEffect>(cubeGif);
  scenes.push_back(
      {"Cube GIF", std::make_shared<CompositeEffect>(bgSparks, cubeEffect)});

  return pacman;
}

void AddTransitionScenes(std::vector<Scene> &scenes) {
  // Ensure we have enough scenes
  if (scenes.size() < 4)
    return;

  // Pick some source/dest scenes
  auto s0 = scenes[0].effect; // Plasma
  auto s1 = scenes[1].effect; // Gradient probably
  auto s2 = scenes[2].effect;

  scenes.push_back(
      {"Dissolve Trans", std::make_shared<DissolveEffect>(s0, s1, 3000)});
  scenes.push_back(
      {"Screen Melt", std::make_shared<MeltTransitionEffect>(s0, s1, 3000)});
  scenes.push_back(
      {"Slide Transition", std::make_shared<SlideTransitionEffect>(
                               s0, s1, 2000, TransitionDirection::Left)});
  scenes.push_back(
      {"Wipe Transition", std::make_shared<WipeTransitionEffect>(
                              s1, s2, 2000, TransitionDirection::Down)});
  scenes.push_back({"Zoom Transition",
                    std::make_shared<ZoomTransitionEffect>(s2, s0, 2000)});
}

// --- Main ---

int main(int argc, char *argv[]) {
  std::cout << "Starting Demo App..." << std::endl;
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    std::cout << "Current working directory: " << cwd << std::endl;
  }

  Configuration config;
  Audio audio(config);
  Graphics graphics(config, true);
  Canvas &canvas = graphics.GetCanvas();
  Resources resources(config, graphics);
  const Font &defaultFont = resources.GetFont("boldbitslarge.fnt");

  // Try loading TTF
  Font *ttfFont = Font::LoadFromTTF("demo/assets/mleitod.ttf", 24.0f);
  const Font &font = (ttfFont) ? *ttfFont : defaultFont;
  if (ttfFont)
    std::cout << "Loaded TTF font!" << std::endl;
  else
    std::cout << "Failed to load TTF font, using default." << std::endl;

  std::vector<Scene> scenes;

  // 1. Basic
  AddBasicScenes(scenes, resources);

  // 2. Animations (Load these first to get Pacman image for particles)
  auto pacmanAnim = AddAnimationScenes(scenes, resources, font);
  const Image *pacmanFrame = (pacmanAnim && pacmanAnim->GetFrameCount() > 30)
                                 ? pacmanAnim->GetFrame(30)
                                 : nullptr;

  // 3. Particles
  AddParticleScenes(scenes, pacmanFrame);

  // 4. Text
  AddTextScenes(scenes, font, canvas.Width(), canvas.Height());

  // 5. Shaders
  // Unstoppable
  scenes.push_back({"Unstoppable", std::make_shared<UnstoppableEffect>()});

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

  std::cout << "Controls: LEFT/RIGHT to switch scenes. ESC to quit."
            << std::endl;
  if (!scenes.empty()) {
    std::cout << "Current Scene: " << scenes[currentScene].name << std::endl;
  }

  try {
    for (int i = 0;; ++i) {
      uint32_t timeMs = i * 25;

      // Input
      int key = graphics.GetKey();
      if (key != 0 && key != lastKey) {
        if (key == XK_Left) {
          currentScene--;
          if (currentScene < 0)
            currentScene = scenes.size() - 1;
          std::cout << "Switching to: " << scenes[currentScene].name
                    << std::endl;
          Resources::GetResources().GetSound("woosh.wav").Play();
        } else if (key == XK_Right) {
          currentScene++;
          if (currentScene >= (int)scenes.size())
            currentScene = 0;
          std::cout << "Switching to: " << scenes[currentScene].name
                    << std::endl;
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
  } catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
