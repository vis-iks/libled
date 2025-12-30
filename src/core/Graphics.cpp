#include <utility>
#include <cassert>
#ifdef RPI
#include "platform/DotMatrixGraphics.h"
#endif
#ifdef X86
#include "platform/X11Graphics.h"
#include "platform/X11Graphics.h"
#endif
#include "platform/DummyGraphics.h"
#include <math.h>
#include "utils/Tools.h"
#include "core/Graphics.h"
#include "core/Canvas.h"
#include "utils/File.h"
#include "external/lodepng.h"

#include "core/GraphicsConstants.h"

Graphics::Graphics(const Configuration& cfg, bool showfps) :
	hal(nullptr),
	showfps(showfps),
	nextfpstime(Clock::now() + ch::seconds(10)),
	framescounted(0),
	frameindex(0)
{
	DISPLAY_WIDTH = cfg.GetInt("Display.Width", 128);
	DISPLAY_HEIGHT = cfg.GetInt("Display.Height", 32);
	DISPLAY_PANELS = cfg.GetInt("Display.Panels", 2);

    // Initial resize of the canvas to match the configuration
    canvas.Resize(DISPLAY_WIDTH, DISPLAY_HEIGHT);

	recordinterval = ch::microseconds(static_cast<int64_t>(std::roundl(1000000.0 / cfg.GetDouble("Graphics.RecordRate", 30))));

	// Choose the graphics implementation depending on the hardware it was built for.
	#ifdef RPI
		hal = new DotMatrixGraphics(cfg);
	#endif
	#ifdef X86
        try {
		    hal = new X11Graphics(cfg);
        } catch (...) {
            std::cerr << "Falling back to DummyGraphics (Headless Mode)" << std::endl;
            hal = new DummyGraphics(cfg);
        }
	#endif
}

Graphics::~Graphics()
{
	SAFE_DELETE(hal);
}

void Graphics::Record(String path)
{
	recordpath = path;
	nextrecordtime = Clock::now() + recordinterval;
}

void Graphics::ClearRenderers()
{
	renderers.clear();
}

void Graphics::AddRenderer(IRenderer* r)
{
	REQUIRE(r != nullptr);
	auto it = std::find(renderers.begin(), renderers.end(), r);
	if(it == renderers.end())
		renderers.push_back(r);
}

void Graphics::RemoveRenderer(IRenderer* r)
{
	REQUIRE(r != nullptr);
	auto it = std::find(renderers.begin(), renderers.end(), r);
	if(it != renderers.end())
		renderers.erase(it);
}

// This renders the canvas and displays it
void Graphics::Present(bool clear)
{
	// Clear the screen
	if (clear)
		canvas.Clear(BLACK);

	// Let the renderers draw their art
	for(IRenderer* r : renderers)
		r->Render(canvas);

	// Show the canvas on display
	hal->Present(canvas);

	// Measure FPS
	framescounted++;
	if(showfps && (Clock::now() >= nextfpstime))
	{
		std::cout << "FPS: " << (static_cast<float>(framescounted) / 10.0f) << std::endl;
		framescounted = 0;
		nextfpstime += ch::seconds(10);
	}

	// Record frames
	if(recordpath.Length() > 0)
	{
		TimePoint t = Clock::now();
		if(t >= nextrecordtime)
		{
			// Repeat the last frame if we skipped over too much time
			while(t >= (nextrecordtime + recordinterval))
			{
				WriteRecordedFrame();
				nextrecordtime += recordinterval;
			}

			// Record a frame
			recordbuffer.clear();
			lodepng::encode(recordbuffer, reinterpret_cast<const unsigned char*>(canvas.GetBuffer()), DISPLAY_WIDTH, DISPLAY_HEIGHT);
			WriteRecordedFrame();
			nextrecordtime += recordinterval;
		}
	}
}

void Graphics::WriteRecordedFrame()
{
	String frameindexstr = String::From(frameindex);
	String strpadding('0', 8 - frameindexstr.Length());
	String filename = File::CombinePath(recordpath, "Frame-" + strpadding + frameindexstr + ".png");
	lodepng::save_file(recordbuffer, filename.stl());
	frameindex++;
}
