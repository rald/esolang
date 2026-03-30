#include <stdio.h>
#include <stdlib.h>

#define MEM_MAX 30000
#define STK_MAX 256
#define PRC_MAX 256

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

int main(int argc,char *argv[]) {

	unsigned char m[MEM_MAX];
	char *c=NULL;
	long p[PRC_MAX];
	long s[STK_MAX];

	long nc=0,cp=0,mp=0,scp=0;
	int c0,c1,d=0,i,sp=STK_MAX;

	if(argc!=2) {
		printf("syntax: bf filename\n");
		return -1;
	}

	nc=slurp(argv[1],&c);

	for(i=0;i<PRC_MAX;i++) p[i]=-1;

	while(cp<nc) {
		c0=c[cp];
		switch(c0) {
			case '#': while(cp<nc-1 && c[cp]!='\n') cp++; break;
			case '.': putchar(m[mp]); break;
			case ',': fflush(stdout); m[mp]=(c1=getchar())==EOF?0:c1; break;
			case '+': m[mp]++; break;
			case '-': m[mp]--; break;
			case '<': mp--; if(cp<0) { printf("ERR: past memory min.\n"); return -1; } break;
			case '>': mp++; if(cp>=nc) { printf("ERR: past memory max.\n"); return -1; } break;
			case '[':
				if(m[mp]==0) {
					d=1;
					while(d) {
						if(cp>=nc) { printf("ERR: unmatch '['.\n"); return -1; }
						cp++;
						d-=(c[cp]==']')-(c[cp]=='[');
					}
				}
				break;
			case ']':
				if(m[mp]) {
					d=1;
					while(d) {
						if(cp<0) { printf("ERR: unmatch ']'.\n"); return -1; }
						cp--;
						d-=(c[cp]=='[')-(c[cp]==']');
					}
				}
				break;
			case '(':
				p[m[mp]]=cp;
				d=1;
				while(d) {
					if(cp>=nc) { printf("ERR: unmatch '('.\n"); return -1; }
					cp++;
					d-=(c[cp]==')')-(c[cp]=='(');
				}
				break;
			case ')':
				d=1;
				while(d) {
					if(cp<0) { printf("ERR: unmatch ')'.\n"); return -1; }
					cp--;
					d-=(c[cp]=='(')-(c[cp]==')');
				}
				if(sp>=STK_MAX) { printf("ERR: stack overflow.\n"); return -1; }
				cp=s[sp++];
				break;
			case ':':
				if(sp<=0) { printf("ERR: stack underflow.\n"); return -1; }
				if(p[m[mp]]==-1) { printf("ERR: procedure %d undefined.\n",m[mp]); return -1; }
				s[--sp]=cp;
				cp=p[m[mp]];
				break;
			default: break;
		}
		cp++;
	}

	free(c);

  return 0;
}

