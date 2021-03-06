/* (C)2015, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGS mk-II SR - emulator
 *    Platform: iOS
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#import "VGSAppDelegate.h"
#import "VGSViewController.h"
#import "VGSView.h"
#import "VGSWindow.h"
void vgs2_term();

@implementation VGSAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // initialize window
    window = [[VGSWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    viewController = [[VGSViewController alloc] init];
    window.rootViewController=viewController;
    [viewController awakeFromNib];
    [window addSubview:viewController.view];
    [window makeKeyAndVisible];
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    NSLog(@"GameDaddy now be ended.");
    vgs2_term();
    [[UIApplication sharedApplication] setIdleTimerDisabled:NO];
}

@end
