#define IO(x) (x)
#define	ISR		IO(0x02007000)
#define MONCSR          IO(0x0200E000)
#define KBDDATA         IO(0x0200E008)
#define VDMACSR         IO(0x02000180)

extern int xp, yp;
extern void pr(char*);
extern void put_c(int, int, char);
extern unsigned char key0[];

int
trap(void) {
  int xpsave, ypsave;
  xpsave = xp; ypsave = yp;

  xp = 0; yp = 4;
  pr("TRAP!"); 
  xp = xpsave; yp = ypsave;

  return 0;
}

void
printhex(int n) {
  char s[20];
  int i, j;
  pr("0x");
  j=0x10000000;
  for (i=7; i>=0; i--) {
    s[9-i] = ((n/j)%16) < 10 ? '0'+((n/j)%16) : 'a'+((n/j)%16);
    j=j/16;
  }
  s[8] = '\0';
  pr(s);
}

void
print(int n) {
  char s[20];
  int i, j;
  pr(">");
  j=1000000000;
  for (i=9; i>=0; i--) {
    s[9-i] = 0x30 + (n/j)%10;
    j=j/10;
  }
  s[10] = '\0';
  pr(s);
  pr("<");
}

void di(void) {
  __asm ("movew #0x2600,%sr");
}

void ei(void) {
  __asm ("andw #0xf8ff,%sr");
}

void 
int3(void) {
  static int count = 0;
  static int time = 0;
  unsigned int isr;
  int xpsave, ypsave;

  xpsave = xp; ypsave = yp; 

  isr = *(unsigned int*)ISR;

  xp = 0; yp = 3;
  printhex(*(volatile unsigned int*)MONCSR);

  xp = 0; yp = 5;

        if(isr & (1<<7)){
                pr("floppyintr();");
        }
        if(isr & (1<<3)){
          if(*(volatile unsigned int*)MONCSR & (1<<23)) {
            if(*(volatile unsigned int*)MONCSR & (1<<22)) {
              unsigned int d;
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
                    pr("kbdmouseintr(): ");
                    put_c(xp+1, yp, key0[d&0x7F]);
                  }
                }
              }
            }
          }
          *(unsigned int*)MONCSR |= (1<<17);
        }
        if(isr & (1<<2)){
                pr("poweroff();");
        }
        if(isr & (1<<9)){
                pr("etherrintr();");
        }
        if(isr & (1<<10)){
                pr("etherxintr();");
        }
        if(isr & (1<<12)){
                pr("scsiintr(Scsidevice);");
        }
        if(isr & (1<<5)){
                time++;
                xp = 40; yp = 4;
                if ((time % 60) == 0) {
                  pr("clock: ");
                  print(time/60);
                }
                // *(unsigned int*)MONCSR |= (1<<5);
                *(unsigned int*)VDMACSR |= (1<<20);
        }

  count++;
  xp = xpsave; yp = ypsave; 
}
