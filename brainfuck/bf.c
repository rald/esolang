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

long slurp(char *fileName, char **buffer) {
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

void getpos(char *c, long cp, long *ln, long *cl) {
    long i;

    *ln = 1;
    *cl = 0;

    i = 0;
    while (i <= cp && c[i] != '\0') {
        switch (c[i]) {
            case '\n': (*ln)++; *cl = 0; break;
            case '\t': (*cl) += TABSIZE; break;
            default: (*cl)++; break;
        }
        i++;
    }
}

void check(char *c, long nc) {
    long i;
    char j;
    pos_t ps[MAX];
    int pp;
    long ln, cl;
    pos_t k;

    pp = MAX;

    i = 0;
    while (i < nc) {
        j = c[i];
        if (j == '(' || j == '[') {
            if (pp <= 0) {
                getpos(c, i, &ln, &cl);
                printf("\n%ld:%ld ERR: unmatched '%c'.\n", ln, cl, (char)j);
                exit(-1);
            }
            pp--;
            ps[pp].c = j;
            ps[pp].p = i;
        } else if (j == ')' || j == ']') {
            if (pp == MAX) {
                getpos(c, i, &ln, &cl);
                printf("\n%ld:%ld ERR: unmatched '%c'.\n", ln, cl, (char)j);
                exit(-1);
            }
            k = ps[pp++];
            if ((j == ')' && k.c != '(') || (j == ']' && k.c != '[')) {
                getpos(c, k.p, &ln, &cl);
                printf("\n%ld:%ld ERR: unmatched '%c'.\n", ln, cl, k.c);
                exit(-1);
            }
        }
        i++;
    }

    if (pp < MAX) {
        k = ps[pp++];
        getpos(c, k.p, &ln, &cl);
        printf("\n%ld:%ld ERR: unmatched '%c'.\n", ln, cl, k.c);
        exit(-1);
    }
}

int main(int argc, char *argv[]) {
    cell_t m[MEM_MAX];
    char *c;
    long nc, cp, mp, scp, ln, cl;
    int c0, c1, d;
    long i;

    c = NULL;
    nc = 0;
    cp = 0;
    mp = 0;
    scp = 0;
    d = 0;

    for (i = 0; i < MEM_MAX; i++) m[i] = 0;

    if (argc != 2) {
        printf("syntax: bf filename\n");
        return -1;
    }

    nc = slurp(argv[1], &c);
    if (nc < 0) return -1;

    check(c, nc);

    while (cp < nc) {
        c0 = c[cp];
        switch (c0) {
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
                if (!m[mp]) {
                    scp = cp;
                    d = 1;
                    while (d) {
                        if (cp >= nc - 1) {
                            getpos(c, scp, &ln, &cl);
                            printf("\n%ld:%ld ERR: unmatch '['.\n", ln, cl);
                            free(c);
                            return -1;
                        }
                        cp++;
                        if (c[cp] == '[') d++;
                        else if (c[cp] == ']') d--;
                    }
                }
                break;

            case ']':
                if (m[mp]) {
                    scp = cp;
                    d = 1;
                    while (d) {
                        if (cp <= 0) {
                            getpos(c, scp, &ln, &cl);
                            printf("\n%ld:%ld ERR: unmatch ']'.\n", ln, cl);
                            free(c);
                            return -1;
                        }
                        cp--;
                        if (c[cp] == '[') d--;
                        else if (c[cp] == ']') d++;
                    }
                }
                break;

            default:
                break;
        }
        cp++;
    }

    free(c);
    return (int)m[mp];
}
