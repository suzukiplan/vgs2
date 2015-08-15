#ifndef INCLUDE_PSG_H
#define INCLUDE_PSG_H

//#ifdef __cplusplus
//extern "C" {
//#endif

#define MAX_CHANNEL 6

#define OP_WAIT     1   /* wait     a:mul, v:time */
#define OP_KEY_ON   2   /* key-on   a:key, v:wait-time */
#define OP_KEY_OFF  3   /* key-off  a:mul, v:wait-time */
#define OP_ENV_AT   4   /* env-at   a:---, v:attack-time */
#define OP_ENV_DT   5   /* env-dt   a:sul, v:decay-time */
#define OP_ENV_RT   6   /* env-rt   a:---, v:release-time */
#define OP_VEL      7   /* velocity a:---. v:velocity */
#define OP_VOL      8   /* volume   a:---, v:master-volume */
#define OP_DOWN     9  /* pitch-d  a:---, v:interval */
#define OP_UP       10  /* pitch-u  a:---, v:interval */
#define OP_JUMP     11  /* jump     a:---, v:--- */
#define OP_LABEL    12  /* label    a:---, v:--- */
#define OP_NOISE    13  /* noise    a:---, v:noise-flag */

struct Operation {
    unsigned char code; /* channel & operation code */
    unsigned char a;    /* accumulator */
    unsigned short v;   /* value */
};

struct BasicSound {
    int on;             /* 0:key-off, 1:key-on */
    unsigned int key;   /* key-number: 0~87 */
    unsigned int r;     /* radian value * 10000 */
    unsigned int vel;   /* velocity: 0~15 */
};

struct Envelope {
    unsigned int at;    /* attack-time */
    unsigned int dt;    /* decay-time */
    unsigned int sl;    /* sustain-level */
    unsigned int rt;    /* release-time */
    unsigned int st;    /* status: 1=at, 2=dt, 3=st, 4=rt */
    int a;              /* add count per 1Hz */
    int pw;             /* power: 0~65535 */
    unsigned int et;    /* elapsed-time */
};

struct Pitch {
    unsigned int down;  /* pitch-down interval */
    unsigned int up;    /* pitch-up interval */
};

struct Channel {
    struct BasicSound bs;
    struct Envelope env;
    struct Pitch pt;
};

struct PsgContext {
    struct Operation* op;
    struct Channel ch[MAX_CHANNEL];
    int wt;             /* wait-time */
    int volume;         /* master-volume */
    int index;          /* operation-index */
    int label;
    unsigned int noise; /* noise-flag */
};

/* apc.c */
int apc_operation(struct PsgContext* ctx);

/* square.c */
void make_sin_table();
int square(unsigned int r);

/* scale.c */
extern int SCALE[88];
void make_scale_table();

//#ifdef __cplusplus
//};
//#endif

#endif /* INCLUDE_PSG_H */
