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

	long nc=0,cp=0,mp=0;
	int c0,c1,d=0,i;

	if(argc!=2) {
		printf("syntax: bf filename\n");
		return -1;
	}

	nc=slurp(argv[1],&c);

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
				if(!m[mp]) {
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
			default: break;
		}
		cp++;
	}

	free(c);

  return 0;
}

