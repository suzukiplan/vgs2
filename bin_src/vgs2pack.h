#ifndef INCLUDE_VGS2PACK_H
#define INCLUDE_VGS2PACK_H

struct VgsMetaHeader {
    char eyecatch[8];
    char format[8];
    char genre[32];
    unsigned char num;
    unsigned char reserved1[3];
    unsigned short secIi;
    unsigned short secIf;
    unsigned short secLi;
    unsigned short secLf;
    unsigned int reserved2;
};

struct VgsMetaData {
    unsigned short year;
    unsigned short aid;
    unsigned short track;
    unsigned short reserved;
    char album[56];
    char song[64];
    char team[32];
    char creator[32];
    char right[32];
    char code[32];
};

#endif
