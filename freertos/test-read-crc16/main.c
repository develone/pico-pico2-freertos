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
#include "crc.h"
#include "head-tail.h"
#include "klt.h"
#include "comprogs.h"
/*adding pshell */
#include "xreceive.h"
#include "xtransmit.h"
#include "hardware/watchdog.h"

#include "pico/stdio.h"
#include "pico/stdlib.h"

#include "fs.h"
#include "tusb.h"
#include "vi.h"
#include "pnmio.h"
#include "error.h"
//#include "dwtlift.h"
/* vi: set sw=4 ts=4: */
/* SPDX-License-Identifier: GPL-3.0-or-later */

/* Copyright (C) 1883 Thomas Edison - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the BSD 3 clause license, which unfortunately
 * won't be written for another century.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * A little flash file system manager for the Raspberry Pico
 */

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "hardware/watchdog.h"

#include "pico/stdio.h"
#include "pico/stdlib.h"

#include "fs.h"
#include "tusb.h"
#include "vi.h"
#include "xreceive.h"
#include "xtransmit.h"

#define MAX_ARGS 4

#define VT_ESC "\033"
#define VT_CLEAR VT_ESC "[H" VT_ESC "[J"

typedef char buf_t[128];

static uint32_t screen_x = 80, screen_y = 24;
static lfs_file_t file;
static buf_t cmd_buffer, curdir, path, result;
static int argc;
static char *argv[MAX_ARGS + 1];
static bool mounted = false, run = true;

static void hex_cmd(void) {
    if (check_mount(true))
        return;
    bool paginate = false;
    char* path = NULL;
    for (int arg = 1; arg < argc; arg++)
        if (strcmp(argv[arg], "-p") == 0)
            paginate = true;
        else
            path = argv[arg];
    if (path == NULL) {
        strcpy(result, "file name argument is required");
        return;
    }
    lfs_file_t file;
    char* fpath = full_path(path);
    if (fs_file_open(&file, fpath, LFS_O_RDONLY) < LFS_ERR_OK) {
        sprintf(result, "error opening file %s", fpath);
        return;
    }
    int l = fs_file_seek(&file, 0, LFS_SEEK_END);
    fs_file_seek(&file, 0, LFS_SEEK_SET);
    char* buf = malloc(l);
    if (!buf) {
        strcpy(result, "insufficient memory");
        return;
    }
    if (fs_file_read(&file, buf, l) != l) {
        sprintf(result, "error reading file %s", fpath);
        goto done;
    }
    char* p = buf;
    char* pend = p + l;
    int line = 0;
    while (p < pend) {
        char* p2 = p;
        printf("%04x", (int)(p - buf));
        for (int i = 0; i < 16; i++) {
            if ((i & 3) == 0)
                printf(" ");
            if (p + i < pend)
                printf("%02x", p[i]);
            else
                printf("  ");
        }
        printf(" '");
        p = p2;
        for (int i = 0; i < 16; i++) {
            if (p + i < pend) {
                if (isprint(p[i]))
                    printf("%c", p[i]);
                else
                    printf(".");
            } else
                printf(" ");
        }
        printf("'\n");
        p += 16;
        if (paginate & (++line % (screen_y - 1) == 0)) {
            char cc = getchar();
            if (cc == 0x03)
                break;
        }
    }
done:
    free(buf);
    fs_file_close(&file);
}

static void
echo_key (char c)
{
  putchar (c);
  if (c == '\r')
    putchar ('\n');
}

typedef void (*cmd_func_t) (void);

static const char *search_cmds (int len);

