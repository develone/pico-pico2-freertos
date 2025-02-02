/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "lifting.h"
#include "crc.h"
#include "head-tail.h"

#define DBUG 0
#define DBUG1 0
#define DBUG2 0
#define DBUG3 0
#define DBUG4 1

#define imgsize 4096
//#define imgsize 512
struct PTRs {
	/*This is the buffer for inp & output
	2048 x 2048 = 4194304
	256 x 256 = 65536
	64 x 64 = 4096
	*/
	short int inpbuf[imgsize*2];
	short int *inp_buf;
	short int *out_buf;
	short int flag;
	short int w;
	short int h;
	short int *fwd_inv;
	short int fwd;
	short int *red;
	char *head;
	char *tail;
	char *topofbuf;
	char *endofbuf;
} ptrs;



unsigned char tt[128];
const char src[] = "Hello, world! ";
const short int a[]; 

//const unsigned char CRC7_POLY = 0x91;
unsigned char CRCTable[256];
 

int read_tt(char * head, char * endofbuf,char * topofbuf) {

	int i,numtoread = 64;
	unsigned char CRC;
	 
	//printf("0x%x 0x%x 0x%x \n",ptrs.head,ptrs.endofbuf,ptrs.topofbuf);
	for(i=0;i<numtoread;i++) {
		
		*ptrs.head = getchar();
	 	ptrs.head = (char *)bump_head(ptrs.head,ptrs.endofbuf,ptrs.topofbuf);
	}
	
	CRC = getCRC(tt,numtoread);
	printf("0x%x\n",CRC);
	//for(i=0;i<numtoread;i++) bump_tail(ptrs.head,ptrs.endofbuf,ptrs.topofbuf);
	//for(i=0;i<numtoread;i++) printf("%c",tt[i]);
	
	
	//printf("\n");

	 
	//printf("0x%x 0x%x 0x%x \n",ptrs.head,ptrs.endofbuf,ptrs.topofbuf);
	//printf("CRC = 0x%x\n",CRC);
	
	return(1);
}
unsigned char userInput;

