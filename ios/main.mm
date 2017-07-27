#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import <QuartzCore/QuartzCore.h>

#include "Application.h"

#include <unistd.h>

namespace Platform
{
	void init();
}

@interface AppDelegate : UIResponder <UIApplicationDelegate, GLKViewDelegate>
{
	Application* myApplication;
}
@property (strong, nonatomic) UIWindow* window;
@end

@implementation AppDelegate

- (void)glkView:(GLKView*)view drawInRect:(CGRect)rect
{
	myApplication->update((int)rect.size.width, (int)rect.size.height);
	myApplication->render((int)view.drawableWidth, (int)view.drawableHeight);
}

- (void)render:(CADisplayLink*)displayLink
{
	GLKView* view = [self.window.subviews objectAtIndex:0];
	if(view && [view isKindOfClass:[GLKView class]])
	{
		[view display];
	}
}

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
	self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	
	EAGLContext* context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
	GLKView* view = [[GLKView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	view.context = context;
	view.delegate = (id)self;
	view.enableSetNeedsDisplay = NO;
	view.drawableColorFormat = GLKViewDrawableColorFormatSRGBA8888;
	view.contentScaleFactor = self.window.screen.nativeScale;
	
	CADisplayLink* displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(render:)];
	[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
	
	UIViewController* controller = [[UIViewController alloc] init];
	controller.view = view;
	[self.window setRootViewController:controller];
	
	[self.window makeKeyAndVisible];
	
	Platform::init();
	myApplication = new Application;

	return YES;
}

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	UITouch* touch = [touches anyObject];
	CGPoint location = [touch locationInView:touch.view];
	myApplication->mouseDown(location.x, location.y);
}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	UITouch* touch = [touches anyObject];
	CGPoint location = [touch locationInView:touch.view];
	myApplication->mouseUp(location.x, location.y);
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	UITouch* touch = [touches anyObject];
	CGPoint location = [touch locationInView:touch.view];
	myApplication->mouseMove(location.x, location.y);
}

- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
	UITouch* touch = [touches anyObject];
	CGPoint location = [touch locationInView:touch.view];
	myApplication->mouseUp(location.x, location.y);
}

@end

int main(int argc, char** argv)
{
	@autoreleasepool
	{
		chdir([[[NSBundle mainBundle] resourcePath] UTF8String]);

		return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
	}
}
