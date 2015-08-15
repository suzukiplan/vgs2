#include "psg.h"

#define BASE 22050

struct Operation waon[] = {
    /* エンベロープを設定 */
    { 0x00 | OP_ENV_AT, 0, 220 }
    , { 0x00 | OP_ENV_DT, 120, 4410 }
    , { 0x00 | OP_ENV_RT, 0, 11025 }
    , { 0x10 | OP_ENV_AT, 0, 220 }
    , { 0x10 | OP_ENV_DT, 120, 4410 }
    , { 0x10 | OP_ENV_RT, 0, 11025 }
    , { 0x20 | OP_ENV_AT, 0, 220 }
    , { 0x20 | OP_ENV_DT, 120, 4410 }
    , { 0x20 | OP_ENV_RT, 0, 11025 }
    , { 0x30 | OP_ENV_AT, 0, 220 }
    , { 0x30 | OP_ENV_DT, 120, 4410 }
    , { 0x30 | OP_ENV_RT, 0, 11025 }
    
    /* C */
    ,{ 0x00 | OP_KEY_ON , 51, 0 }
    ,{ 0x10 | OP_KEY_ON , 55, 0 }
    ,{ 0x20 | OP_KEY_ON , 58, 0 }
    ,{ 0x30 | OP_KEY_OFF,  0, BASE }
    
    /* G7 */
    ,{ 0x00 | OP_KEY_ON , 50, 0 }
    ,{ 0x10 | OP_KEY_ON , 53, 0 }
    ,{ 0x20 | OP_KEY_ON , 56, 0 }
    ,{ 0x30 | OP_KEY_ON , 58, BASE }
    
    /* C */
    ,{ 0x00 | OP_KEY_ON , 51, 0 }
    ,{ 0x10 | OP_KEY_ON , 55, 0 }
    ,{ 0x20 | OP_KEY_ON , 58, 0 }
    ,{ 0x30 | OP_KEY_OFF,  0, BASE }
    
    /* （おしまい） */
    , { 0x00 | OP_KEY_OFF, 0, 0 }
    , { 0x10 | OP_KEY_OFF, 0, 0 }
    , { 0x20 | OP_KEY_OFF, 0, 0 }
    , { 0x30 | OP_KEY_OFF, 0, BASE*2 }
    , { 0, 0, 0 }
};
