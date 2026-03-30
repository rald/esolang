#include <stdio.h>
#include <stdlib.h>

#define MEM_MAX 30000
#define STK_MAX 256
#define PRC_MAX 256

#define TABSIZE 4

unsigned char m[MEM_MAX];
char *c=NULL;
long p[PRC_MAX];
long s[STK_MAX];

long nc=0,cp=0,mp=0,scp=0;
int c0,c1,d=0,sp=STK_MAX;
long ln,cl;
long i,j,k;

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

void getpos(long cp) {
	i=0;
	ln=1; cl=0;
	while(i<=cp) {
		switch(c[i]) {
			case '\n': ln++; cl=0; break;
			case '\t': cl+=TABSIZE; break;
			default: cl++; break;
		}
		i++;
	}
}

typedef struct {
	char c;
	long p;
} pos_t;

#define MAX 1000
void check() {
	pos_t ps[MAX];
	int pp=MAX;

	i=0;
	while(i<nc) {
		j=c[i];
		if(j=='(' || j=='[') {
			if(pp<=0) { getpos(i); printf("\n%ld:%ld ERR: unmatched '%c'.\n",ln,cl,(char)j); exit(-1); }
			--pp;
			ps[pp].c=j;
			ps[pp].p=i;
		} else if(j==')' || j==']') {
			if(pp==MAX) { getpos(i); printf("\n%ld:%ld ERR: unmatched '%c'.\n",ln,cl,(char)j); exit(-1); }
			pos_t k=ps[pp++];
			if	( (j==')' && k.c!='(') ||
			   	  (j==']' && k.c!='[') ) {
				getpos(k.p); printf("\n%ld:%ld ERR: unmatched '%c'.\n",ln,cl,k.c); exit(-1);
			}
		}
		i++;
	}

	if(pp<MAX) {
		pos_t k=ps[pp++];
		getpos(k.p); printf("\n%ld:%ld ERR: unmatched '%c'.\n",ln,cl,k.c); exit(-1);
	}
}

int main(int argc,char *argv[]) {

	if(argc!=2) {
		printf("syntax: bf filename\n");
		return -1;
	}

	nc=slurp(argv[1],&c);

	check();

	for(i=0;i<PRC_MAX;i++) p[i]=-1;

	while(cp<nc) {
		c0=c[cp];
		switch(c0) {
			case '#': while(cp<nc-1 && c[cp]!='\n') cp++; break;
			case '@': return m[mp]; break;
			case '.': putchar(m[mp]); break;
			case ',': fflush(stdout); m[mp]=(c1=getchar())==EOF?0:c1; break;
			case '+': m[mp]++; break;
			case '-': m[mp]--; break;
			case '<': mp--; if(cp<0) { getpos(cp); printf("\n%ld:%ld ERR: past memory min.\n",ln,cl); return -1; } break;
			case '>': mp++; if(cp>=nc) { getpos(cp); printf("\n%ld:%ld ERR: past memory max.\n",ln,cl); return -1; } break;
			case '[':
				if(!m[mp]) {
					scp=cp;
					d=1;
					while(d) {
						if(cp>=nc) { getpos(scp); printf("\n%ld:%ld ERR: unmatch '['.\n",ln,cl); return -1; }
						cp++;
						d-=(c[cp]==']')-(c[cp]=='[');
					}
				}
				break;
			case ']':
				if(m[mp]) {
					scp=cp;
					d=1;
					while(d) {
						if(cp<0) { getpos(scp); printf("\n%ld:%ld ERR: unmatch ']'.\n",ln,cl); return -1; }
						cp--;
						d-=(c[cp]=='[')-(c[cp]==']');
					}
				}
				break;
			case '(':
				scp=cp;
				d=1;
				while(d) {
					if(cp>=nc) { getpos(scp); printf("\n%ld:%ld ERR: unmatch '('.\n",ln,cl); return -1; }
					cp++;
					d-=(c[cp]==')')-(c[cp]=='(');
				}
				p[m[mp]]=scp;
				break;
			case ')':
				scp=cp;
				d=1;
				while(d) {
					if(cp<0) { getpos(scp); printf("\n%ld:%ld ERR: unmatch ')'.\n",ln,cl); return -1; }
					cp--;
					d-=(c[cp]=='(')-(c[cp]==')');
				}
				if(sp>=STK_MAX) { getpos(scp); printf("\n%ld:%ld ERR: stack overflow.\n",ln,cl); return -1; }
				cp=s[sp++];
				break;
			case ':':
				if(sp<=0) { getpos(cp); printf("\n%ld:%ld ERR: stack underflow.\n",ln,cl); return -1; }
				if(p[m[mp]]==-1) { getpos(cp); printf("\n%ld:%ld ERR: procedure %d undefined.\n",ln,cl,m[mp]); return -1; }
				s[--sp]=cp;
				cp=p[m[mp]];
				break;
			default: break;
		}
		cp++;
	}

	free(c);

	return m[mp];
}

