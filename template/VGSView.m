/* (C)2015, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGS mk-II SR - emulator
 *    Platform: iOS
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#import <Foundation/NSURL.h>
#import "VGSView.h"
#import "VGSViewController.h"
#import "vgs2.h"

extern unsigned short ADPAL[256];
extern unsigned char _interlace;

static VGSView* sharedInstance = nil;
static unsigned short imgbuf[2][400 * 320 * 2];
static CGContextRef img[2];
static pthread_t tid=0;
static volatile int bno;
static volatile bool event_flag=false;
static volatile bool alive_flag=true;
static volatile bool end_flag=false;

@implementation VGSLayer

// Main loop
static void* GameLoop(void* args)
{
    int i,j;
    unsigned short* buf;
    unsigned char* bg;
    unsigned char* sp;
    
    while(alive_flag) {
        while(event_flag) usleep(100);
        // call AP procedure
        if(_pause) {
            vgs2_pause();
        } else {
            vgs2_loop();
        }
        
        // NOTES: does not implements the exit procedure of VGS.
        // REASON: Apple rule.
        
        // VRAM to CG image buffer
        buf=(unsigned short*)imgbuf[bno];
        bg=_vram.bg;
        sp=_vram.sp;
        if(_interlace) {
            for(i=0;i<200;i++) { // 160 x 200
                for(j=0;j<160;j++) {
                    if(*sp) {
                        *buf=ADPAL[*sp];
                        *(buf+320)=0;
                        buf++;
                        *buf=ADPAL[*sp];
                        *(buf+320)=0;
                        buf++;
                        *sp=0;
                    } else {
                        *buf=ADPAL[*bg];
                        *(buf+320)=0;
                        buf++;
                        *buf=ADPAL[*bg];
                        *(buf+320)=0;
                        buf++;
                    }
                    bg++;
                    sp++;
                }
                buf+=320;
            }
        } else {
            for(i=0;i<200;i++) { // 160 x 200
                for(j=0;j<160;j++) {
                    if(*sp) {
                        *buf=ADPAL[*sp];
                        *(buf+320)=ADPAL[*sp];
                        buf++;
                        *buf=ADPAL[*sp];
                        *(buf+320)=ADPAL[*sp];
                        buf++;
                        *sp=0;
                    } else {
                        *buf=ADPAL[*bg];
                        *(buf+320)=ADPAL[*bg];
                        buf++;
                        *buf=ADPAL[*bg];
                        *(buf+320)=ADPAL[*bg];
                        buf++;
                    }
                    bg++;
                    sp++;
                }
                buf+=320;
            }
        }
        event_flag=true;
    }
    end_flag=true;
    return NULL;
}

+ (id) defaultActionForKey:(NSString *)key
{
    return nil;
}

- (id)init {
    if (self = [super init]) {
        img[0] = nil;
        img[1] = nil;
        // create image buffer
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        for(int i=0;i<2;i++) {
            img[i] = CGBitmapContextCreate(imgbuf[i],
                                    320,400,
                                    5,
                                    2*320,
                                    colorSpace,
                                    kCGImageAlphaNoneSkipFirst|
                                    kCGBitmapByteOrder16Little
                                    );
        }
        CFRelease(colorSpace);
        // start buffering thread
        pthread_create(&tid, NULL, GameLoop, NULL);
        struct sched_param param;
        memset(&param,0,sizeof(param));
        param.sched_priority = 46;
        pthread_setschedparam(tid,SCHED_OTHER,&param);
    }
    return self;
}

- (void) orientationChanged:(NSNotification *)notification
{
	// does not implement because portlate only
}

- (void)display {
    while(!event_flag) usleep(100); // wait for sync
    bno=1-bno;                      // flip to another vram
    event_flag=false;               // start another buffering
    // visible the previous buffer
    CGImageRef cgImage = CGBitmapContextCreateImage(img[1-bno]);
    self.contents = (__bridge id)cgImage;
    CFRelease(cgImage);
}

- (void)dealloc
{
    alive_flag=false;
    while(!end_flag) usleep(100);
    for(int i=0;i<2;i++) {
        if(img[i]!=nil) {
            CFRelease(img[i]);
            img[i]=nil;
        }
    }
}
@end

@implementation VGSView

+ (Class) layerClass
{
    return [VGSLayer class];
}

- (id)initWithFrame:(CGRect)frame {
    if ((self = [super initWithFrame:frame])!=nil) {
        // slow interfaces are no thanks (this view is video only)
        self.opaque = NO;
        self.clearsContextBeforeDrawing = NO;
        self.multipleTouchEnabled = NO;
        self.userInteractionEnabled = NO;
        sharedInstance=self;
        // call setNeedsDisplay by VSYNC
        mpDisplayLink=[CADisplayLink displayLinkWithTarget:self selector:@selector(setNeedsDisplay)];
        [mpDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }
    return self;
}

- (void)dealloc
{
    // not need destroy procedure of CADisplayLink
}

- (void)drawRect:(CGRect)rect
{
    // no draw in view, because VERY slow
}

@end