int main() {
	unsigned char recCRC;
	unsigned char message[3] = {0xd3, 0x01, 0x00};
	int flag = 0,numofchars,error=0;
    stdio_init_all();
    int i,j,l,index;
    ptrs.w = 64;
    ptrs.h = 64;
    
    
    ptrs.inp_buf = ptrs.inpbuf;   
    ptrs.head = &tt[0];
	ptrs.tail = &tt[0];
	ptrs.topofbuf = &tt[0];
	
	ptrs.out_buf = ptrs.inpbuf + imgsize;
	ptrs.endofbuf = &tt[128];
	sleep_ms(2000);
	printf("setting pointers\n");
	printf("ptrs.inp_buf = 0x%x ptrs.out_buf = 0x%x\n",ptrs.inpbuf, ptrs.out_buf);
	
	printf("head 0x%x tail 0x%x end 0x%x top 0x%x\n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf);
	
	ptrs.fwd_inv =  &ptrs.fwd;
    *ptrs.fwd_inv = 1;
    
    buildCRCTable();
	message[2] = getCRC(message, 2);
	const uint SERIAL_BAUD = 1000000;
    while (true) {
        if (DBUG == 1 ) {
            printf("Hello, world!\n");
            printf("Now copmpiles with lifting code as part of hello_usb.c\n"); 
            printf("structure PTRS added to hello_usb.c\n");
            printf("ptrs.w = %d ptrs.h = %d \n", ptrs.w, ptrs.h);
            printf("These are the variables needed for lifting\n");
            printf("ptrs.inp_buf = 0x%x ptrs.out_buf = 0x%x\n",ptrs.inp_buf, ptrs.out_buf);
            
            printf("w = %d ptrs.fwd_inv = 0x%x ptrs.fwd_inv = %d\n",ptrs.w,ptrs.fwd_inv, *ptrs.fwd_inv);
            printf("head = 0x%x tail = 0x%x end = 0x%x top = 0x%x\n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf); 
            //for(int i=0;i<25;i++) printf("%d ",a[i]);
            //printf("\n");
            printf("head = 0x%x tail = 0x%x 0x%x 0x%x\n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf); 
            ptrs.head = (char *)bump_head(ptrs.head,ptrs.endofbuf,ptrs.topofbuf);
            ptrs.tail = (char *)bump_tail(ptrs.tail,ptrs.endofbuf,ptrs.topofbuf);
            ptrs.head = (char *)bump_head(ptrs.head,ptrs.endofbuf,ptrs.topofbuf);
            ptrs.tail = (char *)bump_tail(ptrs.tail,ptrs.endofbuf,ptrs.topofbuf);
            ptrs.head = (char *)bump_head(ptrs.head,ptrs.endofbuf,ptrs.topofbuf);
            ptrs.tail = (char *)bump_tail(ptrs.tail,ptrs.endofbuf,ptrs.topofbuf);
			printf("head = 0x%x tail = 0x%x 0x%x 0x%x\n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf); 
            ptrs.head = (char *)dec_head(ptrs.head,ptrs.endofbuf,ptrs.topofbuf);
            ptrs.tail = (char *)dec_tail(ptrs.tail,ptrs.endofbuf,ptrs.topofbuf);
            ptrs.head = (char *)dec_head(ptrs.head,ptrs.endofbuf,ptrs.topofbuf);
            ptrs.tail = (char *)dec_tail(ptrs.tail,ptrs.endofbuf,ptrs.topofbuf);
            ptrs.head = (char *)dec_head(ptrs.head,ptrs.endofbuf,ptrs.topofbuf);
            ptrs.tail = (char *)dec_tail(ptrs.tail,ptrs.endofbuf,ptrs.topofbuf);
            
            printf("head = 0x%x tail = 0x%x 0x%x 0x%x\n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf); 

        } 
        if (DBUG1 == 1) {
			printf("Command (1 = Send or 0 = Wait):\n");
			userInput = getchar();
			
			//for(i = 0; i < imgsize;i++) ptrs.inp_buf[i] = a[i];
			 
			//lifting(ptrs.w,ptrs.inp_buf,ptrs.out_buf,ptrs.fwd_inv);
			
			if(userInput == '1'){
				for(i=0;i<imgsize;i++) printf("%d ",ptrs.inp_buf[i]);
				index = 0;
				for(j=0;j<64;j++) {
					//for(l=0;l<4;l++) {
					//printf("%d\n",l);
					for(i=0;i<64;i++) {
						printf("%d,",ptrs.inp_buf[index]);
						//printf("%d %d %d\n",i,index,index++);
						index++;
					}
					//index = index + 64;
					printf("\n");
					//}
				}
			}
			
		}
		if (DBUG2 == 1) {
			for (i = 0; i < sizeof(message); i++)
			{
				for (j = 0; j < 8; j++)
				printf("%d", (message[i] >> j) % 2);
				printf(" ");
			}
			printf("\n");
			printf("0x%x 0x%x 0x%x 0x%x\n",*ptrs.head,*ptrs.tail,*ptrs.endofbuf,*ptrs.topofbuf);	
		}
	//printf("read 16 values\n");
	

			//printf("\n");

	
		if (DBUG3 == 1) {
			//printf("head = 0x%x tail = 0x%x 0x%x 0x%x\n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf); 
			read_tt(ptrs.head,ptrs.endofbuf,ptrs.topofbuf);	
			//printf("head = 0x%x tail = 0x%x 0x%x 0x%x\n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf);
			for(i=0;i<32;i++) printf("%d ",tt[i]);
			printf("\n");
			for(i=32;i<64;i++) printf("%d ",tt[i]);
			printf("\n");
			
			//numofchars = ptrs.head -ptrs.tail;
			//printf("%d ", numofchars);
			//printf("0x%x 0x%x 0x%x 0x%x 0x%x \n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf,ptrs.inp_buf);
			for(i=0;i<64;i++) {
				*ptrs.inp_buf = (unsigned short int)*ptrs.tail;
				ptrs.inp_buf++;
				ptrs.tail = (char *)bump_tail(ptrs.tail,ptrs.endofbuf,ptrs.topofbuf);
			}
			recCRC = getchar();
			printf("recCRC 0x%x ",recCRC);
			
			printf("0x%x 0x%x 0x%x 0x%x 0x%x \n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf,ptrs.inp_buf);
			
		}
		
		if (DBUG4 == 1) {
			/* Reads 4096 values*/
			
			while (ptrs.inp_buf < ptrs.out_buf) {
				//printf("head = 0x%x tail = 0x%x 0x%x 0x%x\n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf); 
				read_tt(ptrs.head,ptrs.endofbuf,ptrs.topofbuf);	
				//printf("head = 0x%x tail = 0x%x 0x%x 0x%x\n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf);
				for(i=0;i<32;i++) printf("%d ",tt[i]);
				printf("\n");
				for(i=32;i<64;i++) printf("%d ",tt[i]);
				printf("\n");
			
				//numofchars = ptrs.head -ptrs.tail;
				//printf("%d ", numofchars);
				//printf("0x%x 0x%x 0x%x 0x%x 0x%x \n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf,ptrs.inp_buf);
				for(i=0;i<64;i++) {
					*ptrs.inp_buf = (unsigned short int)*ptrs.tail;
					ptrs.inp_buf++;
					ptrs.tail = (char *)bump_tail(ptrs.tail,ptrs.endofbuf,ptrs.topofbuf);
				}
				recCRC = getchar();
				printf("recCRC 0x%x ",recCRC);
			
				printf("0x%x 0x%x 0x%x 0x%x 0x%x \n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf,ptrs.inp_buf);
			}
			ptrs.inp_buf = ptrs.inpbuf;
			/*
			for(i = 0; i < imgsize;i++) {
				if (ptrs.inp_buf[i] == a[i]) {
					//printf("matched %d \n",i);
					error = 0;
				} else {
					//printf("error %d\n",i);
					error = 1;
				}
				
			}
			printf("errors %d \n",error); 
			*/
			printf("Command (1 = Send or 0 = Wait):\n");
			userInput = getchar();
			 
			
			if(userInput == '1'){
				lifting(ptrs.w,ptrs.inp_buf,ptrs.out_buf,ptrs.fwd_inv);
				printf("liftting done \n");
				
				//for(i=0;i<imgsize;i++) printf("%d ",ptrs.inp_buf[i]);
				index = 0;
				for(j=0;j<64;j++) {
					//for(l=0;l<4;l++) {
					//printf("%d\n",l);
					for(i=0;i<64;i++) {
						printf("%d,",ptrs.inp_buf[index]);
						//printf("%d %d %d\n",i,index,index++);
						index++;
					}
					//index = index + 64;
					printf("\n");
					//}
				}
			} 
		}
        //sleep_ms(8000);
        sleep_ms(50);
    }
    return 0;
}
