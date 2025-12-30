#include <utility>
#include <cassert>
#include <math.h>
#include "utils/Tools.h"
#include "platform/X11Graphics.h"
#include "utils/Configuration.h"

#define WINDOW_BORDER			10
#define DOT_SIZE				4
#define DOT_LESS_BRIGHTNESS		180
#define DOT_DARK_BRIGHTNESS		100

X11Graphics::X11Graphics(const Configuration& cfg) :
	display(nullptr),
	screen(0),
	window(0),
	img(nullptr),
	imgdata(nullptr)
{
	unsigned long black, white;

	std::cout << "Initializing X11 graphics..." << std::endl;

    XSetIOErrorHandler([](Display*) -> int {
        throw std::runtime_error("X11 IO Error: Connection broken");
    });

	display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Failed to open X display. Ensure X11 is running or DISPLAY is set." << std::endl;
        throw std::runtime_error("Failed to open X display");
    }
	screen = DefaultScreen(display);
	black = BlackPixel(display, screen);
	white = WhitePixel(display, screen);

	// Setup window and GC
	window = XCreateSimpleWindow(display, DefaultRootWindow(display), 20, 20,
		DISPLAY_WIDTH * DOT_SIZE + WINDOW_BORDER * 2, DISPLAY_HEIGHT * DOT_SIZE + WINDOW_BORDER * 2, WINDOW_BORDER, white, black);
	XSetStandardProperties(display, window, "LibLED", "HI!", None, NULL, 0, NULL);
	XSelectInput(display, window, ExposureMask|ButtonPressMask|KeyPressMask);
	gc = XCreateGC(display, window, 0, 0);
	XSetBackground(display, gc, white);
	XSetForeground(display, gc, black);
	XClearWindow(display, window);
	XMapRaised(display, window);

	// Setup image which we'll use to present
	imgdata = (char*)malloc(DISPLAY_WIDTH * DOT_SIZE * DISPLAY_HEIGHT * DOT_SIZE * 32);
	img = XCreateImage(display, DefaultVisual(display, screen), 24, ZPixmap, 0, imgdata, DISPLAY_WIDTH * DOT_SIZE, DISPLAY_HEIGHT * DOT_SIZE, 32, 0);
	ENSURE(img != nullptr);
	XInitImage(img);
	for(int y = 0; y < DISPLAY_HEIGHT * DOT_SIZE; y++)
	{
		for(int x = 0; x < DISPLAY_WIDTH * DOT_SIZE; x++)
		{
			XPutPixel(img, x, y, 0);
		}
	}
}

X11Graphics::~X11Graphics()
{
	XDestroyImage(img);
	img = nullptr;
	imgdata = nullptr;
	XDestroyWindow(display, window);
	XFreeGC(display, gc);
	XCloseDisplay(display);
	display = nullptr;
}

void X11Graphics::Present(Canvas& sourcecanvas)
{
	const Color* p = sourcecanvas.GetBuffer();
	for(int y = 0; y < DISPLAY_HEIGHT; y++)
	{
		for(int x = 0; x < DISPLAY_WIDTH; x++)
		{
			int ox = x * DOT_SIZE;
			int oy = y * DOT_SIZE;

			// Full bright color
			unsigned long x1 = p->b | (p->g << 8) | (p->r << 16);

			// Less bright color
			Color lessp = *p;
			lessp.ModulateRGB(DOT_LESS_BRIGHTNESS);
			unsigned long x2 = lessp.b | (lessp.g << 8) | (lessp.r << 16);

			// Half bright color
			Color darkp = *p;
			darkp.ModulateRGB(DOT_DARK_BRIGHTNESS);
			unsigned long x3 = darkp.b | (darkp.g << 8) | (darkp.r << 16);

			// Draw a 4x4 'dot' which looks like a LED from a dot matrix display
			XPutPixel(img, ox + 1, oy + 0, x2);
			XPutPixel(img, ox + 2, oy + 0, x2);
			XPutPixel(img, ox + 1, oy + 3, x2);
			XPutPixel(img, ox + 2, oy + 3, x2);
			XPutPixel(img, ox + 0, oy + 1, x2);
			XPutPixel(img, ox + 0, oy + 2, x2);
			XPutPixel(img, ox + 3, oy + 1, x2);
			XPutPixel(img, ox + 3, oy + 2, x2);
			XPutPixel(img, ox + 1, oy + 1, x1);
			XPutPixel(img, ox + 1, oy + 2, x1);
			XPutPixel(img, ox + 2, oy + 1, x1);
			XPutPixel(img, ox + 2, oy + 2, x1);
			XPutPixel(img, ox + 0, oy + 0, x3);
			XPutPixel(img, ox + 0, oy + 3, x3);
			XPutPixel(img, ox + 3, oy + 0, x3);
			XPutPixel(img, ox + 3, oy + 3, x3);
			p++;
		}
	}

    XPutImage(display, window, gc, img, 0, 0, WINDOW_BORDER, WINDOW_BORDER, DISPLAY_WIDTH * DOT_SIZE, DISPLAY_HEIGHT * DOT_SIZE);
}

void X11Graphics::SetBrightness(int b)
{
	// Not supported
}

int X11Graphics::GetBrightness() const
{
	return 100;
}

int X11Graphics::GetKeyPress()
{
    // Process all pending events
    XEvent event;
    int key = 0;
    while(XCheckMaskEvent(display, KeyPressMask | ExposureMask | ButtonPressMask, &event)) {
         if (event.type == KeyPress) {
             key = XLookupKeysym(&event.xkey, 0);
         }
    }
    return key;
}
