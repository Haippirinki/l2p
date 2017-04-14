#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import <QuartzCore/QuartzCore.h>

@interface AppDelegate : UIResponder <UIApplicationDelegate, GLKViewDelegate>
@property (strong, nonatomic) UIWindow *window;
@end

@implementation AppDelegate

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    glClearColor(1.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

- (void)render:(CADisplayLink*)displayLink
{
    GLKView* view = [self.window.subviews objectAtIndex:0];
    [view display];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    
    EAGLContext* context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    GLKView* view = [[GLKView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    view.context = context;
    view.delegate = (id)self;
    view.enableSetNeedsDisplay = NO;
    
    CADisplayLink* displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(render:)];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    
    UIViewController* controller = [[UIViewController alloc] init];
    controller.view = view;
    [self.window setRootViewController:controller];
    
    [self.window makeKeyAndVisible];
    
    return YES;
}

@end

int main(int argc, char * argv[])
{
    @autoreleasepool
    {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
