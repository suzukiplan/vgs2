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
    static int key; /* 現在の音程 (nの添え字になる) : 0〜7 の範囲 */
    static int hz;  /* 1秒をカウントするための変数 (22050になったらkeyをインクリメント) */
    
    static double r=0.0;
    short* ptr=(short*)buf;
    int i;
    int size16=size/2;
    
    lock();
    for(i=0;i<size16;i++,ptr++) {
        *ptr=(short)(sin(r)*16384);
        r += PI2 / (22050 / n[key]); /* n[key]Hz にする */
        
        /* 1秒経過したら音程を変える */
        if(22050 == (++hz)) {
            hz = 0;
            key++;
            key &= 7;
            
        }
    }
    unlock();
}
