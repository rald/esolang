#include <stdio.h>
#include <stdlib.h>

#define TABSIZE 8
#define MEM_MAX 30000
#define STK_MAX 256
#define PRC_MAX 256
#define MAX 1000

typedef unsigned char cell_t;

typedef struct {
    char c;
    long p;
} pos_t;

static long slurp(char *fileName, char **buffer) {
    FILE *fp;
    long fileSize;

    fp = fopen(fileName, "rb");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return -1;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return -1;
    }

    fileSize = ftell(fp);
    if (fileSize < 0) {
        fclose(fp);
        return -1;
    }

    rewind(fp);
    *buffer = (char *)malloc((size_t)fileSize + 1);
    if (*buffer == NULL) {
        printf("Error allocating memory.\n");
        fclose(fp);
        return -1;
    }

    if (fread(*buffer, 1, (size_t)fileSize, fp) != (size_t)fileSize) {
        printf("Error reading file.\n");
        free(*buffer);
        fclose(fp);
        return -1;
    }

    (*buffer)[fileSize] = '\0';
    fclose(fp);
    return fileSize;
}

static void getpos(char *c, long cp, long *ln, long *cl) {
    long i;
    *ln = 1;
    *cl = 0;
    for (i = 0; i <= cp && c[i] != '\0'; i++) {
        switch (c[i]) {
            case '\n': (*ln)++; *cl = 0; break;
            case '\t': (*cl) += TABSIZE; break;
            default: (*cl)++; break;
        }
    }
}

static void check(char *c, long nc) {
    pos_t ps[MAX];
    int pp;
    long i, ln, cl;
    pos_t k;

    pp = 0;
    for (i = 0; i < nc; i++) {
        if (c[i] == '(' || c[i] == '[') {
            if (pp >= MAX) {
                getpos(c, i, &ln, &cl);
                printf("\n%ld:%ld ERR: too many nested brackets.\n", ln, cl);
                exit(-1);
            }
            ps[pp].c = c[i];
            ps[pp].p = i;
            pp++;
        } else if (c[i] == ')' || c[i] == ']') {
            if (pp <= 0) {
                getpos(c, i, &ln, &cl);
                printf("\n%ld:%ld ERR: unmatched '%c'.\n", ln, cl, c[i]);
                exit(-1);
            }
            pp--;
            k = ps[pp];
            if ((c[i] == ')' && k.c != '(') || (c[i] == ']' && k.c != '[')) {
                getpos(c, k.p, &ln, &cl);
                printf("\n%ld:%ld ERR: unmatched '%c'.\n", ln, cl, k.c);
                exit(-1);
            }
        }
    }

    if (pp > 0) {
        k = ps[pp - 1];
        getpos(c, k.p, &ln, &cl);
        printf("\n%ld:%ld ERR: unmatched '%c'.\n", ln, cl, k.c);
        exit(-1);
    }
}

int main(int argc, char *argv[]) {
    cell_t m[MEM_MAX];
    char *c;
    long p[PRC_MAX];
    long s[STK_MAX];
    long nc, cp, mp, scp, i, ln, cl;
    int c1, d, sp;

    c = NULL;
    cp = 0;
    mp = 0;
    scp = 0;
    d = 0;
    sp = STK_MAX;

    for (i = 0; i < MEM_MAX; i++) m[i] = 0;
    for (i = 0; i < PRC_MAX; i++) p[i] = -1;

    if (argc != 2) {
        printf("syntax: bf filename\n");
        return -1;
    }

    nc = slurp(argv[1], &c);
    if (nc < 0) return -1;

    check(c, nc);

    while (cp < nc) {
        switch (c[cp]) {
            case '@':
                free(c);
                return (int)m[mp];

            case '#':
                while (cp < nc && c[cp] != '\n') cp++;
                break;

            case '.':
                putchar((int)m[mp]);
                break;

            case ',':
                fflush(stdout);
                c1 = getchar();
                m[mp] = (c1 == EOF) ? 0 : (cell_t)c1;
                break;

            case '+':
                m[mp]++;
                break;

            case '-':
                m[mp]--;
                break;

            case '<':
                if (mp <= 0) {
                    getpos(c, cp, &ln, &cl);
                    printf("\n%ld:%ld ERR: past memory min.\n", ln, cl);
                    free(c);
                    return -1;
                }
                mp--;
                break;

            case '>':
                if (mp >= MEM_MAX - 1) {
                    getpos(c, cp, &ln, &cl);
                    printf("\n%ld:%ld ERR: past memory max.\n", ln, cl);
                    free(c);
                    return -1;
                }
                mp++;
                break;

            case '[':
                if (m[mp] == 0) {
                    scp = cp;
                    d = 1;
                    while (d > 0) {
                        cp++;
                        if (cp >= nc) {
                            getpos(c, scp, &ln, &cl);
                            printf("\n%ld:%ld ERR: unmatch '['.\n", ln, cl);
                            free(c);
                            return -1;
                        }
                        if (c[cp] == '[') d++;
                        else if (c[cp] == ']') d--;
                    }
                }
                break;

            case ']':
                if (m[mp] != 0) {
                    scp = cp;
                    d = 1;
                    while (d > 0) {
                        if (cp <= 0) {
                            getpos(c, scp, &ln, &cl);
                            printf("\n%ld:%ld ERR: unmatch ']'.\n", ln, cl);
                            free(c);
                            return -1;
                        }
                        cp--;
                        if (c[cp] == ']') d++;
                        else if (c[cp] == '[') d--;
                    }
                }
                break;

            case '(':
                scp = cp;
                d = 1;
                while (d > 0) {
                    cp++;
                    if (cp >= nc) {
                        getpos(c, scp, &ln, &cl);
                        printf("\n%ld:%ld ERR: unmatch '('.\n", ln, cl);
                        free(c);
                        return -1;
                    }
                    if (c[cp] == '(') d++;
                    else if (c[cp] == ')') d--;
                }
                p[m[mp]] = scp;
                break;

            case ')':
                scp = cp;
                d = 1;
                while (d > 0) {
                    if (cp <= 0) {
                        getpos(c, scp, &ln, &cl);
                        printf("\n%ld:%ld ERR: unmatch ')'.\n", ln, cl);
                        free(c);
                        return -1;
                    }
                    cp--;
                    if (c[cp] == ')') d++;
                    else if (c[cp] == '(') d--;
                }
                if (sp >= STK_MAX) {
                    getpos(c, scp, &ln, &cl);
                    printf("\n%ld:%ld ERR: stack overflow.\n", ln, cl);
                    free(c);
                    return -1;
                }
                cp = s[sp++];
                break;

            case ':':
                if (sp <= 0) {
                    getpos(c, cp, &ln, &cl);
                    printf("\n%ld:%ld ERR: stack underflow.\n", ln, cl);
                    free(c);
                    return -1;
                }
                if (p[m[mp]] == -1) {
                    getpos(c, cp, &ln, &cl);
                    printf("\n%ld:%ld ERR: procedure %d undefined.\n", ln, cl, (int)m[mp]);
                    free(c);
                    return -1;
                }
                s[--sp] = cp;
                cp = p[m[mp]];
                break;

            default:
                break;
        }
        cp++;
    }

    free(c);
    return (int)m[mp];
}
