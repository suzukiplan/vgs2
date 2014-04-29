/* (C)2014, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: GameDaddy - emulator
 *    Platform: iOS
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *        Date: 15-Mar-2014
 * FileVersion: 1.01
 *----------------------------------------------------------------------------
 */
#import <UIKit/UIKit.h>

@interface VGSBarView : UIView {
    UILabel* myclock;
    NSTimer* myTicker;
}

- (void)showDate;

@end
