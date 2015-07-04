#include <stdio.h>
#include <math.h>
#include "vgs2.h"

#define PI2 6.2831853071795864

int main()
{
    char buf[1024];
    
    puts("Initialize sound-system");
    if(init_sound_cli()) {
        puts("Failed!");
        return -1;
    }
    
    puts("Enter to finish program.");
    while(fgets(buf,sizeof(buf),stdin)) {
        break;
    }
    
    puts("Terminate sound-system.");
    term_sound();
    return 0;
}

void sndbuf(char* buf,size_t size)
{
    /* ドレミファソラシドの周波数テーブル (const/final) */
    static const double n[8] = {
        523.2511, 587.3296, 659.2551, 698.4565,
        783.9909, 880.0000, 987.7666, 1046.5023
    };
    static int key;   /* 現在の音程 (nの添え字になる) : 0〜7 の範囲 */
    static int hz;    /* 1秒をカウントするための変数 (22050になったらkeyをインクリメント) */
    static double pw; /* 音量(1.0 = 100%) */
    
    static double r=0.0;
    short* ptr=(short*)buf;
    int i;
    int size16=size/2;
    
    lock();
    for(i=0;i<size16;i++,ptr++) {
        *ptr=(short)(((r < PI2/2) ? -1.0 : 1.0) * 8192 * pw);
        r += PI2 / (22050 / n[key]); /* n[key]Hz にする */
        if(PI2 <= r) r -= PI2; /* rをPI2未満にする */
        
        /* 音量を増減させる */
        if(hz < 11025) {
            /* 増幅(0Hzの時に無音 → 11025Hzの時に最大) */
            pw = (double)hz / 11025.0;
        } else {
            /* 減衰(11025Hzの時に最大 → 22050Hzの時に無音) */
            pw = (double)(22050-hz) / 11025.0;
        }
        
        /* 1秒経過したら音程を変える */
        if(22050 == (++hz)) {
            hz = 0;
            key++;
            key &= 7;
        }
    }
    unlock();
}
