/* (C)2014, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: GameDaddy - emulator
 *    Platform: iOS
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *        Date: 15-Mar-2014
 * FileVersion: 1.01
 *----------------------------------------------------------------------------
 */
#import "VGSBarView.h"

@implementation VGSBarView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        myclock = [[UILabel alloc] init];
        myclock.textColor=[UIColor whiteColor];
        myclock.backgroundColor=[UIColor blackColor];
        myclock.font=[UIFont fontWithName:@"Arial-BoldMT" size:13.0f];
        NSDateFormatter *formatter = [[NSDateFormatter alloc]init];
        NSDate* date = [NSDate date];
        [formatter setDateStyle:NSDateFormatterFullStyle];
        [formatter setDateFormat:@"    dd-MMM-YYYY  HH:mm:ss    "];
        NSString* str=[formatter stringFromDate:date];
        myclock.text = str;
        myclock.frame=CGRectMake(0,0,320,20);
        [myclock sizeToFit];
        myclock.center = self.center;
        [self addSubview:myclock];
        myTicker = [NSTimer scheduledTimerWithTimeInterval:0.5
                                                    target:self
                                                  selector:@selector(showDate)
                                                  userInfo:nil
                                                   repeats:YES];
    }
    return self;
}

- (void)showDate {
    NSDateFormatter *formatter = [[NSDateFormatter alloc]init];
    NSDate* date = [NSDate date];
    [formatter setDateStyle:NSDateFormatterFullStyle];
    [formatter setDateFormat:@"    dd-MMM-YYYY  HH:mm:ss    "];
    NSString* str=[formatter stringFromDate:date];
    myclock.text=str;
}

@end
