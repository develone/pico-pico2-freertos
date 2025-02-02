#include <FreeRTOS.h>
#include <task.h>
#include <stream_buffer.h>
#include <stdio.h>
#include <queue.h>
#include <string.h>
#include "pico/stdlib.h"
#include "semphr.h"
#include "event_groups.h"
#include "lifting.h"
#include "crc16.h"
#include "head-tail.h"
#include "klt.h"
#include "pico_hal.h"
/*adding pshell */
#include "xreceive.h"
#include "xtransmit.h"
#include "hardware/watchdog.h"

#include "pico/stdio.h"
#include "pico/stdlib.h"

#include "lfs.h"
#include "lfs_util.h"
#include "tusb.h"
#include "vi.h"
#include "pshell.h"
/*adding pshell */

/***********************needs to be in a header***********************/
#define STORAGE_SIZE_BYTES 100

#define TASK1_BIT  (1UL << 0UL)	//zero
#define TASK2_BIT  (1UL << 1UL)	//1
#define TASK3_BIT  (1UL << 2UL)	//2
#define TASK4_BIT  (1UL << 3UL)	//3
#define TASK5_BIT  (1UL << 4UL)	//4
#define TASK6_BIT  (1UL << 5UL)	//5

/*Used to dimension the array used to hold the streams.
The availble space is 1 less than this */
static uint8_t ucBufferStorage[STORAGE_SIZE_BYTES];

/*The varaible used to hold the stream buffer structure*/
StaticStreamBuffer_t xStreamBufferStruct;

//StreamBufferHandle_t xStreamBuffer;
StreamBufferHandle_t DynxStreamBuffer;

//const size_t xStreamBufferSizeBytes = 100,xTriggerLevel = 10;
//xStreamBuffer = xStreamBufferCreate(xStreamBufferSizeBytes,xTriggerLevel);
static QueueHandle_t xQueue = NULL;

int streamFlag, ii, received, processed, j, m;
size_t numbytes1;
size_t numbytes2;
uint8_t *pucRXData;
size_t rdnumbytes1;
size_t Event = 0;

static SemaphoreHandle_t mutex;
//static SemaphoreHandle_t mutex1;
//static SemaphoreHandle_t mutex2;

//EventGroupHandle_t xEventGroupCreate( void);
		/*Declare a variables to hold the created event groups
		   #define configUSE_16_BIT_TICKS                  0
		   This means the the number of bits(or flags) implemented
		   within an event group is 24                  
		 */

EventGroupHandle_t xCreatedEventGroup;

// define a variable which holds the state of events 
const EventBits_t xBitsToWaitFor =
  (TASK1_BIT | TASK2_BIT | TASK3_BIT | TASK4_BIT);
EventBits_t xEventGroupValue;


