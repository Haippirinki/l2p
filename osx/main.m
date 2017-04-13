#include <AppKit/AppKit.h>
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>

int main(int argc, char** argv)
{
	[NSApplication sharedApplication];
	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
	[NSApp finishLaunching];

	NSRect contentRect = NSMakeRect(0, 0, 1280, 720);
	int styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
	NSWindow* window = [[NSWindow alloc] initWithContentRect:contentRect styleMask:styleMask backing:NSBackingStoreRetained defer:NO];
	[window makeKeyAndOrderFront:nil];

	[NSApp activateIgnoringOtherApps:YES];

	NSOpenGLPixelFormatAttribute attributes[] = {
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
		0
	};
	id pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
	id context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
	[context setView:[window contentView]];
	[context makeCurrentContext];

	int frame = 0;
	while(true)
	{
		while(true)
		{
			NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES];
			if(event)
			{
				[NSApp sendEvent:event];
			}
			else
			{
				break;
			}
		}

		float r = (float)frame / 300.f;
		glClearColor(r, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		[context flushBuffer];

		frame++;
	}

	return 0;
}