static void
parse_cmd (void)
{
  // read line into buffer
  char *cp = cmd_buffer;
  char *cp_end = cp + sizeof (cmd_buffer);
  char c;
  do
    {
      c = getchar ();
      if (c == '\t')
	{
	  bool infirst = true;
	  for (char *p = cmd_buffer; p < cp; p++)
	    if ((*p == ' ') || (*p == ','))
	      {
		infirst = false;
		break;
	      }
	  if (infirst)
	    {
	      const char *p = search_cmds (cp - cmd_buffer);
	      if (p)
		{
		  while (*p)
		    {
		      *cp++ = *p;
		      echo_key (*p++);
		    }
		  *cp++ = ' ';
		  echo_key (' ');
		}
	    }
	  continue;
	}
      echo_key (c);
      if (c == '\b')
	{
	  if (cp != cmd_buffer)
	    {
	      cp--;
	      printf (" \b");
	      fflush (stdout);
	    }
	}
      else if (cp < cp_end)
	*cp++ = c;
    }
  while ((c != '\r') && (c != '\n'));
  // parse buffer
  cp = cmd_buffer;
  bool not_last = true;
  for (argc = 0; not_last && (argc < MAX_ARGS); argc++)
    {
      while ((*cp == ' ') || (*cp == ','))
	cp++;			// skip blanks
      if ((*cp == '\r') || (*cp == '\n'))
	break;
      argv[argc] = cp;		// start of string
      while ((*cp != ' ') && (*cp != ',') && (*cp != '\r') && (*cp != '\n'))
	cp++;			// skip non blank
      if ((*cp == '\r') || (*cp == '\n'))
	not_last = false;
      *cp++ = 0;		// terminate string
    }
  argv[argc] = NULL;
}

char *
full_path (const char *name)
{
  if (name == NULL)
    return NULL;
  if (name[0] == '/')
    {
      strcpy (path, name);
    }
  else if (curdir[0] == 0)
    {
      strcpy (path, "/");
      strcat (path, name);
    }
  else
    {
      strcpy (path, curdir);
      if (name[0])
	{
	  strcat (path, "/");
	  strcat (path, name);
	}
    }
  return path;
}

static void
xmodem_cb (uint8_t * buf, uint32_t len)
{
  if (fs_file_write (&file, buf, len) != len)
    printf ("error writing file\n");
}

static bool
check_mount (bool need)
{
  if (mounted == need)
    return false;
  sprintf (result, "filesystem is %s mounted", (need ? "not" : "already"));
  return true;
}

static bool
check_name (void)
{
  if (argc > 1)
    return false;
  strcpy (result, "missing file or directory name");
  return true;
}

static void
put_cmd (void)
{
  if (check_mount (true))
    return;
  if (check_name ())
    return;
  if (fs_file_open (&file, full_path (argv[1]), LFS_O_WRONLY | LFS_O_CREAT) <
      0)
    {
      strcpy (result, "Can't create file");
      return;
    }
  stdio_set_translate_crlf (&stdio_uart, false);
  xmodemReceive (xmodem_cb);
  stdio_set_translate_crlf (&stdio_uart, true);
  int pos = fs_file_seek (&file, 0, LFS_SEEK_END);
  fs_file_close (&file);
  sprintf (result, "\nfile transfered, size: %d\n", pos);
}

int
check_cp_parms (char **from, char **to, int copy)
{
  *from = NULL;
  *to = NULL;
  int rc = 1;
  do
    {
      if (argc < 3)
	{
	  strcpy (result, "need two names");
	  break;
	}
      *from = strdup (full_path (argv[1]));
      if (*from == NULL)
	{
	  strcpy (result, "no memory");
	  break;
	}
      if (copy)
	{
	  struct lfs_info info;
	  if (fs_stat (*from, &info) < 0)
	    {
	      sprintf (result, "%s not found", *from);
	      break;
	    }
	  if (info.type != LFS_TYPE_REG)
	    {
	      sprintf (result, "%s is a directory", *from);
	      break;
	    }
	}
      *to = strdup (full_path (argv[2]));
      if (*to == NULL)
	{
	  strcpy (result, "no memory");
	  break;
	}
      struct lfs_info info;
      if (fs_stat (*from, &info) < 0)
	{
	  sprintf (result, "%s not found", *from);
	  break;
	}
      if (fs_stat (*to, &info) >= 0)
	{
	  sprintf (result, "%s already exists", *to);
	  break;
	}
      rc = 0;
    }
  while (0);
  if (rc)
    {
      if (*from)
	free (*from);
      if (*to)
	free (*to);
    }
  return rc;
}

static void
mv_cmd (void)
{
  char *from;
  char *to;
  if (check_cp_parms (&from, &to, 0))
    return;
  if (fs_rename (from, to) < 0)
    sprintf (result, "could not rename %s to %s", from, to);
  else
    sprintf (result, "%s renamed to %s", from, to);
  free (from);
  free (to);
}

