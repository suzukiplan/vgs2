#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct VgsTarHeader {
    char eyecatch[8];
    unsigned char page;
    unsigned char num;
    char endian;
    char reserved1;
    unsigned int reserved2;
};

int main(int argc, char* argv[])
{
    FILE* fp;
    FILE* fpR;
    struct VgsTarHeader head;
    int i = 1;
    int n;
    int s;
    int c;
    char buf[8192];

    memset(&head, 0, sizeof(head));

    strcpy(head.eyecatch, "VGSARCH");
    if (((char*)&i)[0]) {
        head.endian = 'L';
    } else {
        head.endian = 'B';
    }

    if (argc < 3) {
        puts("usage: vgs2tar output input1 [input2...]");
        return 1;
    }

    head.page = (unsigned char)(((argc - 2) & 0xff00) >> 8);
    head.num = (unsigned char)((argc - 2) & 0xff);
    n = (argc - 2) & 0xffff;

    if (NULL == (fp = fopen(argv[1], "wb"))) {
        puts("I/O error");
        return 2;
    }

    fwrite(&head, sizeof(head), 1, fp);

    for (i = 0; i < n; i++) {
        if (NULL == (fpR = fopen(argv[2 + i], "rb"))) {
            puts("I/O error");
            fclose(fp);
            return 3;
        }
        fseek(fpR, 0, SEEK_END);
        s = (int)ftell(fpR);
        fseek(fpR, 0, SEEK_SET);
        if (s < 1) {
            printf("Invalid file: %s\n", argv[2 + i]);
            fclose(fpR);
            fclose(fp);
            return 4;
        }
        fwrite(&s, sizeof(s), 1, fp);
        c = 0;
        while (0 < (s = fread(buf, 1, sizeof(buf), fpR))) {
            if (0 == c) {
                if (memcmp(buf, "VGSPACK", 8) != 0) {
                    printf("Invalid file: %s\n", argv[2 + i]);
                    fclose(fpR);
                    fclose(fp);
                    return 5;
                }
            }
            fwrite(buf, s, 1, fp);
            c++;
        }
        fclose(fpR);
    }

    fclose(fp);
    return 0;
}
