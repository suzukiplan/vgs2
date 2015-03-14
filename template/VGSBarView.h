/* (C)2015, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGS mk-II SR - emulator
 *    Platform: iOS
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#import <UIKit/UIKit.h>

@interface VGSBarView : UIView {
    UILabel* myclock;
    NSTimer* myTicker;
}

- (void)showDate;

@end
