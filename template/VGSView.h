/* (C)2014, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: GameDaddy - emulator
 *    Platform: iOS
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *        Date: 15-Mar-2014
 * FileVersion: 1.01
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
