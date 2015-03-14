/* (C)2015, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGS mk-II SR - emulator
 *    Platform: iOS
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#import <UIKit/UIKit.h>
#import <OpenAL/al.h>
#import <OpenAL/alc.h>

@class VGSView;

@interface VGSViewController : UIViewController
{
    VGSView* vram;
    UIImageView* panel;
}

@end
