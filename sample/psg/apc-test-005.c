#include <stdio.h>
#include "psg.h"

int apc_operation(struct PsgContext* ctx)
{
    unsigned int wt=0;
    int n;
    if(NULL==ctx->op || 0==ctx->op[ctx->index].code) {
        return 0; /* no operation */
    }
    while(0==wt && ctx->op[ctx->index].code) {
        n=ctx->op[ctx->index].code & 0xf0;
        n>>=4;
        if(0==ctx->ch[n].env.at) ctx->ch[n].env.at=1;
        if(0==ctx->ch[n].env.dt) ctx->ch[n].env.dt=1;
        if(0==ctx->ch[n].env.rt) ctx->ch[n].env.rt=1;
        switch(ctx->op[ctx->index].code & 0xf) {
            case OP_KEY_OFF:
                ctx->ch[n].bs.on=0;
                ctx->ch[n].env.st=4;
                ctx->ch[n].env.a=ctx->ch[n].env.pw;
                ctx->ch[n].env.a/=ctx->ch[n].env.rt;
                ctx->ch[n].env.a=-ctx->ch[n].env.a;
            case OP_WAIT:
                wt=ctx->op[ctx->index].a;
                wt<<=16;
                wt|=ctx->op[ctx->index].v;
                break;
            case OP_KEY_ON:
                ctx->ch[n].bs.on=1;
                ctx->ch[n].bs.key=ctx->op[ctx->index].a;
                wt=ctx->op[ctx->index].v;
                ctx->ch[n].env.st=1;
                ctx->ch[n].env.pw=0;
                ctx->ch[n].env.a=65535/ctx->ch[n].env.at;
                break;
            case OP_ENV_AT:
                ctx->ch[n].env.at=ctx->op[ctx->index].a;
                ctx->ch[n].env.at<<=16;
                ctx->ch[n].env.at|=ctx->op[ctx->index].v;
                break;
            case OP_ENV_DT:
                ctx->ch[n].env.sl=ctx->op[ctx->index].a;
                ctx->ch[n].env.dt=ctx->op[ctx->index].v;
                break;
            case OP_ENV_RT:
                ctx->ch[n].env.rt=ctx->op[ctx->index].a;
                ctx->ch[n].env.rt<<=16;
                ctx->ch[n].env.rt|=ctx->op[ctx->index].v;
                break;
        }
        ctx->index++;
    }
    ctx->wt=wt;
    return wt;
}
