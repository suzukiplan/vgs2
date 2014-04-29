/* (C)2014, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: GameDaddy - emulator
 *    Platform: iOS
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *        Date: 15-Mar-2014
 * FileVersion: 1.01
 *----------------------------------------------------------------------------
 */
#import "VGSScreenView.h"
#import "vgs2.h"
extern int isIphone5;

@implementation VGSScreenView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        NSLog(@"width=%f, height=%f"
              ,self.frame.size.width
              ,self.frame.size.height
              );
        self.backgroundColor=[UIColor blackColor]; /*VERY IMPORTANT*/
        self.multipleTouchEnabled=YES;
        self.exclusiveTouch=YES;
    }
    return self;
}

// detect touch
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    touches=[event allTouches];
    UITouch* aTouch=[touches anyObject];
    CGPoint point=[aTouch locationInView:self];
    if(2==touches.count) {
        memset(&_touch,0,sizeof(_touch));
        return;
    }
    if(3<=touches.count && 0==_pause) {
        vgs2_setPause(1);
        memset(&_touch,0,sizeof(_touch));
        return;
    }
    _touch.s=1;
    _touch.t++;
    _touch.px=_touch.cx;
    _touch.py=_touch.cy;
    _touch.cx=(int)(point.x/2);
    _touch.cy=(int)(point.y/2);
    _touch.cy-=25;
    if(isIphone5) _touch.cy-=10;
    if(_touch.px) _touch.dx+=_touch.cx-_touch.px;
    if(_touch.py) _touch.dy+=_touch.cy-_touch.py;
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    touches=[event allTouches];
    UITouch* aTouch=[touches anyObject];
    CGPoint point=[aTouch locationInView:self];
    if(2==touches.count) {
        memset(&_touch,0,sizeof(_touch));
        return;
    }
    if(3<=touches.count && 0==_pause) {
        vgs2_setPause(1);
        memset(&_touch,0,sizeof(_touch));
        return;
    }
    _touch.s=1;
    _touch.t++;
    _touch.px=_touch.cx;
    _touch.py=_touch.cy;
    _touch.cx=(int)(point.x/2);
    _touch.cy=(int)(point.y/2);
    _touch.cy-=25;
    if(isIphone5) _touch.cy-=10;
    if(_touch.px) _touch.dx+=_touch.cx-_touch.px;
    if(_touch.py) _touch.dy+=_touch.cy-_touch.py;
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    memset(&_touch,0,sizeof(_touch));
}

@end