static void
cp_cmd (void)
{
  char *from;
  char *to;
  char *buf = NULL;
  if (check_cp_parms (&from, &to, 1))
    return;
  result[0] = 0;
  lfs_file_t in, out;
  do
    {
      buf = malloc (4096);
      if (buf == NULL)
	{
	  strcpy (result, "no memory");
	  break;
	}
      if (fs_file_open (&in, from, LFS_O_RDONLY) < 0)
	{
	  sprintf (result, "error opening %s", from);
	  break;
	}
      if (fs_file_open (&out, to, LFS_O_WRONLY | LFS_O_CREAT) < 0)
	{
	  sprintf (result, "error opening %s", from);
	  break;
	}
      int l = fs_file_read (&in, buf, 4096);
      while (l > 0)
	{
	  if (fs_file_write (&out, buf, l) != l)
	    {
	      sprintf (result, "error writing %s", to);
	      break;
	    }
	  l = fs_file_read (&in, buf, 4096);
	}
    }
  while (false);
  fs_file_close (&in);
  fs_file_close (&out);
  if (buf)
    free (buf);
  if (!result[0])
    sprintf (result, "file %s copied to %s", from, to);
  free (from);
  free (to);

}
static void
get_cmd (void)
{
  if (check_mount (true))
    return;
  if (check_name ())
    return;
  if (fs_file_open (&file, full_path (argv[1]), LFS_O_RDONLY) < 0)
    {
      strcpy (result, "Can't open file");
      return;
    }
  uint32_t len = fs_file_seek (&file, 0, LFS_SEEK_END);
  fs_file_rewind (&file);
  char *buf = malloc (len);
  if (buf == NULL)
    {
      strcpy (result, "not enough memory");
      goto err2;
    }
  if (fs_file_read (&file, buf, len) != len)
    {
      strcpy (result, "error reading file");
      goto err1;
    }
  stdio_set_translate_crlf (&stdio_uart, false);
  xmodemTransmit (buf, len);
  stdio_set_translate_crlf (&stdio_uart, true);
  printf ("\nfile transfered, size: %d\n", len);
err1:
  free (buf);
err2:
  fs_file_close (&file);
  strcpy (result, "transfered");
}

static void
mkdir_cmd (void)
{
  if (check_mount (true))
    return;
  if (check_name ())
    return;
  if (fs_mkdir (full_path (argv[1])) < 0)
    {
      strcpy (result, "Can't create directory");
      return;
    }
  sprintf (result, "%s created", full_path (argv[1]));
}

static void
rm_cmd (void)
{
  if (check_mount (true))
    return;
  if (check_name ())
    return;
  // lfs won't remove a non empty directory but returns without error!
  struct lfs_info info;
  char *fp = full_path (argv[1]);
  if (fs_stat (fp, &info) < 0)
    {
      sprintf (result, "%s not found", full_path (argv[1]));
      return;
    }
  int isdir = 0;
  if (info.type == LFS_TYPE_DIR)
    {
      isdir = 1;
      lfs_dir_t dir;
      fs_dir_open (&dir, fp);
      int n = 0;
      while (fs_dir_read (&dir, &info))
	if ((strcmp (info.name, ".") != 0) && (strcmp (info.name, "..") != 0))
	  n++;
      fs_dir_close (&dir);
      if (n)
	{
	  sprintf (result, "directory %s not empty", fp);
	  return;
	}
    }
  if (fs_remove (fp) < 0)
    strcpy (result, "Can't remove file or directory");
  sprintf (result, "%s %s removed", isdir ? "directory" : "file", fp);
}

static void
mount_cmd (void)
{
  if (check_mount (false))
    return;
  if (fs_mount () != LFS_ERR_OK)
    {
      strcpy (result, "Error mounting filesystem");
      return;
    }
  mounted = true;
  strcpy (result, "mounted");
}

static void
unmount_cmd (void)
{
  if (check_mount (true))
    return;
  if (fs_unmount () != LFS_ERR_OK)
    {
      strcpy (result, "Error unmounting filesystem");
      return;
    }
  mounted = false;
  strcpy (result, "mounted");
}

