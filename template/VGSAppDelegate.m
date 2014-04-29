/* (C)2014, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: GameDaddy - emulator
 *    Platform: iOS
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *        Date: 15-Mar-2014
 * FileVersion: 1.01
 *----------------------------------------------------------------------------
 */
#import "VGSAppDelegate.h"
#import "VGSViewController.h"
#import "VGSView.h"
#import "VGSWindow.h"

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
    [[UIApplication sharedApplication] setIdleTimerDisabled:NO];
}

@end
