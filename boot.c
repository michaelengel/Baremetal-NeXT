/*	$NetBSD: boot.c,v 1.12 2016/06/11 06:35:00 dholland Exp $	*/
/*
 * Copyright (c) 1994 Rolf Grossmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Rolf Grossmann.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/param.h>

#include "font.h"
#include "keys.h"

int xp=0, yp=100;

unsigned char duplow(unsigned char c);
unsigned char duphigh(unsigned char c);
void put_c(int xp, int yp, char c);
void pr(char *c);

typedef struct Systimer Systimer;
struct Systimer
{
        unsigned char   high;
        unsigned char   low;
        unsigned char   dummy[2];
        unsigned char   csr;
};

unsigned char duplow(unsigned char c) {
  unsigned char r = 0;
  r |= (c & 0x8)? (1<<7):0;
  r |= (c & 0x8)? (1<<6):0;
  r |= (c & 0x4)? (1<<5):0;
  r |= (c & 0x4)? (1<<4):0;
  r |= (c & 0x2)? (1<<3):0;
  r |= (c & 0x2)? (1<<2):0;
  r |= (c & 0x1)? (1<<1):0;
  r |= (c & 0x1)? (1<<0):0;
  return r;
}

unsigned char duphigh(unsigned char c) {
  unsigned char r = 0;
  r |= (c & 0x80)? (1<<7):0;
  r |= (c & 0x80)? (1<<6):0;
  r |= (c & 0x40)? (1<<5):0;
  r |= (c & 0x40)? (1<<4):0;
  r |= (c & 0x20)? (1<<3):0;
  r |= (c & 0x20)? (1<<2):0;
  r |= (c & 0x10)? (1<<1):0;
  r |= (c & 0x10)? (1<<0):0;
  return r;
}

void 
put_c(int x, int y, char c) {
  volatile unsigned char *vid = (volatile unsigned char *)0x0b000000;
  if (c<32 || c>126) return;

  vid += (2*x) + (y*16*1152/4);
  unsigned char b;
  for (int i=0; i<13; i++) {
    b = font[(int)c-32][12-i];
    *vid     = duphigh(b);
    *(vid+1) = duplow(b);
    vid+=1152/4;
  }
  xp++;
  if (xp>144) { xp=0; yp++; }
  // if (yp>52) { scroll(); }
}

void 
pr(char *s) {
  char c;
  while ((c=*s++)) {
    put_c(xp, yp, c);
  }
}

unsigned char cursor[] = {
  0b11000000, 
  0b11100000,
  0b11110000,
  0b11111000,
  0b11111100,
  0b00110000,
  0b00110000,
  0b00011000,
  0b00011000,
};

void mousetrack(int b, int dx, int dy);
void drawmouse(int x, int y, int mode);

int mousex = 560, mousey = 416;

void drawmouse(int x, int y, int mode) {
  volatile unsigned char *vid = (volatile unsigned char *)0x0b000000;
  vid += (x/4) + (y*1152/4);
  *(vid+0) ^= duphigh(cursor[0]);
  *(vid+1) ^= duplow(cursor[0]);
  vid += 1152/4;
  *(vid+0) ^= duphigh(cursor[1]);
  *(vid+1) ^= duplow(cursor[1]);
  vid += 1152/4;
  *(vid+0) ^= duphigh(cursor[2]);
  *(vid+1) ^= duplow(cursor[2]);
  vid += 1152/4;
  *(vid+0) ^= duphigh(cursor[3]);
  *(vid+1) ^= duplow(cursor[3]);
  vid += 1152/4;
  *(vid+0) ^= duphigh(cursor[4]);
  *(vid+1) ^= duplow(cursor[4]);
  vid += 1152/4;
  *(vid+0) ^= duphigh(cursor[5]);
  *(vid+1) ^= duplow(cursor[5]);
  vid += 1152/4;
  *(vid+0) ^= duphigh(cursor[6]);
  *(vid+1) ^= duplow(cursor[6]);
  vid += 1152/4;
  *(vid+0) ^= duphigh(cursor[7]);
  *(vid+1) ^= duplow(cursor[7]);
  vid += 1152/4;
  *(vid+0) ^= duphigh(cursor[8]);
  *(vid+1) ^= duplow(cursor[8]);
}

void xdrawmouse(int x, int y, int mode) {
  volatile unsigned char *vid = (volatile unsigned char *)0x0b000000;
  vid += (x/4) + (y*1152/4);
  *(vid+0) ^= 0xFF;
  *(vid+1) ^= 0xFF;
  vid += 1152/4;
  *(vid+0) ^= 0xFF;
  *(vid+1) ^= 0xFF;
  vid += 1152/4;
  *(vid+0) ^= 0xFF;
  *(vid+1) ^= 0xFF;
  vid += 1152/4;
  *(vid+0) ^= 0xFF;
  *(vid+1) ^= 0xFF;
  vid += 1152/4;
  *(vid+0) ^= 0xFF;
  *(vid+1) ^= 0xFF;
  vid += 1152/4;
  *(vid+0) ^= 0xFF;
  *(vid+1) ^= 0xFF;
  vid += 1152/4;
  *(vid+0) ^= 0xFF;
  *(vid+1) ^= 0xFF;
  vid += 1152/4;
  *(vid+0) ^= 0xFF;
  *(vid+1) ^= 0xFF;
  vid += 1152/4;
  *(vid+0) ^= 0xFF;
  *(vid+1) ^= 0xFF;
  vid += 1152/4;
  *(vid+0) ^= 0xFF;
  *(vid+1) ^= 0xFF;
}

void mousetrack(int b, int dx, int dy) {
  if (b == 0)
    drawmouse(mousex, mousey, 0);

  mousex += dx; 
  if (mousex < 0) mousex = 0;
  if (mousex > 1119) mousex = 1119;

  mousey += dy; 
  if (mousey < 0) mousey = 0;
  if (mousey > 831) mousey = 831;

  drawmouse(mousex, mousey, 1);
}

#define IO(x) (x)
#define	IMR		IO(0x02007800)
#define TIMER           IO(0x02116000)
#define VIDEOOUTLIMIT   IO(0x02004184) /* XXX */