static void
format_cmd (void)
{
  if (check_mount (false))
    return;
  printf ("are you sure (y/N) ? ");
  fflush (stdout);
  parse_cmd ();
  if ((argc == 0) || ((argv[0][0] | ' ') != 'y'))
    {
      strcpy (result, "user cancelled");
      return;
    }
  if (fs_format () != LFS_ERR_OK)
    strcpy (result, "Error formating filesystem");
  strcpy (result, "formatted");
}

static void
status_cmd (void)
{
  if (check_mount (true))
    return;
  struct fs_fsstat_t stat;
  fs_fsstat (&stat);
  const char percent = 37;
  sprintf (result,
	   "\nflash base 0x%x, blocks %d, block size %d, used %d, total %u bytes, %1.1f%c used.\n",
	   fs_flash_base (), (int) stat.block_count, (int) stat.block_size,
	   (int) stat.blocks_used, stat.block_count * stat.block_size,
	   stat.blocks_used * 100.0 / stat.block_count, percent);
}

static void
ls_cmd (void)
{
  if (check_mount (true))
    return;
  if (argc > 1)
    full_path (argv[1]);
  else
    full_path ("");
  lfs_dir_t dir;
  if (fs_dir_open (&dir, path) < 0)
    {
      strcpy (result, "not a directory");
      return;
    }
  printf ("\n");
  struct lfs_info info;
  while (fs_dir_read (&dir, &info) > 0)
    if (strcmp (info.name, ".") && strcmp (info.name, ".."))
      if (info.type == LFS_TYPE_DIR)
	printf (" %7d [%s]\n", info.size, info.name);
  fs_dir_rewind (&dir);
  while (fs_dir_read (&dir, &info) > 0)
    if (strcmp (info.name, ".") && strcmp (info.name, ".."))
      if (info.type == LFS_TYPE_REG)
	printf (" %7d %s\n", info.size, info.name);
  fs_dir_close (&dir);
  result[0] = 0;
}

static void
cd_cmd (void)
{
  if (check_mount (true))
    return;
  if (argc < 2)
    {
      curdir[0] = 0;
      return;
    }
  if (strcmp (argv[1], "..") == 0)
    {
      if (curdir[0] == 0)
	{
	  strcpy (result, "not a directory");
	  return;
	}
      int i;
      for (i = strlen (curdir) - 1; i >= 0; i--)
	if (curdir[i] == '/')
	  break;
      if (i < 0)
	i = 0;
      curdir[i] = 0;
      return;
    }
  full_path (argv[1]);
  lfs_dir_t dir;
  if (fs_dir_open (&dir, path) < 0)
    {
      strcpy (result, "not a directory");
      return;
    }
  fs_dir_close (&dir);
  strcpy (curdir, path);
  sprintf (result, "changed to %s", curdir);
}

static void
vi_cmd (void)
{
  if (check_mount (true))
    return;
  vi (screen_x, screen_y, argc - 1, argv + 1);
  strcpy (result, VT_CLEAR "\n");
}

static void
j2k_cmd (void)
{
  if (check_mount (true))
    return;
	//in_fname is passed as argv[1]
	//out_fname is passed as argv[2]
	//Compression Ratio
	//Compress DeCompress
	/*x0=128;
	yy0=128;
	x1=256;
	yy1=256;
	*ff_in="test.j2k";*/
	printf ("%d in_fname = %s out_fname = %s CR %d C/D = %d \n", argc, argv[1], argv[2], atoi(argv[3]), atoi(argv[4]));
	//decom_test(x0, yy0,  x1,  yy1, *ff_in);	
	//lift_config(int dec, int enc, int TCP_DISTORATIO, int FILTER, int CR, int flg, int bp, long imgsz,long him,long wim, int *bufferptr)
	
	//decompress(int da_x0, int da_y0, int da_x1, int da_y1,const char *input_file)
//fs_file_close (&in);
}

