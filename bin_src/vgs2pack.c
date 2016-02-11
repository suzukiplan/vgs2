#ifdef __WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vgs2.h"
#include "vgsdec.h"

void* _psg;

static int getInfoNum(FILE* fp);
static int cpy(char* to, size_t size, char* from, int line);
static void trimstring(char* src);

int main(int argc, char* argv[])
{
    struct VgsMetaHeader head;
    struct VgsMetaData data;
    char buf[4096];
    FILE* bgm = NULL;
    FILE* meta = NULL;
    FILE* pack = NULL;
    size_t len;
    char* cp;
    int st = 0;
    int line = 0;
    int size;
    int i;

    int result = 1;

    if (argc < 2) {
        puts("vgs2pack base-filename");
        puts("- input(1): {base-filename}.BGM");
        puts("- input(2): {base-filename}.meta");
        puts("- output: {base-filename}.vgs");
        goto ENDPROC;
    }

    _psg = vgsdec_create_context();
    if (NULL == _psg) {
        puts("could not create vgs-bgm-decoder");
        goto ENDPROC;
    }

    if (NULL != (cp = strchr(argv[1], '.'))) *cp = '\0';

    sprintf(buf, "%s.BGM", argv[1]);
    if (NULL == (bgm = fopen(buf, "rb"))) {
        puts("BGM file not found.");
        goto ENDPROC;
    }
    if (vgsdec_load_bgm_from_file(_psg, buf)) {
        puts("Could not read the BGM file.");
        goto ENDPROC;
    }

    sprintf(buf, "%s.meta", argv[1]);
    if (NULL == (meta = fopen(buf, "rt"))) {
        puts("meta file not found.");
        goto ENDPROC;
    }

    sprintf(buf, "%s.vgs", argv[1]);
    if (NULL == (pack = fopen(buf, "wb"))) {
        puts("meta file not found.");
        goto ENDPROC;
    }

    memset(&head, 0, sizeof(head));
    memset(&data, 0, sizeof(data));
    memset(&buf, 0, sizeof(buf));

    if (0 == (head.num = (unsigned char)(getInfoNum(meta) & 0xff))) {
        puts("meta file has no \"Data:\".");
        goto ENDPROC;
    }

    i = vgsdec_get_value(_psg, VGSDEC_REG_LOOP_TIME);
    head.secIi = htons((unsigned short)(i / 22050));
    head.secIf = htons((unsigned short)(i % 22050 * 0.0453514739229));
    if (999 < head.secIf) head.secIf = 999;
    i = vgsdec_get_value(_psg, VGSDEC_REG_TIME_LENGTH);
    head.secLi = htons((unsigned short)(i / 22050));
    head.secLf = htons((unsigned short)(i % 22050 * 0.0453514739229));
    if (999 < head.secLf) head.secLf = 999;

    strcpy(head.eyecatch, "VGSPACK");

    while (fgets(buf, sizeof(buf) - 1, meta)) {
        line++;
        if (NULL != (cp = strchr(buf, '\r'))) *cp = '\0';
        if (NULL != (cp = strchr(buf, '\n'))) *cp = '\0';
        trimstring(buf);
        if ('#' == buf[0]) continue;
        len = strlen(buf);
        if (len < 1) continue;
        if (strcmp("Data:", buf) == 0) {
            if (0 == st)
                fwrite(&head, 1, sizeof(head), pack);
            else
                fwrite(&data, 1, sizeof(data), pack);
            memset(&data, 0, sizeof(data));
            st++;
            continue;
        }
        cp = strchr(buf, '=');
        if (NULL == cp) continue;
        *cp = '\0';
        cp++;
        trimstring(buf);
        trimstring(cp);
        if (0 == st) {
            if (strcmp(buf, "format") == 0) {
                if (cpy(head.format, sizeof(head.format), cp, line)) goto ENDPROC;
            } else if (strcmp(buf, "genre") == 0) {
                if (cpy(head.genre, sizeof(head.genre), cp, line)) goto ENDPROC;
            } else {
                printf("Error: unknown parameter \"%s\" (line=%d)\n", buf, line);
                goto ENDPROC;
            }
        } else {
            if (strcmp(buf, "year") == 0) {
                data.year = htons((unsigned short)atoi(cp));
            } else if (strcmp(buf, "aid") == 0) {
                data.aid = htons((unsigned short)atoi(cp));
            } else if (strcmp(buf, "track") == 0) {
                data.track = htons((unsigned short)atoi(cp));
            } else if (strcmp(buf, "album") == 0) {
                if (cpy(data.album, sizeof(data.album), cp, line)) goto ENDPROC;
            } else if (strcmp(buf, "song") == 0) {
                if (cpy(data.song, sizeof(data.song), cp, line)) goto ENDPROC;
            } else if (strcmp(buf, "team") == 0) {
                if (cpy(data.team, sizeof(data.team), cp, line)) goto ENDPROC;
            } else if (strcmp(buf, "creator") == 0) {
                if (cpy(data.creator, sizeof(data.creator), cp, line)) goto ENDPROC;
            } else if (strcmp(buf, "right") == 0) {
                if (cpy(data.right, sizeof(data.right), cp, line)) goto ENDPROC;
            } else if (strcmp(buf, "code") == 0) {
                if (cpy(data.code, sizeof(data.code), cp, line)) goto ENDPROC;
            } else {
                printf("Error: unknown parameter \"%s\" (line=%d)\n", buf, line);
                goto ENDPROC;
            }
        }
    }
    fwrite(&data, 1, sizeof(data), pack);

    while (0 < (size = fread(buf, 1, sizeof(buf), bgm))) {
        fwrite(buf, size, 1, pack);
    }

    result = 0;
ENDPROC:
    if (bgm) fclose(bgm);
    if (meta) fclose(meta);
    if (pack) fclose(pack);
    if (_psg) vgsdec_release_context(_psg);
    return result;
}

static int getInfoNum(FILE* fp)
{
    char buf[4096];
    char* cp;
    int n = 0;
    memset(buf, 0, sizeof(buf));
    while (fgets(buf, sizeof(buf) - 1, fp)) {
        if (NULL != (cp = strchr(buf, '\r'))) *cp = '\0';
        if (NULL != (cp = strchr(buf, '\n'))) *cp = '\0';
        trimstring(buf);
        if (strcmp("Data:", buf) == 0) n++;
    }
    fseek(fp, 0, SEEK_SET);
    return n;
}

static int cpy(char* to, size_t size, char* from, int line)
{
    if (size - 1 < strlen(from)) {
        printf("Error: length over (line=%d)\n", line);
        return -1;
    }
    strcpy(to, from);
    return 0;
}

static void trimstring(char* src)
{
    int i, j;
    int len;
    for (i = 0; ' ' == src[i]; i++)
        ;
    if (i) {
        for (j = 0; src[i] != '\0'; j++, i++) {
            src[j] = src[i];
        }
        src[j] = '\0';
    }
    for (len = (int)strlen(src) - 1; 0 <= len && ' ' == src[len]; len--) {
        src[len] = '\0';
    }
}

void putlog(const char* fn, int ln, const char* msg, ...)
{
}

void make_pallet()
{
}

FILE* vgs2_fopen(const char* n, const char* p)
{
    return fopen(n, p);
}

void vgs2_showAds()
{
}

void vgs2_deleteAds()
{
}
