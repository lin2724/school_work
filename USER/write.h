#ifndef _WRITE_H_
#define _WRITE_H_

#define MAXDOT 400
#define XMAX 240
#define YMAX 320


#define MOD(dat) ((unsigned int)((signed int)dat>0?dat:-dat))
#define GAP (20)

extern void WriteSignal(unsigned int pox,unsigned int poy,char* dat);


#endif