static void
test_pnmio_cmd (void)
{
  if (check_mount (true))
    return;
	printf ("%d %s\n", argc, argv[1]);
    char ch;
	int ncols;
	int *ptrncols;
	int nrows;
	int *ptrnrows;
	int *maxval;
	char *fname;
	unsigned char *img;
	int i,j;
	short int val; 
 	ncols=320;
	nrows=240;
	ptrncols = &ncols;
	ptrnrows = &nrows;
	printf ("%d %s \n", argc, argv[1]);
	//fname is passed as argv[1]
	printf("ptrncols = 0x%x,ptrnrows = 0x%x \n",ptrncols, ptrnrows);
	lfs_file_t in, out;
    if (fs_file_open(&in, argv[1], LFS_O_RDONLY) < 0)
  		printf("error in open\n");        
 
/*	
	//img = pgmReadFile(fname,NULL,ptrncols, ptrnrows);
	printf("0x%x,%d 0x%x %d \n",ptrncols,ncols, ptrnrows, nrows);
	for(j=0;j<nrows;j++) {
		for (i=0;i<ncols;i++) {
			val = *img;
			printf("%d j=  %d i=  %d \n",j,i,val);
		img++;
	}
}
*/
//fs_file_close (&in);
}

static void
lsklt_cmd (void)
{
  if (check_mount (true))
    return;

  ptrs.w = 64;
  ptrs.h = 64;
	tc = KLTCreateTrackingContext ();
  //printf("tc 0x%x\n",tc);
  fl = KLTCreateFeatureList (nFeatures);
  ptrs.out_buf = ptrs.inpbuf + imgsize;
  ptrs.inp_buf = ptrs.inpbuf;
  ncols = 64;
  nrows = 64;
  printf ("%d %s %s %s\n", argc, argv[1], argv[2], argv[3]);
	ptrs.fwd_inv = &ptrs.fwd;
	*ptrs.fwd_inv = 1;
  
  lfs_file_t in, out;


  //if (fs_file_open(&in, argv[1], LFS_O_RDONLY) < 0)
  //printf("error in open\n");        

  printf ("%d\n", fs_file_open (&in, argv[1], LFS_O_RDONLY));
  int l = fs_file_size (&in), charcnt = 0, charsent = 0;
  int ii = 0, jj = 0, flag;
  char *bufptr;
   
  //char *outstrptr;
  char *buf = malloc (l + 1);
	char *outstr = malloc (75 + 1);
  fs_file_read (&in, buf, l);
/*
Read the pgm header
P5                                                                              
# Created by GIMP version 2.10.8 PNM plug-in                                    
48                                                                              
64 64                                                                           
255
*/
  for (ii = 0; ii < 3; ii++)
    printf ("%c", buf[ii]);
  flag = 1;
  while (flag)
    {
      if (buf[ii] != 10)
	{
	  printf ("%c", buf[ii]);
	  ii++;
	}
      else
	flag = 0;
    }
  ii++;
  //printf("\n%d\n",ii);
  flag = 1;
  while (flag)
    {
      if (buf[ii] != 10)
	{
	  printf ("%c", buf[ii]);
	  ii++;
	}
      else
	flag = 0;
    }
  printf ("\n");
  ii++;

  flag = 1;
  while (flag)
    {
      if (buf[ii] != 10)
	{
	  printf ("%c", buf[ii]);
	  ii++;
	}
      else
	flag = 0;
    }
  printf ("\n");
  ptrs.inp_buf = ptrs.inpbuf;
  ii++;
  bufptr = &buf[ii];
  for (jj = 0; jj < 64; jj++)
    {
      for (ii = 0; ii < 64; ii++)
	{
	  //printf("%d ",*bufptr);
	  *ptrs.inp_buf = (unsigned short int) *bufptr;
	  printf ("%d ", *ptrs.inp_buf);
	  bufptr++;
	  ptrs.inp_buf++;
	}
    }
  printf ("\n");

  ptrs.inp_buf = ptrs.inpbuf;
  printf ("opening a file to write the results\n");
  //if (fs_file_open(&fd, argv[2], LFS_O_WRONLY) < 0)
  // printf("error in open\n"); 
  printf ("need to copy the data received from host to img1\n");
  img1 = inpbuf;
  img2 = &inpbuf[4096];

  printf ("img1 = 0x%x img2 = 0x%x\n", img1, img2);
  for (i = 0; i < ncols * nrows; i++)
    {
      img1[i] = ptrs.inp_buf[i];
      //img2[i+4096] = img1[i]; 
      if (i < 5)
	printf ("%d img1 %d ptrs.buf %d \n", i, img1[i], ptrs.inp_buf[i]);
      if (i > 4090)
	printf ("%d img1 %d ptrs.buf %d \n", i, img1[i], ptrs.inp_buf[i]);
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
  ptrs.inp_buf = ptrs.inpbuf;
  img1 = inpbuf;
  img2 = &inpbuf[4096];

  //printf("img1 = 0x%x img2 = 0x%x\n",img1, img2);
	img1 = &inpbuf[0];
	img2 = &inpbuf[4096];

  if(atoi(argv[3])==1) {
		printf("klt\n");

		KLTSelectGoodFeatures (tc, img1, ncols, nrows, fl);

  	//printf("\nIn first image:\n");
  	for (i = 0; i < fl->nFeatures; i++)
    	{
      	printf ("Feature #%d:  (%f,%f) with value of %d\n",
        	 i, fl->feature[i]->x, fl->feature[i]->y,
         	fl->feature[i]->val); 

      	/*charsent = sprintf (outstr, "Feature #%d:  (%f,%f) with value of %d",
			  	i, fl->feature[i]->x, fl->feature[i]->y,
			  	fl->feature[i]->val);*/

      	//*outstr = *outstr + charsent;
      	//charcnt = charcnt + charsent;
    	}
  	//*outstr = *outstr - charcnt;
  	printf ("this is the string %d %s", charcnt, outstr);
	}
  else
	{
		printf("lifting step\n");
		ptrs.inp_buf = ptrs.inpbuf;
    printf("%d 0x%x 0x%x 0%x \n",ptrs.w, ptrs.inp_buf, ptrs.out_buf, *ptrs.fwd_inv);
		lifting (ptrs.w, ptrs.inp_buf, ptrs.out_buf, ptrs.fwd_inv);
		for(nrows=0;nrows<64;nrows++) {
			for(ncols=0;ncols<64;ncols++) {
				printf("%d ",ptrs.inp_buf[offset]);
				offset++;
			}
			printf("\n");
		}
	}
  free (buf);
	free (outstr);
  //free(&fd);
  fs_file_close (&in);
  printf ("%d \n", fs_file_open (&out, argv[2], LFS_O_WRONLY | LFS_O_CREAT));
  //fs_file_write (&out, outstr, charsent);
  fs_file_close (&out);
}



static void
quit_cmd (void)
{
  // release any resources we were using
  if (mounted)
    fs_unmount ();
  strcpy (result, "");
  run = false;
}

typedef struct
{
  const char *name;
  cmd_func_t func;
  const char *descr;
} cmd_t;

// clang-format off
static cmd_t cmd_table[] = {
  {"cd", cd_cmd, "change directory"},
  {"cp", cp_cmd, "copy file"},
  {"format", format_cmd, "format the filesystem"},
	{"hex",     hex_cmd,        "simple hexdump, use -p to paginate"},
  {"get", get_cmd, "get file (xmodem)"},
  {"ls", ls_cmd, "list directory"},
  {"mkdir", mkdir_cmd, "create directory"},
  {"mount", mount_cmd, "mount filesystem"},
  {"mv", mv_cmd, "rename file or directory"},
  {"put", put_cmd, "put file (xmodem)"},
  {"q", quit_cmd, "quit"},
  {"rm", rm_cmd, "remove file or directory"},
  {"status", status_cmd, "filesystem status"},
  {"unmount", unmount_cmd, "unmount filesystem"},
  {"vi", vi_cmd, "vi editor"},
  {"lsklt", lsklt_cmd, "lifting step 0 klt 1"},
  {"j2k", j2k_cmd, "In File Out Frile Compression Ratio Compression 0 Decompression 1"},	
  {"test_pnmio", test_pnmio_cmd, "test_pnmio"} 
};

// clang-format on

static const char *
search_cmds (int len)
{
  if (len == 0)
    return NULL;
  int i, last_i, count = 0;
  for (i = 0; i < sizeof cmd_table / sizeof cmd_table[0]; i++)
    if (strncmp (cmd_buffer, cmd_table[i].name, len) == 0)
      {
	last_i = i;
	count++;
      }
  if (count != 1)
    return NULL;
  return cmd_table[last_i].name + len;
}

static bool
stdio_init (int uart_rx_pin)
{
  gpio_init (uart_rx_pin);
  gpio_set_pulls (uart_rx_pin, 1, 0);
  sleep_ms (1);
  bool v1 = gpio_get (uart_rx_pin);
  gpio_set_pulls (uart_rx_pin, 0, 1);
  sleep_ms (1);
  bool v2 = gpio_get (uart_rx_pin);
  gpio_set_pulls (uart_rx_pin, 0, 0);
  if (v1 != v2)
    {
      stdio_usb_init ();
      while (!tud_cdc_connected ())
	sleep_ms (1000);
      return false;
    }
  else
    {
      stdio_uart_init ();
      getchar_timeout_us (1000);
    }
  return true;
}

static bool
screen_size (void)
{
  int rc = false;
  do
    {
      stdio_set_translate_crlf (&stdio_uart, false);
      printf (VT_ESC "[999;999H" VT_ESC "[6n");
      fflush (stdout);
      int k = getchar_timeout_us (100000);
      if (k == PICO_ERROR_TIMEOUT)
	break;
      char *cp = cmd_buffer;
      while (cp < cmd_buffer + sizeof cmd_buffer)
	{
	  k = getchar_timeout_us (100000);
	  if (k == PICO_ERROR_TIMEOUT)
	    break;
	  *cp++ = k;
	}
      stdio_set_translate_crlf (&stdio_uart, true);
      if (cmd_buffer[0] != '[')
	break;
      *cp = 0;
      if (cp - cmd_buffer < 5)
	break;
      char *end;
      uint32_t row, col;
      if (!isdigit (cmd_buffer[1]))
	break;
      errno = 0;
      row = strtoul (cmd_buffer + 1, &end, 10);
      if (errno)
	break;
      if (*end != ';' || !isdigit (end[1]))
	break;
      col = strtoul (end + 1, &end, 10);
      if (errno)
	break;
      if (*end != 'R')
	break;
      if (row < 1 || col < 1 || (row | col) > 0x7fff)
	break;
      screen_x = col;
      screen_y = row;
      rc = true;
    }
  while (false);
  return rc;
}

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
pshell (void *pvParameters)
{
  while (true)
    {
      //printf("This is a place holder for the pshell task\n");
      while (run)
	{
	  printf ("%s: ", full_path (""));
	  fflush (stdout);
	  parse_cmd ();
	  bool found = false;
	  int i;
	  result[0] = 0;
	  if (argc)
	    for (i = 0; i < sizeof cmd_table / sizeof cmd_table[0]; i++)
	      if (strcmp (argv[0], cmd_table[i].name) == 0)
		{
		  cmd_table[i].func ();
		  printf ("%d %s %s\n", i, cmd_table[i].name,
			  cmd_table[i].descr);
		  found = true;
		  break;
		}
	  if (!found)
	    {
	      if (argc)
		printf ("command unknown!\n\n");
	      for (int i = 0; i < sizeof cmd_table / sizeof cmd_table[0]; i++)
		printf ("%7s - %s\n", cmd_table[i].name, cmd_table[i].descr);
	      printf ("\n");
	      continue;
	    }
	  printf ("%s\n", result);
	}
      vTaskDelay (35000);
    }
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
  bool uart = stdio_init (PICO_DEFAULT_UART_RX_PIN);
  bool detected = screen_size ();
  printf (VT_CLEAR "\n"
	  "Pico Shell - Copyright (C) 1883 Thomas Edison\n"
	  "This program comes with ABSOLUTELY NO WARRANTY.\n"
	  "This is free software, and you are welcome to redistribute it\n"
	  "under certain conditions. See LICENSE file for details.\n\n"
	  "console on %s (%s %u rows, %u columns)\n\n"
	  "enter command, hit return for help\n\n",
	  uart ? "UART" : "USB", detected ? "detected" : "defaulted to",
	  screen_y, screen_x);
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


  tc = KLTCreateTrackingContext ();
  //printf("tc 0x%x\n",tc);
  fl = KLTCreateFeatureList (nFeatures);
  //KLTPrintTrackingContext(tc);     

  ptrs.head = &tt[0];
  ptrs.tail = &tt[0];
  ptrs.topofbuf = &tt[0];


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



  xTaskCreate (pshell, "Task 5", 256, NULL, 2, NULL);
  vTaskStartScheduler ();


  while (1)
    {
    };
}
