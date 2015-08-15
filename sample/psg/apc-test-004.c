#include <stdio.h>
#include "psg.h"

int apc_operation(struct PsgContext* ctx)
{
    unsigned int wt=0;
    if(NULL==ctx->op || 0==ctx->op[ctx->index].code) {
        return 0; /* no operation */
    }
    while(0==wt && ctx->op[ctx->index].code) {
        if(0==ctx->ch[0].env.at) ctx->ch[0].env.at=1;
        if(0==ctx->ch[0].env.dt) ctx->ch[0].env.dt=1;
        if(0==ctx->ch[0].env.rt) ctx->ch[0].env.rt=1;
        switch(ctx->op[ctx->index].code & 0xf) {
            case OP_KEY_OFF:
                ctx->ch[0].bs.on=0;
                ctx->ch[0].env.st=4;
                ctx->ch[0].env.a=ctx->ch[0].env.pw;
                ctx->ch[0].env.a/=ctx->ch[0].env.rt;
                ctx->ch[0].env.a=-ctx->ch[0].env.a;
            case OP_WAIT:
                wt=ctx->op[ctx->index].a;
                wt<<=16;
                wt|=ctx->op[ctx->index].v;
                break;
            case OP_KEY_ON:
                ctx->ch[0].bs.on=1;
                ctx->ch[0].bs.key=ctx->op[ctx->index].a;
                wt=ctx->op[ctx->index].v;
                ctx->ch[0].env.st=1;
                ctx->ch[0].env.pw=0;
                ctx->ch[0].env.a=65535/ctx->ch[0].env.at;
                break;
            case OP_ENV_AT:
                ctx->ch[0].env.at=ctx->op[ctx->index].a;
                ctx->ch[0].env.at<<=16;
                ctx->ch[0].env.at|=ctx->op[ctx->index].v;
                break;
            case OP_ENV_DT:
                ctx->ch[0].env.sl=ctx->op[ctx->index].a;
                ctx->ch[0].env.dt=ctx->op[ctx->index].v;
                break;
            case OP_ENV_RT:
                ctx->ch[0].env.rt=ctx->op[ctx->index].a;
                ctx->ch[0].env.rt<<=16;
                ctx->ch[0].env.rt|=ctx->op[ctx->index].v;
                break;
        }
        ctx->index++;
    }
    ctx->wt=wt;
    return wt;
}
