/* (C)2015, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGS mk-II SR - emulator
 *    Platform: iOS
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#import "VGSViewController.h"
#import "VGSScreenView.h"
#import "VGSView.h"
#import "VGSBarView.h"
#import "vgs2.h"

extern int isIphone5;

bool bannerIsVisible=NO;
int vgsint_init(const char* bin);
void vgsint_setdir(const char* dir);

@interface VGSViewController ()

@end

@implementation VGSViewController

- (void)viewDidLoad
{
    // fix for iOS8's bug
    static int viewDidLoaded=0;
    if(viewDidLoaded) return;
    viewDidLoaded=1;

    [super viewDidLoad];

    const float version = [[[UIDevice currentDevice] systemVersion] floatValue];
    NSLog(@"iOS version: %f", version);

    bool iPhone5=false;
    CGSize result = [[UIScreen mainScreen] bounds].size;
    CGFloat scale = [UIScreen mainScreen].scale;
    result = CGSizeMake(result.width * scale, result.height * scale);

    UIApplication *app = [UIApplication sharedApplication];
    app.statusBarHidden=YES;

    if (result.height == 1136.0) {
        iPhone5=true;
        isIphone5=1;
    } else {
        iPhone5=false;
        isIphone5=0;
    }
    self.view=[[VGSScreenView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    // initialize VGS
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *docs_dir = [paths objectAtIndex:0];
    vgsint_setdir([docs_dir UTF8String]);
    NSString* fullPath;
    fullPath=[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"romdata.bin"];
    if(vgsint_init([fullPath UTF8String])) {
        NSLog("ERROR: romdata.bin may not placed in the project! please confirm.");
        _exit(-1);
    }
    // set vram window
    if(iPhone5) {
        vram=[[VGSView alloc] initWithFrame:CGRectMake(0, 70, 320, 400)];
        [self.view addSubview:vram];
        UIImage *imageA= [UIImage imageNamed:@"panel1.png"];
        UIImageView *ivA= [[UIImageView alloc] initWithImage:imageA];
        ivA.frame=CGRectMake(0,20,320,50);
        [self.view addSubview:ivA];
        UIImage *imageB = [UIImage imageNamed:@"panel2.png"];
        UIImageView *ivB = [[UIImageView alloc] initWithImage:imageB];
        ivB.frame=CGRectMake(0,470,320,96);
        [self.view addSubview:ivB];
    } else {
        vram=[[VGSView alloc] initWithFrame:CGRectMake(0, 0, 320, 400)];
        [self.view addSubview:vram];
        UIImage *imageA= [UIImage imageNamed:@"panel2.png"];
        UIImageView *ivA= [[UIImageView alloc] initWithImage:imageA];
        ivA.frame=CGRectMake(0,400,320,80);
        [self.view addSubview:ivA];
    }

    if(iPhone5) {
        // set status bar
        VGSBarView *bar=[[VGSBarView alloc] initWithFrame:CGRectMake(0,0,320,20)];
        [self.view addSubview:bar];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

- (BOOL)prefersStatusBarHidden
{
    return NO;
}

@end
