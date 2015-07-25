#include <math.h>

static int SINTBL[628];

/* sinテーブル（256進数の固定小数点方式）を作成 */
void make_sin_table()
{
    int i;
    for(i=0;i<628;i++) {
        SINTBL[i] = (int)(sin((double)i / 100.0)*256);
    }
}

/* 矩形波を求める */
int square(unsigned int r)
{
    int ret;
    r%=628;
    ret=SINTBL[r];
    ret+=SINTBL[(r*3)%628]/3;
    ret+=SINTBL[(r*5)%628]/5;
    ret+=SINTBL[(r*7)%628]/7;
    ret+=SINTBL[(r*9)%628]/9;
    ret+=SINTBL[(r*11)%628]/11;
    ret+=SINTBL[(r*13)%628]/13;
    ret+=SINTBL[(r*15)%628]/15;
    return ret;
}
