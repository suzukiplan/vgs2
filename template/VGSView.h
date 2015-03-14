/* (C)2015, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGS mk-II SR - emulator
 *    Platform: iOS
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#import <pthread.h>
#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import <QuartzCore/CALayer.h>
#import <UIKit/UIKit.h>

@interface VGSLayer : CALayer
- (void) orientationChanged:(NSNotification *)notification;

@end

@interface VGSView : UIView
{
    CADisplayLink* mpDisplayLink;
}
- (id)initWithFrame:(CGRect)frame;
- (void)drawRect:(CGRect)rect;

@end
