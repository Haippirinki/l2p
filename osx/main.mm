#include <AppKit/AppKit.h>
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>

#include <common/Application.h>

bool g_shouldExit = false;

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

@end

int main(int argc, char** argv)
{
	[NSApplication sharedApplication];
	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
	[NSApp finishLaunching];

	NSRect contentRect = NSMakeRect(0, 0, 1280, 720);
	int styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
	NSWindow* window = [[NSWindow alloc] initWithContentRect:contentRect styleMask:styleMask backing:NSBackingStoreBuffered defer:NO];
	[window makeKeyAndOrderFront:nil];

	[NSApp activateIgnoringOtherApps:YES];

	Application* application = new Application;

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

	while(!g_shouldExit)
	{
		NSRect contentRect = [window contentRectForFrameRect:[window frame]];
		application->update((int)contentRect.size.width, (int)contentRect.size.height);

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
