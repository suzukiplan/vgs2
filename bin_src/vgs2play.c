/* VGS BGM player cli */
#ifdef _WIN32
#include <Windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/timeb.h>
#endif
#include <stdio.h>
#include <time.h>
#include "vgsdec.c"
#include "vgsmml.c"
#include "vgs2.h"

void* _psg;

/**
 * get secound from text
 */
int getsec(const char* text)
{
    char* cp;
    int ret = 0;
    cp = strchr(text, ':');
    if (NULL == cp) {
        ret = atoi(text);
    } else {
        ret = atoi(text) * 60;
        ret += atoi(cp + 1);
    }
    return ret;
}

int isMML(const char* file)
{
    const char* cp;
    cp = strrchr(file, '.');
    if (NULL == cp) {
        return 0;
    }
    cp++;
    if (*cp != 'm' && *cp != 'M') {
        return 0;
    }
    cp++;
    if (*cp != 'm' && *cp != 'M') {
        return 0;
    }
    cp++;
    if (*cp != 'l' && *cp != 'L') {
        return 0;
    }
    cp++;
    if (*cp != '\0') {
        return 0;
    }
    return 1;
}

/**
 * main procedure
 */
int main(int argc, char* argv[])
{
    char buf[1024];
    int isPlaying = 1;
    int i, j;
    int st = 0;
    char* cp;
    int show = 0;

    /* check argument */
    if (argc < 2) {
        puts("usage: vgs2play bgm-file [mm:ss]");
        return 1;
    }
    if (3 <= argc) {
        st = getsec(argv[2]);
    }

    /* initialize vgs-bgm-decoder */
    _psg = vgsdec_create_context();
    if (NULL == _psg) {
        fprintf(stderr, "Could not initialize the vgs-bgm-decoder.\n");
        return 2;
    }

    /* initialize sound system */
    if (init_sound_cli()) {
        fprintf(stderr, "Could not initialize the sound system.\n");
        return 2;
    }

RELOAD:
    /* load BGM data and play */
    if (isMML(argv[1])) {
        struct VgsMmlErrorInfo err;
        struct VgsBgmData* bgm;
        bgm = vgsmml_compile_from_file(argv[1], &err);
        if (NULL == bgm) {
            if (err.line) {
                fprintf(stderr, "MML compile error (line=%d): %s\n", err.line, err.message);
            } else {
                fprintf(stderr, "MML compile error: %s\n", err.message);
            }
            return 2;
        }
        if (vgsdec_load_bgm_from_memory(_psg, bgm->data, bgm->size)) {
            fprintf(stderr, "Load error.\n");
            return 2;
        }
        vgsmml_free_bgm_data(bgm);
    } else {
        if (vgsdec_load_bgm_from_file(_psg, argv[1])) {
            fprintf(stderr, "Load error.\n");
            return 2;
        }
    }
    if (st) {
        vgs2_bjump(st);
    } else {
        vgs2_bjump(0);
    }

    if (!show) {
        /* meta data */
        struct VgsMetaHeader* mhead;
        struct VgsMetaData* mdata;

        /* meta header if exist */
        mhead = vgsdec_get_meta_header(_psg);
        printf("META-HEADER: ");
        if (NULL != mhead) {
            printf("\n");
            printf(" - format: %s\n", mhead->format);
            printf(" - genre: %s\n", mhead->genre);
            printf(" - data count: %d\n", (int)mhead->num);
        } else {
            printf("n/a\n");
        }
        puts("");

        /* meta data if exist */
        for (i = 0; NULL != (mdata = vgsdec_get_meta_data(_psg, i)); i++) {
            printf("META-DATA #%d:\n", i + 1);
            printf(" - year: %d\n", (int)mdata->year);
            printf(" - aid: %d\n", (int)mdata->aid);
            printf(" - track: %d\n", (int)mdata->track);
            printf(" - album: %s\n", mdata->album);
            printf(" - song: %s\n", mdata->song);
            printf(" - team: %s\n", mdata->team);
            printf(" - creator: %s\n", mdata->creator);
            printf(" - right: %s\n", mdata->right);
            printf(" - code: %s\n", mdata->code);
            puts("");
        }

        /* show song info */
        puts("SONG-DATA:");
        printf("- number of notes = %d\n", vgsdec_get_value(_psg, VGSDEC_REG_LENGTH));
        if (-1 != vgsdec_get_value(_psg, VGSDEC_REG_LOOP_INDEX)) {
            i = vgsdec_get_value(_psg, VGSDEC_REG_LOOP_TIME) / 22050;
            printf("- intro time = %02u:%02u\n", i / 60, i % 60);
        } else {
            printf("- acyclic song\n");
        }
        i = vgsdec_get_value(_psg, VGSDEC_REG_TIME_LENGTH) / 22050;
        printf("- play time = %02u:%02u\n", i / 60, i % 60);
        puts("");

        /* show reference */
        puts("Command Reference:");
        puts("- p            : pause / resume");
        puts("- v rate       : master volume");
        puts("- c ch rate    : channel volume");
        puts("- j{sec|mm:ss} : jump");
        puts("- k{+up|-down} : key change");
        puts("- m[ch]...     : mute channel");
        puts("- f            : fadeout");
        puts("- r            : reload");
        puts("- q            : quit playing");
        puts("");
        show = 1;
    }

    /* main loop */
    memset(buf, 0, sizeof(buf));
    printf("command: ");
    while (NULL != fgets(buf, sizeof(buf) - 1, stdin)) {
        for (i = 0; buf[i]; i++) {
            if ('A' <= buf[i] && buf[i] <= 'Z') {
                buf[i] -= 'a' - 'A';
            }
            if (' ' == buf[i] || '\t' == buf[i]) {
                for (j = i;; j++) {
                    buf[j] = buf[j + 1];
                    if (!buf[j]) break;
                }
            }
        }
        if (buf[0] == 'p') {
            if (isPlaying) {
                vgs2_bstop();
                isPlaying = 0;
            } else {
                vgs2_bresume();
                isPlaying = 1;
            }
        } else if (buf[0] == 'v') {
            vgs2_bmvol(atoi(buf + 1));
        } else if (buf[0] == 'c') {
            if (3 <= strlen(buf)) {
                vgs2_bcvol((int)buf[1] - '0', atoi(buf + 2));
            }
        } else if (buf[0] == 'j') {
            vgs2_bjump(getsec(buf + 1));
        } else if (buf[0] == 'k') {
            if (buf[1] == '+') {
                vgs2_bkey(atoi(&buf[2]));
            } else if (buf[1] == '-') {
                vgs2_bkey(0 - atoi(&buf[2]));
            }
        } else if (buf[0] == 'm') {
            for (i = 1; buf[i]; i++) {
                if ('0' <= buf[i] && buf[i] <= '5') {
                    vgs2_bmute((int)(buf[i] - '0'));
                }
            }
        } else if (buf[0] == 'r') {
            goto RELOAD;
        } else if (buf[0] == 'f') {
            vgs2_bfade2();
        } else if (buf[0] == 'q') {
            break;
        }
        memset(buf, 0, sizeof(buf));
        printf("command: ");
    }

    /* terminate procedure */
    term_sound();
    vgsdec_release_context(_psg);
    return 0;
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
