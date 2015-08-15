#define S 22050.00
#define PI 3.1415926535897932384626433832795
#define B  1.0594630943592952645618252949463

int SCALE[88];

void make_scale_table()
{
    double hz;
   	double phz;
    int n;
    double a=13.75;
    for(n=0;n<88;n++,phz=hz) {
        if(n%12==0) {
            a*=2.0;
            hz=a;
        } else {
            hz = phz * B;
        }
        SCALE[n]=PI*2/(S/hz)*10000;
    }
}
