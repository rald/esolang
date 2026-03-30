#include <stdio.h>
#include <stdlib.h>

#define MEM_MAX 30000
#define STK_MAX 256
#define PRC_MAX 256

#define TABSIZE 4

long slurp(char *fileName,char **buffer) {
	FILE *fp;
	long fileSize;

	fp=fopen(fileName,"rb");

	if(fp==NULL) {
		printf("Error opening file.\n");
		return -1;
	}

	fseek(fp,0,SEEK_END);
	fileSize=ftell(fp);
	rewind(fp);

	*buffer=malloc(fileSize);

	if(*buffer==NULL) {
		printf("Error allocating memory.\n");
		return -1;
	}

	fread(*buffer,1,fileSize,fp);

	fclose(fp);

	return fileSize;
}

void getpos(char *c,long cp,long *ln,long *cl) {
	long i=0;
	(*ln)=1; (*cl)=0;
	while(i<=cp) {
		switch(c[i]) {
			case '\n': (*ln)++; (*cl)=0; break;
			case '\t': (*cl)+=TABSIZE; break;
			default: (*cl)++; break;
		}
		i++;
	}
}

typedef struct {
	char c;
	long p;
} pos_t;

#define MAX 1000
void check(char *c,long nc) {
	long i=0;
	char j;
	pos_t ps[MAX];
	int pp=MAX;
	long ln,cl;
	pos_t k;

	while(i<nc) {
		j=c[i];
		if(j=='(' || j=='[') {
			if(pp<=0) { getpos(c,i,&ln,&cl); printf("\n%ld:%ld ERR: unmatched '%c'.\n",ln,cl,(char)j); exit(-1); }
			--pp;
			ps[pp].c=j;
			ps[pp].p=i;
		} else if(j==')' || j==']') {
			if(pp==MAX) { getpos(c,i,&ln,&cl); printf("\n%ld:%ld ERR: unmatched '%c'.\n",ln,cl,(char)j); exit(-1); }
			k=ps[pp++];
			if	( (j==')' && k.c!='(') ||
			   	  (j==']' && k.c!='[') ) {
				getpos(c,k.p,&ln,&cl); printf("\n%ld:%ld ERR: unmatched '%c'.\n",ln,cl,k.c); exit(-1);
			}
		}
		i++;
	}

	if(pp<MAX) {
		pos_t k=ps[pp++];
		getpos(c,k.p,&ln,&cl); printf("\n%ld:%ld ERR: unmatched '%c'.\n",ln,cl,k.c); exit(-1);
	}
}

int main(int argc,char *argv[]) {
	unsigned char m[MEM_MAX];
	char *c=NULL;

	long nc=0,cp=0,mp=0,scp=0;
	int c0,c1,d=0;
	long ln=0,cl=0;

	if(argc!=2) {
		printf("syntax: bf filename\n");
		return -1;
	}

	nc=slurp(argv[1],&c);

	check(c,nc);

	while(cp<nc) {
		c0=c[cp];
		switch(c0) {
			case '@': return m[mp]; break;
			case '#': while(cp<nc-1 && c[cp]!='\n') cp++; break;
			case '.': putchar(m[mp]); break;
			case ',': fflush(stdout); m[mp]=(c1=getchar())==EOF?0:c1; break;
			case '+': m[mp]++; break;
			case '-': m[mp]--; break;
			case '<': mp--; if(cp<0) { getpos(c,cp,&ln,&cl); printf("\n%ld:%ld ERR: past memory min.\n",ln,cl); return -1;} break;
			case '>': mp++; if(cp>=nc) { getpos(c,cp,&ln,&cl); printf("\n%ld:%ld ERR: past memory max.\n",ln,cl); return -1; } break;
			case '[':
				if(!m[mp]) {
					scp=cp;
					d=1;
					while(d) {
						if(cp>=nc) { getpos(c,scp,&ln,&cl); printf("\n%ld:%ld ERR: unmatch '['.\n",ln,cl); return -1; }
						cp++;
						d-=(c[cp]==']')-(c[cp]=='[');
					}
				}
				break;
			case ']':
				if(m[mp]) {
					d=1;
					while(d) {
						if(cp<0) { getpos(c,scp,&ln,&cl); printf("\n%ld:%ld ERR: unmatch ']'.\n",ln,cl); return -1; }
						cp--;
						d-=(c[cp]=='[')-(c[cp]==']');
					}
				}
				break;
			default: break;
		}
		cp++;
	}

	free(c);

	return m[mp];
}

