/* (C)2015, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGS mk-II SR - emulator
 *    Platform: iOS
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#import "VGSWindow.h"
#import "VGSAppDelegate.h"
#import "VGSViewController.h"

@implementation VGSWindow

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

- (void)sendEvent:(UIEvent *)event
{
    UIView* gesture=self.rootViewController.view;
    if (gesture /*TransformGesture *gesture in transformGestures*/) {
        NSSet *touches = [event allTouches];
        NSMutableSet *began = nil;
        NSMutableSet *moved = nil;
        NSMutableSet *ended = nil;
        NSMutableSet *cancelled = nil;
        for(UITouch *touch in touches) {
            switch ([touch phase]) {
                case UITouchPhaseBegan:
                    if (!began) began = [NSMutableSet set];
                    [began addObject:touch];
                    break;
                case UITouchPhaseMoved:
                    if (!moved) moved = [NSMutableSet set];
                    [moved addObject:touch];
                    break;
                case UITouchPhaseEnded:
                    if (!ended) ended = [NSMutableSet set];
                    [ended addObject:touch];
                    break;
                case UITouchPhaseCancelled:
                    if (!cancelled) cancelled = [NSMutableSet set];
                    [cancelled addObject:touch];
                    break;
                default:
                    break;
            }
        }
        if (began) [gesture touchesBegan:began withEvent:event];
        if (moved) [gesture touchesMoved:moved withEvent:event];
        if (ended) [gesture touchesEnded:ended withEvent:event];
        if (cancelled) [gesture touchesCancelled:cancelled withEvent:event];
    }
    [super sendEvent:event];
}

@end
