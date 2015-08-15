#include <stdio.h>
#include "vgs2.h"
#include "psg.h"

int main()
{
    char buf[1024];
    
    puts("Initialize sound-system");
    if(init_sound_cli()) {
        puts("Failed!");
        return -1;
    }
    make_sin_table();
    make_scale_table();
    
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
    static int n[8] = { 3+48, 5+48, 7+48, 8+48, 10+48, 12+48, 14+48, 15+48 };
    static int key;   /* 現在の音程 (nの添え字になる)  */
    static int hz;    /* 1秒をカウントするための変数 */
    static int pw;    /* 音量(256 = 100%) */
    
    static int r=0;
    short* ptr=(short*)buf;
    int i;
    int size16=size/2;
    
    lock();
    for(i=0;i<size16;i++,ptr++) {
        *ptr=(short)(square(r/100) * 8192 * pw / 65536);
        r += SCALE[n[key]];
        
        /* 音量を増減させる */
        if(hz < 11025) {
            /* 増幅(0Hzの時に無音 → 11025Hzの時に最大) */
            pw = hz*256 / 11025;
        } else {
            /* 減衰(11025Hzの時に最大 → 22050Hzの時に無音) */
            pw = (22050-hz)*256 / 11025;
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
