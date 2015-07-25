#include <stdio.h>
#include "psg.h"

int apc_operation(struct PsgContext* ctx)
{
    unsigned int wt=0;
    if(NULL==ctx->op || 0==ctx->op[ctx->index].code) {
        return 0; /* no operation */
    }
    while(0==wt && ctx->op[ctx->index].code) {
        switch(ctx->op[ctx->index].code & 0xf) {
            case OP_KEY_OFF:
                ctx->ch[0].bs.on=0;
            case OP_WAIT:
                wt=ctx->op[ctx->index].a;
                wt<<=16;
                wt|=ctx->op[ctx->index].v;
                break;
            case OP_KEY_ON:
                ctx->ch[0].bs.on=1;
                wt=ctx->op[ctx->index].v;
                ctx->ch[0].bs.key=ctx->op[ctx->index].a;
                ctx->ch[0].bs.vel=ctx->op[ctx->index].v;
				break;
		}
        ctx->index++;
    }
    ctx->wt=wt;
    return wt;
}
