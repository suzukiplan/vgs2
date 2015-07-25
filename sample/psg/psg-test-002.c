#include <stdio.h>
#include "vgs2.h"
#include "psg.h"

struct BasicSound BS;

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
    
    puts("Commands:");
    puts("- key-on  : on key_number(0~87)");
    puts("- key-off : off");
    puts("- other   : quit\n");
    while(fgets(buf,sizeof(buf),stdin)) {
        lock();
        if(0==strncmp(buf,"on ",3)) {
            BS.key=atoi(&buf[3]);
            BS.key%=88;
            BS.on=1;
        } else if(0==strncmp(buf,"off",3)) {
            BS.on=0;
        } else {
            unlock();
            break;
        }
        unlock();
    }
    
    puts("Terminate sound-system.");
    term_sound();
    return 0;
}


void sndbuf(char* buf,size_t size)
{
    short* ptr=(short*)buf;
    int size16=(int)size/2;
    int i;
    lock();
    if(BS.on) {
        for(i=0;i<size16;i++,ptr++) {
            *ptr=(short)(square(BS.r/100) * 8192 / 256);
            BS.r += SCALE[BS.key];
        }
    } else {
        memset(buf,0,size);
    }
    unlock();
}
