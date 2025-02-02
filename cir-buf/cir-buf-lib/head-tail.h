////////////////////////////////////////////////////////////////////////////////
//
// Filename: 	head-tail.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef	HEAD_TAIL_H
#define	HEAD_TAIL_H

extern char * bump_head(char * head, char * endofbuf,char * topofbuf);
extern char * bump_tail(char * tail,char * endofbuf,char * topofbuf);
extern char * dec_head(char * head,char * endofbuf,char * topofbuf);
extern char * dec_tail(char * tail,char * endofbuf,char * topofbuf);

#endif