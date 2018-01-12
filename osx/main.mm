#include <AppKit/AppKit.h>
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>

#include <common/Application.h>

#include <sys/time.h>

bool g_shouldExit = false;

static int64_t getMicroseconds()
{
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000L + tv.tv_usec;
}

@interface WindowDelegate : NSObject <NSWindowDelegate>
{
}
@end

@implementation WindowDelegate

- (void)windowWillClose:(NSNotification*)notification
{
	g_shouldExit = true;
}

- (void)windowDidResize:(NSNotification*)notification
{
	[[NSOpenGLContext currentContext] update];
}

- (void)windowDidMove:(NSNotification*)notification
{
	[[NSOpenGLContext currentContext] update];
}

@end

@interface WindowView : NSView
{
	Application* application;
}

- (WindowView*)initWithApplication:(Application*)aApplication;

@end

@implementation WindowView

- (WindowView*)initWithApplication:(Application*)aApplication
{
	self = [super init];
	[self setWantsBestResolutionOpenGLSurface:YES];
	application = aApplication;
	return self;
}

- (void)mouseDown:(NSEvent*)event
{
	NSPoint point = [event locationInWindow];
	application->mouseDown(point.x, self.bounds.size.height - point.y);
}

- (void)mouseUp:(NSEvent*)event
{
	NSPoint point = [event locationInWindow];
	application->mouseUp(point.x, self.bounds.size.height - point.y);
}

- (void)mouseDragged:(NSEvent*)event
{
	NSPoint point = [event locationInWindow];
	application->mouseMove(point.x, self.bounds.size.height - point.y);
}

@end

int main(int argc, char** argv)
{
	[NSApplication sharedApplication];
	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
	[NSApp finishLaunching];

	NSRect contentRect = NSMakeRect(0, 0, 1280, 720);
	int styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
	NSWindow* window = [[NSWindow alloc] initWithContentRect:contentRect styleMask:styleMask backing:NSBackingStoreBuffered defer:NO];
	[window setColorSpace: [NSColorSpace sRGBColorSpace]];
	[window makeKeyAndOrderFront:nil];

	[NSApp activateIgnoringOtherApps:YES];

	Application* application = createApplication();

	WindowDelegate* windowDelegate = [[WindowDelegate alloc] init];
	[window setDelegate:windowDelegate];

	WindowView* windowView = [[WindowView alloc] initWithApplication:application];
	[window setContentView:windowView];

	NSOpenGLPixelFormatAttribute attributes[] = {
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
		0
	};
	NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
	NSOpenGLContext* context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
	[pixelFormat release];
	[context setView:[window contentView]];
	[context makeCurrentContext];

	glEnable(GL_FRAMEBUFFER_SRGB);

	application->init();

	int64_t startTime = getMicroseconds();
	int64_t lastUpdateTime = startTime;

	while(!g_shouldExit)
	{
		int64_t time = getMicroseconds();

		NSRect contentRect = [window contentRectForFrameRect:[window frame]];
		application->update((int)contentRect.size.width, (int)contentRect.size.height, (time - startTime) * (1.0 / 1000000.0), (time - lastUpdateTime) * (1.0 / 1000000.0));

		lastUpdateTime = time;

		NSRect backingRect = [windowView convertRectToBacking:contentRect];
		application->render((int)backingRect.size.width, (int)backingRect.size.height);
		[context flushBuffer];

		while(!g_shouldExit)
		{
			NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES];
			if(event)
			{
				[NSApp sendEvent:event];
				[event release];
			}
			else
			{
				break;
			}
		}
	}

	delete application;

	[NSOpenGLContext clearCurrentContext];
	[context release];
	[windowView release];
	[windowDelegate release];
	[window release];

	return 0;
}
