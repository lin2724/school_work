#include <stdio.h>
#include "atk_ncr.h"
#include "lcd.h"
#include "write.h"
#include "moto.h"
#include "delay.h"
#include "key.h"






 int CurPointX=0;
 int CurPointY=0;
int WriteEn = 0;

int PosCheck(void)
{
	if(CurPointX>240)return 1;
	if(CurPointY>320)return 1;
	return 0;
}



/******************
* @brief:单步控制函数
* @param:
* @date:2015.3.30
* @aulther: zzl
* @other:
 *******************/
void Xmoto(int dir, int step)
{
	int i;
	if(PosCheck())return;
	for(i=1;i<=step;i++)
	{
		if(!WriteEn)break;
		if(dir>0)
			{
//				xmotostep(1, 1);
				if(CurPointX+i>XMAX){LCD_DrawPoint(XMAX,CurPointY);CurPointX=XMAX;return;}
				LCD_DrawPoint(CurPointX+i,CurPointY);
			}
		else 
			{
//				xmotostep(0, 1);
				if(CurPointX<=i){LCD_DrawPoint(0,CurPointY);CurPointX=0;return;}
				LCD_DrawPoint(CurPointX-i,CurPointY);
			}
		
	}
	xmotostep(dir, step);
	
	if(dir>0){CurPointX+=step;}
	else {CurPointX-=step;}
}

void Ymoto(int dir, int step)
{
	int i;
	if(PosCheck())return;
	for(i=1;i<=step;i++)
	{
		if(!WriteEn)break;
		if(dir>0)
		{
			if(CurPointY+i>YMAX){LCD_DrawPoint(CurPointX,YMAX);CurPointY=XMAX;return;}
			LCD_DrawPoint(CurPointX,CurPointY+i);
		}
		else 
		{
			if(CurPointY<=i){LCD_DrawPoint(CurPointX,0);CurPointY=0;return;}
			LCD_DrawPoint(CurPointX,CurPointY-i);
		}
		
	}
	
	if(dir>0){CurPointY+=step;}
	else {CurPointY-=step;}
}

void Zmoto(int updown)
{
	
}

#define UP 0 
#define DOWN 1

void PenUpDown(int cmd)
{
	if(cmd)WriteEn=1;
	else WriteEn=0;
}



/******************
* @brief:斜线绘制子函数
* @param:
* @date:2015.3.30
* @aulther: zzl
* @other:
 *******************/

void MovePoint( int desx, int desy)
{

	int arg,witch;
	signed int lengthx,lengthy;
	lengthx = desx - CurPointX;
	lengthy = desy - CurPointY;
	
	while(lengthx&&lengthy)
	{
//		arg = (MOD(lengthx)<<3);

		lengthx = desx - CurPointX;
		lengthy = desy - CurPointY;
		if(MOD(lengthx)/MOD(lengthy))
		{
			arg = ( (MOD(lengthx)<<3) / MOD(lengthy) +(1<<2) )>>3;//+0.5  0.5*2*2*2 = (1<<2)=4
			witch = 0;
		}else
		{
			arg = ( (MOD(lengthy)<<3) / MOD(lengthx) +(1<<2) )>>3;//+0.5  0.5*2*2*2 = (1<<2)=4
			witch = 1;
		}
		
		if(witch)
		{

			
			Xmoto(lengthx, 1);
//			lengthx-=1;
			Ymoto(lengthy, arg);
//			lengthy-=arg;
		}else
		{
			Ymoto(lengthy, 1);
//			lengthy-=1;
			Xmoto(lengthx, arg);
//			lengthx-=arg;
			
		}
	}
	

}



void WriteSignal(unsigned int pox,unsigned int poy,char* dat)
{
	atk_ncr_point* buf = (atk_ncr_point*)dat;
	int i;
	int start=0;
	int empty=0;
	int delay_arg=100;
	int key;
	int lastx=0,lasty=0;
	MovePoint(pox,poy);
	for(i=0;i<MAXDOT;i++)
	{
		key=KEY_Scan(0);
		if(key==KEY_LEFT&&delay_arg>20)
		{
			delay_arg-=20;
		}
		if(key==KEY_RIGHT)	
		{
			delay_arg+=20;
		}
		
		if(empty>10)break;
		
		
		delay_ms(delay_arg);
		if(buf->x==0 && buf->y==0){PenUpDown(UP);buf++;empty++;continue;}
		if(buf->x==1 && buf->y==1){start=1;buf++;empty=0;continue;}
		
		if(start)
		{
			MovePoint(buf->x,buf->y);
			PenUpDown(DOWN);
		}
		
		if( MOD((signed int)(buf->x-lastx))<10 && MOD((signed int)(buf->y-lasty))<10)
		{
			buf++;i++;
		}
		MovePoint(buf->x,buf->y);
		if(WriteEn)
		{
		lastx = buf->x;
		lastx = buf->x;
		}
		buf++;
//		if(!buf->x&&!buf->y)return;
	}
	
	
	
}