#define imgsize 4096
//#define imgsize 512
struct PTRs
{
  /*This is the buffer for inp & output
     2048 x 2048 = 4194304
     256 x 256 = 65536
     64 x 64 = 4096
   */
  short int inpbuf[imgsize * 2];
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
int nFeatures = 100;
KLT_TrackingContext tc;
KLT_FeatureList fl;
//const unsigned char CRC7_POLY = 0x91;
unsigned char CRCTable[256];


int
read_tt (char *head, char *endofbuf, char *topofbuf)
{

  int i, numtoread = 64;
  unsigned char CRC;

  //printf("0x%x 0x%x 0x%x \n",ptrs.head,ptrs.endofbuf,ptrs.topofbuf);
  for (i = 0; i < numtoread; i++)
    {

      *ptrs.head = getchar ();
      ptrs.head =
	(char *) bump_head (ptrs.head, ptrs.endofbuf, ptrs.topofbuf);
    }

  CRC = crc16_ccitt (tt, numtoread);
  //printf("0x%x\n",CRC);
  //for(i=0;i<numtoread;i++) bump_tail(ptrs.head,ptrs.endofbuf,ptrs.topofbuf);
  //for(i=0;i<numtoread;i++) printf("%c",tt[i]);


  //printf("\n");


  //printf("0x%x 0x%x 0x%x \n",ptrs.head,ptrs.endofbuf,ptrs.topofbuf);
  //printf("CRC = 0x%x\n",CRC);

  return (1);
}

unsigned char userInput;

unsigned char recCRC;
unsigned char message[3] = { 0xd3, 0x01, 0x00 };

int flag = 0, numofchars, error = 0, syncflag = 1, rdyflag = 1, testsx =
  10, testsx1 = 10, syncdone = 0;
int ncols, nrows, i;
unsigned char inpbuf[imgsize * 2];
unsigned char *img1, *img2;
/***********************needs to be in a header***********************/

void
led_task (void *pvParameters)
{
  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  uint uIValueToSend = 0;
  gpio_init (LED_PIN);
  gpio_set_dir (LED_PIN, GPIO_OUT);

  while (true)
    {
      // set flag bit TASK1_BIT
      xEventGroupSetBits (xCreatedEventGroup, TASK1_BIT);
      gpio_put (LED_PIN, 1);
      uIValueToSend = 1;
      xQueueSend (xQueue, &uIValueToSend, 0U);
      vTaskDelay (5000);


      gpio_put (LED_PIN, 0);
      uIValueToSend = 0;
      xQueueSend (xQueue, &uIValueToSend, 0U);
      vTaskDelay (5000);
    }
}

void
scanline_task (void *pvParameters)
{
  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  uint uIValueToSend = 0;
  gpio_init (LED_PIN);
  gpio_set_dir (LED_PIN, GPIO_OUT);

  while (true)
    {
      gpio_put (LED_PIN, 1);
      uIValueToSend = 1;
      xQueueSend (xQueue, &uIValueToSend, 0U);
      vTaskDelay (5000);


      gpio_put (LED_PIN, 0);
      uIValueToSend = 0;
      xQueueSend (xQueue, &uIValueToSend, 0U);
      vTaskDelay (5000);
    }
}

void
usb_task (void *pvParameters)
{
  uint uIReceivedValue;

  while (1)
    {
      xQueueReceive (xQueue, &uIReceivedValue, portMAX_DELAY);
      // set flag bit TASK2_BIT
      xEventGroupSetBits (xCreatedEventGroup, TASK2_BIT);
      if (uIReceivedValue == 1)
	{
	  //printf("LED is ON! \n");
	}
      if (uIReceivedValue == 0)
	{
	  //printf("Sync%d\n",testsx);
	  //if(syncdone==1) printf("Ready%d\n",testsx1);
	  /*            
	     printf("LED is OFF! streamFlag=%d DynStreamBuffer=0x%x \n",streamFlag, DynxStreamBuffer);
	     printf("numbytes1=%d numbytes2=%d\n",numbytes1,numbytes2);
	     printf("rdnumbytes1=%d Event=%d\n",rdnumbytes1,Event);
	     printf("EGroup=0x%x \n",xCreatedEventGroup);

	     if(rdnumbytes1> 0)
	     for(ii=0;ii<rdnumbytes1;ii++) printf("%c ",pucRXData[ii]);
	     printf("\n");
	   */
	  xEventGroupValue = xEventGroupWaitBits (xCreatedEventGroup,
						  xBitsToWaitFor,
						  pdTRUE,
						  pdTRUE, portMAX_DELAY);
	  if ((xEventGroupValue & TASK1_BIT != 0))
	    {
	      //printf("sync event occured\n");
	    }
	  if ((xEventGroupValue & TASK2_BIT != 0))
	    {
	      //printf("ready event occured\n");
	    }

	  if ((xEventGroupValue & TASK3_BIT != 0))
	    {
	      //printf("Task3 event occured\n");
	    }
	  if ((xEventGroupValue & TASK4_BIT != 0))
	    {
	      //printf("Task4 event occured\n");
	    }
	  if ((xEventGroupValue & TASK5_BIT != 0))
	    {
	      //printf("Task5 event occured received = %d\n",received);
	    }
	  if ((xEventGroupValue & TASK6_BIT != 0))
	    {
	      //printf("Task6 event occured processed = %d\n",processed);

	    }

	}
      vTaskDelay (35000);

    }

}


void
sync (void *pvParameters)
{

  while (true)
    {
      // set flag bit TASK3_BIT
      xEventGroupSetBits (xCreatedEventGroup, TASK3_BIT);
      //if(xSemaphoreTake(mutex1, 0) == pdTRUE){
      while (testsx)
	{
	  printf ("Sync\n");
	  sleep_ms (1400);
	  //vTaskDelay(1400);
	  testsx--;
	  if (testsx == 0)
	    syncdone = 1;
	  /*
	     if(syncdone==1) {
	     while (testsx1) {            
	     //printf("Ready\n");
	     sleep_ms(1400);
	     testsx1--;
	     }
	     }
	   */
	}

      //xSemaphoreGive(mutex1);
      vTaskDelay (5000);
      //}

    }
}

void
ready (void *pvParameters)
{
  while (true)
    {
      //if(syncdone == 1) {
      // set flag bit TASK4_BIT
      xEventGroupSetBits (xCreatedEventGroup, TASK4_BIT);
      //if(xSemaphoreTake(mutex2, 0) == pdTRUE){
      while (testsx1)
	{
	  printf ("Ready\n");
	  sleep_ms (1400);
	  testsx1--;
	}
      //xSemaphoreGive(mutex2);
      vTaskDelay (5000);
      //}

      //}
    }
}

void
read (void *pvParameters)
{
  while (true)
    {
      // set flag bit TASK5_BIT 
      xEventGroupSetBits (xCreatedEventGroup, TASK5_BIT);
      if (xSemaphoreTake (mutex, 0) == pdTRUE)
	{
	  while (ptrs.inp_buf < ptrs.out_buf)
	    {




	      //printf("head = 0x%x tail = 0x%x 0x%x 0x%x\n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf); 
	      read_tt (ptrs.head, ptrs.endofbuf, ptrs.topofbuf);
	      //printf("head = 0x%x tail = 0x%x 0x%x 0x%x\n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf);
	      //for(i=0;i<32;i++) printf("%d ",tt[i]);
	      //printf("\n");
	      //for(i=32;i<64;i++) printf("%d ",tt[i]);
	      //printf("\n");

	      //numofchars = ptrs.head -ptrs.tail;
	      //printf("%d ", numofchars);
	      //printf("0x%x 0x%x 0x%x 0x%x 0x%x \n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf,ptrs.inp_buf);
	      for (i = 0; i < 64; i++)
		{
		  *ptrs.inp_buf = (unsigned short int) *ptrs.tail;
		  ptrs.inp_buf++;
		  ptrs.tail =
		    (char *) bump_tail (ptrs.tail, ptrs.endofbuf,
					ptrs.topofbuf);
		}
	      recCRC = getchar ();
	      //printf("recCRC 0x%x ",recCRC);
	      received = 1;




	    }
	  //printf("0x%x 0x%x 0x%x 0x%x 0x%x \n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf,ptrs.inp_buf);
	  xSemaphoreGive (mutex);
	  vTaskDelay (5000);


	}
    }
}

void pshell(void *pvParameters)
{
	while(true) {
		//printf("This is a place holder for the pshell task\n");
    while (run) {
        printf("%s: ", full_path(""));
        fflush(stdout);
        parse_cmd();
        bool found = false;
        int i;
        result[0] = 0;
        if (argc)
            for (i = 0; i < sizeof cmd_table / sizeof cmd_table[0]; i++)
                if (strcmp(argv[0], cmd_table[i].name) == 0) {
                    cmd_table[i].func();
										printf("%d %s %s\n",i ,cmd_table[i].name, cmd_table[i].descr);
                    found = true;
                    break;
                }
        if (!found) {
            if (argc)
                printf("command unknown!\n\n");
            for (int i = 0; i < sizeof cmd_table / sizeof cmd_table[0]; i++)
                printf("%7s - %s\n", cmd_table[i].name, cmd_table[i].descr);
            printf("\n");
            continue;
        }
        printf("%s\n", result);
    }
		vTaskDelay (35000); 
	}
}
void debug(void *pvParameters)
{
	unsigned char message[9] = {0xd3, 0x01, 0x00,0xd3, 0x01, 0x00,0xd3, 0x01, 0x10};
  uint16_t crc;
  while (true) {
   
		printf("Hello, world!\n");
    crc = crc16_ccitt(message, 9);
    printf("0x%x\n",crc);
    vTaskDelay (35000);
   
  }
}

void
processliftklt (void *pvParameters)
{
  while (true)
    {
      while (testsx)
	{
	  printf ("Sync\n");
	  sleep_ms (1400);
	  testsx--;
	}
      //testsx = 10;
      while (testsx1)
	{
	  printf ("Ready\n");
	  sleep_ms (100);
	  testsx1--;

	}
      //userInput = getchar();
      while (ptrs.inp_buf < ptrs.out_buf)
	{




	  //printf("head = 0x%x tail = 0x%x 0x%x 0x%x\n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf); 
	  read_tt (ptrs.head, ptrs.endofbuf, ptrs.topofbuf);
	  //printf("head = 0x%x tail = 0x%x 0x%x 0x%x\n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf);
	  //for(i=0;i<32;i++) printf("%d ",tt[i]);
	  //printf("\n");
	  //for(i=32;i<64;i++) printf("%d ",tt[i]);
	  //printf("\n");

	  //numofchars = ptrs.head -ptrs.tail;
	  //printf("%d ", numofchars);
	  //printf("0x%x 0x%x 0x%x 0x%x 0x%x \n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf,ptrs.inp_buf);
	  for (i = 0; i < 64; i++)
	    {
	      *ptrs.inp_buf = (unsigned short int) *ptrs.tail;
	      ptrs.inp_buf++;
	      ptrs.tail =
		(char *) bump_tail (ptrs.tail, ptrs.endofbuf, ptrs.topofbuf);
	    }
	  recCRC = getchar ();
	  //printf("recCRC 0x%x ",recCRC);

	  //printf("0x%x 0x%x 0x%x 0x%x 0x%x \n",ptrs.head,ptrs.tail,ptrs.endofbuf,ptrs.topofbuf,ptrs.inp_buf);
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
         //printf("errors %d \n",error);

       */
      printf ("Command (1 = Send or 0 = Wait):\n");
      userInput = getchar ();


      if (userInput == '1')
	{
	  printf ("need to copy the data received from host to img1\n");
	  printf ("img1 = 0x%x img2 = 0x%x\n", img1, img2);
	  for (i = 0; i < ncols * nrows; i++)
	    {
	      img1[i] = ptrs.inp_buf[i];
	      //img2[i+4095] = img1[i]; 
	      if (i < 5)
		printf ("%d img1 %d ptrs.buf %d \n", i, img1[i],
			ptrs.inp_buf[i]);
	      if (i > 4090)
		printf ("%d img1 %d ptrs.buf %d \n", i, img1[i],
			ptrs.inp_buf[i]);
	    }
	  printf ("need to copy the data from img1 to img2\n");
	  for (i = 0; i < ncols * nrows; i++)
	    {
	      *img2 = *img1;
	      if (i < 5)
		printf ("%d img2 %d img1 %d \n", i, *img2, *img1);
	      if (i > 4090)
		printf ("%d img2 %d img1 %d \n", i, *img2, *img1);
	      img2++;
	      img1++;
	    }
	  img1 = &inpbuf[0];
	  img2 = &inpbuf[4096];
	  //printf("img1 = 0x%x img2 = 0x%x\n",img1, img2);

	  KLTSelectGoodFeatures (tc, img1, ncols, nrows, fl);

	  //printf("\nIn first image:\n");
	  for (i = 0; i < fl->nFeatures; i++)
	    {
	      printf ("Feature #%d:  (%f,%f) with value of %d\n",
		      i, fl->feature[i]->x, fl->feature[i]->y,
		      fl->feature[i]->val);
	    }
	  lifting (ptrs.w, ptrs.inp_buf, ptrs.out_buf, ptrs.fwd_inv);
	  //printf("liftting done \n");

	  //for(i=0;i<imgsize;i++) printf("%d ",ptrs.inp_buf[i]);
	  m = 0;
	  for (j = 0; j < 64; j++)
	    {
	      //for(l=0;l<4;l++) {
	      //printf("%d\n",l);
	      for (i = 0; i < 64; i++)
		{
		  printf ("%d,", ptrs.inp_buf[m]);
		  //printf("%d %d %d\n",i,m,m++);
		  m++;
		}
	      //m = m + 64;
	      printf ("\n");
	      //}
	    }
	}
    }
  syncflag = 1;
  //sleep_ms(8000);
  sleep_ms (50);
  // } 

  // }
}

/*Tries to create a StreamBuffer of 100 bytes and blocks after 10*/
void
vAFunction (void)
{
  StreamBufferHandle_t xStreamBuffer;
  const size_t xStreamBufferSizeBytes = 100, xTriggerLevel = 10;
  xStreamBuffer = xStreamBufferCreate (xStreamBufferSizeBytes, xTriggerLevel);

  if (xStreamBuffer == NULL)
    {
      streamFlag = 0;
    }
  else
    {
      streamFlag = 1;
      DynxStreamBuffer = xStreamBuffer;
    }
}


void
MyFunction (void)
{
  StreamBufferHandle_t xStreamBuffer;
  const size_t xTriggerLevel = 1;
  xStreamBuffer = xStreamBufferCreateStatic (sizeof (ucBufferStorage),
					     xTriggerLevel,
					     ucBufferStorage,
					     &xStreamBufferStruct);
}


void
vASendStream (StreamBufferHandle_t DynxStreamBuffer)
{
  size_t xByteSent;
  uint8_t ucArrayToSend = (0, 1, 2, 3);

  /*numbytes2 29 rdnumbytes1 30
     if the string is uncommneted */
  char *pcStringToSend = "String To Send String To Send";

  /*numbytes2 14 rdnumbytes1 15
     if the string is uncommneted */
  //char *pcStringToSend ="String To Send";

  const TickType_t x100ms = pdMS_TO_TICKS (100);

  xByteSent = xStreamBufferSend (DynxStreamBuffer, (void *) ucArrayToSend,
				 sizeof (ucArrayToSend), x100ms);
  numbytes1 = xByteSent;

  if (xByteSent != sizeof (ucArrayToSend))
    {

    }
  xByteSent = xStreamBufferSend (DynxStreamBuffer, (void *) pcStringToSend,
				 strlen (pcStringToSend), 0);
  numbytes2 = strlen (pcStringToSend);

  if (xByteSent != strlen (pcStringToSend))
    {

    }
}


void
vAReadStream (StreamBufferHandle_t xStreamBuffer)
{
  int i;
  uint8_t ucRXData[40];
  size_t xRecivedBytes;
  const TickType_t xBlockTime = pdMS_TO_TICKS (20);
  pucRXData = &ucRXData;
  xRecivedBytes = xStreamBufferReceive (DynxStreamBuffer,
					(void *) ucRXData, sizeof (ucRXData),
					xBlockTime);
  rdnumbytes1 = xRecivedBytes;
  i = 0;
  if (xRecivedBytes > 0)
    {
      printf ("%d ", ucRXData[i]);
      i++;
    }
}

int
main ()
{
     
  stdio_init_all ();

/*adding pshell */
		bool uart = stdio_init(PICO_DEFAULT_UART_RX_PIN);
    bool detected = screen_size();
    printf(VT_CLEAR "\n"
                    "Pico Shell - Copyright (C) 1883 Thomas Edison\n"
                    "This program comes with ABSOLUTELY NO WARRANTY.\n"
                    "This is free software, and you are welcome to redistribute it\n"
                    "under certain conditions. See LICENSE file for details.\n\n"
                    "console on %s (%s %u rows, %u columns)\n\n"
                    "enter command, hit return for help\n\n",
           uart ? "UART" : "USB", detected ? "detected" : "defaulted to", screen_y, screen_x);
/*adding pshell */
  xQueue = xQueueCreate (1, sizeof (uint));
  mutex = xSemaphoreCreateMutex ();

  //mutex1 = xSemaphoreCreateMutex();

  //mutex2 = xSemaphoreCreateMutex();

  /*Attempt to create the event groups */
  xCreatedEventGroup = xEventGroupCreate ();


  /*Need to test if the Event Group was created */
    /**************************/

  if (xCreatedEventGroup == NULL)
    {
      /*The event group was not created */
    }
  else
    {
      /*The event group was created */
      Event = 1;
    }
  ptrs.w = 64;
  ptrs.h = 64;

  ncols = 64;
  nrows = 64;

  tc = KLTCreateTrackingContext ();
  //printf("tc 0x%x\n",tc);
  fl = KLTCreateFeatureList (nFeatures);
  //KLTPrintTrackingContext(tc);     
  ptrs.inp_buf = ptrs.inpbuf;
  ptrs.head = &tt[0];
  ptrs.tail = &tt[0];
  ptrs.topofbuf = &tt[0];

  ptrs.out_buf = ptrs.inpbuf + imgsize;
  ptrs.endofbuf = &tt[128];
  img1 = &inpbuf[0];
  img2 = &inpbuf[4096];
  ptrs.fwd_inv = &ptrs.fwd;
  *ptrs.fwd_inv = 1;

  //buildCRCTable ();
  message[2] = crc16_ccitt (message, 2);
  ptrs.fwd_inv = &ptrs.fwd;
  *ptrs.fwd_inv = 1;

  //buildCRCTable ();
  message[2] = crc16_ccitt (message, 2);
  received = 0;
  processed = 0;
  /*Need to test if the Event Group was created */
    /**************************/
  const uint SERIAL_BAUD = 1000000;
  /*Setting the streamFlag to 0 before the call of vAFunction
     if the stream was successful the streamFlag will be set to 1         
   */
  streamFlag = 0;
  vAFunction ();
  vASendStream (DynxStreamBuffer);
  vAReadStream (DynxStreamBuffer);
  //MyFunction();


  xTaskCreate (led_task, "LED_Task", 256, NULL, 2, NULL);
  xTaskCreate (usb_task, "USB_Task", 256, NULL, 1, NULL);
  //xTaskCreate (sync, "Task 1", 256, NULL, 5, NULL);
  //xTaskCreate (ready, "Task 2", 256, NULL, 3, NULL);
  //xTaskCreate (read, "Task 3", 256, NULL, 1, NULL);
  //xTaskCreate (processliftklt, "Task 4", 256, NULL, 2, NULL);
  //xTaskCreate (debug, "Task 4", 256, NULL, 4, NULL);
  xTaskCreate (pshell, "Task 5", 256, NULL, 2, NULL);
  vTaskStartScheduler ();


  while (1)
    {
    };
}
