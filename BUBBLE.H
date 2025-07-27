/*
	@(#)bubble.h
	
	Julian F. Reschke, 20. Juli 1998
*/

#ifndef __BUBBLE_H
#define __BUBBLE_H

#define BUBBLEGEM_REQUEST	0xBABA
#define BUBBLEGEM_SHOW  	0xBABB
#define BUBBLEGEM_ACK   	0xBABC
#define BUBBLEGEM_HIDE  	0xBABF

typedef struct
{
   long   magic;   /* 'BGEM' */
   long   size;    /* Gr”že dieser Struktur, derzeit 18 */
   int    release; /* derzeit 7, nie kleiner als 5 */
   int    active;  /* <>0, wenn gerade eine Hilfe angezeigt wird;
                        0  sonst */
   MFORM *mhelp;   /* Zeiger auf Hilfe-Mausform */
   int    dtimer;  /* D„mon-Timer; Default 200ms; ab Release 6 */
} BGEM;

#endif