int
main(char *boot_arg)
{
#if 0
        *(unsigned int*)IMR    = (1<<2)                /* power on/off switch */
                        | (1<<3)                /* kbd, mouse */
                        | (1<<5)                /* video flyback */
                        | (1<<7)                /* floppy */
                        | (1<<28)               /* ether xmit complete */
                        | (1<<27)               /* ether rcv complete */
                        | (1<<12)               /* scsi device int */
                        | (1<<23)               /* sound out dma */
                        ;
#endif

	/* HACK HACK */
	unsigned char *vid = (unsigned char *)0x0b000000;

	for (int i=0; i<239616; i++)
	  *vid++ = 0;

	xp = 0;
	yp = 0;

        volatile Systimer *st;

        st = (volatile Systimer*)TIMER;
        st->low = 0xff;
        st->high = 0xff;
        st->csr = (1<<7)|(1<<6);
        *(unsigned int*)VIDEOOUTLIMIT = 0xea;

        *(unsigned int*)IMR    = 0;
        *(unsigned int*)IMR    = (1<<3)                /* kbd, mouse */
                        | (1<<5)                /* video flyback */
                        ;

        put_c(0,0,'N');	
        put_c(1,0,'e');	
        put_c(0,1,'X');	
        put_c(1,1,'T');	

	xp = 20;
	yp = 10;
	pr("Hallo STANDALONE NeXTstation!");


#define MONCSR          IO(0x0200E000)
#define KBDDATA         IO(0x0200E008)

	xp = 0;
	yp = 12;

        pr("#");

        drawmouse(mousex, mousey, 1);

	unsigned long d;
        char c = '!';
        char s[2];
        s[1] = '\0';

	while(1) {
          s[0] = c;
          c = (c+1) % 127;
          pr(s);

          for (volatile int i=0; i<100000; i++);
#if 0
          if(*(volatile unsigned int*)MONCSR & (1<<23)) {
            if(*(volatile unsigned int*)MONCSR & (1<<22)) {
              d = *(volatile unsigned int*)KBDDATA;
              if(d & (0xF<<24)){
                // mse
                int dx, dy, b=0;
                if((d&0x01) == 0)
                  b |= 1;
                else
                  b &= ~1;
                if((d&0x100) == 0)
                  b |= 4;
                  // b |= mouseshifted ? 2 : 4;
                else
                  b &= ~6;
                dx = (d>>1) & 0x7F;
                dy = (d>>9) & 0x7F;
                if(dx & 0x40)
                  dx |= ~0x7F;
                if(dy & 0x40)
                  dy |= ~0x7F;
                mousetrack(b, 3*-dx, 3*-dy);
              } else {
                // kbd
                if (d & 0x8000) {
                  if ((d & 0x780) == 0) {
                    put_c(xp, yp, key0[d&0x7F]);
                  }
                }
              }
            }
	  }
#endif
	}
          
	while(1);
}
