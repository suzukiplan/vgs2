#include "psg.h"

#define BASE 22050

struct Operation kaeru[] = {
    /* エンベロープを設定 */
    { OP_ENV_AT, 0, 220 }
    , { OP_ENV_DT, 120, 4410 }
    , { OP_ENV_RT, 0, 11025 }
    
    /* かーえーるーのーうーたーがーー */
    , { OP_KEY_ON, 51, BASE/4 }
    , { OP_KEY_ON, 53, BASE/4 }
    , { OP_KEY_ON, 55, BASE/4 }
    , { OP_KEY_ON, 56, BASE/4 }
    , { OP_KEY_ON, 55, BASE/4 }
    , { OP_KEY_ON, 53, BASE/4 }
    , { OP_KEY_ON, 51, BASE/2 }
    
    /* きーこーえーてーくーるーよーー */
    , { OP_KEY_ON, 55, BASE/4 }
    , { OP_KEY_ON, 56, BASE/4 }
    , { OP_KEY_ON, 58, BASE/4 }
    , { OP_KEY_ON, 60, BASE/4 }
    , { OP_KEY_ON, 58, BASE/4 }
    , { OP_KEY_ON, 56, BASE/4 }
    , { OP_KEY_ON, 55, BASE/2 }
    
    /* ぐわっ　ぐわっ　ぐわっ　ぐわっ */
    , { OP_KEY_ON, 51, BASE/4 }
    , { OP_KEY_OFF, 0, BASE/4 }
    , { OP_KEY_ON, 51, BASE/4 }
    , { OP_KEY_OFF, 0, BASE/4 }
    , { OP_KEY_ON, 51, BASE/4 }
    , { OP_KEY_OFF, 0, BASE/4 }
    , { OP_KEY_ON, 51, BASE/4 }
    , { OP_KEY_OFF, 0, BASE/4 }
    
    /* げろ げろ げろ げろ */
    , { OP_KEY_ON, 51, BASE/8 }
    , { OP_KEY_ON, 51, BASE/8 }
    , { OP_KEY_ON, 53, BASE/8 }
    , { OP_KEY_ON, 53, BASE/8 }
    , { OP_KEY_ON, 55, BASE/8 }
    , { OP_KEY_ON, 55, BASE/8 }
    , { OP_KEY_ON, 56, BASE/8 }
    , { OP_KEY_ON, 56, BASE/8 }
    
    /* ぐわっ ぐわっ ぐわーー */
    , { OP_KEY_ON, 55, BASE/4 }
    , { OP_KEY_ON, 53, BASE/4 }
    , { OP_KEY_ON, 51, BASE/2 }
    , { OP_KEY_OFF, 0, BASE }
    
    /* （おしまい） */
    , { 0, 0, 0 }
};
