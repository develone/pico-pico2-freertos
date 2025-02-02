#include <stdio.h>
#include <stdlib.h>
#include "crc.h"
   

void main(int argc, char *argv[]) {
FILE *ptr;
char *fn;
int ii;
fn=argv[1];
printf("%s\n",fn);
buildCRCTable();
unsigned char rcrc,ccrc,msg[64],buf[4160];
ptr = fopen(fn,"r");
if (ptr!=0) {
    fread(buf,sizeof(char),4160,ptr);
   
    for(int rr=0;rr<64;rr++) {
    	for(int cc=0;cc<65;cc++) {
	  msg[cc]=buf[ii];
	  printf("%d ",buf[ii]);
	  ii++;
        }
    	printf("\n");
	rcrc=msg[64];
        
	msg[64]=0;
	ccrc=0;
        ccrc=getCRC(msg,64);
        if (rcrc != ccrc) printf("%d %d\n",rcrc,ccrc);
     }
     
     fclose(ptr);
}

}