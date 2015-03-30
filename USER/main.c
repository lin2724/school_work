#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "24cxx.h"
#include "touch.h"
#include "malloc.h"
#include "string.h"
#include "text.h"
#include "atk_ncr.h"


#include "write.h"
#include "method.h"
//ALIENTEK战舰STM32开发板实验46
//手写识别 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
//最大记录的轨迹点数
atk_ncr_point READ_BUF[MAXDOT];					 
int fuck=0;
 int main(void)
 {	 
 	unsigned int i=0,j=0,k=0,t=0,b=0;	    	
	u8 tcnt=0;    
	u8 res[10];
	u8 key;		    
	u16 pcnt=0;
	u8 mode=4;	//默认是混合模式	    	    

	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为9600
 	LCD_Init();				//初始化液晶 
	LED_Init();         	//LED初始化	 
	KEY_Init();				//按键初始化	  													    
	TP_Init();     			//触摸屏初始化
 	mem_init(SRAMIN);		//初始化内部内存池	
	alientek_ncr_init();	//初始化手写识别

	POINT_COLOR=RED;      
 	while(font_init()) 				//检查字库
	{	    
		LCD_ShowString(60,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(60,50,240,66,WHITE);//清除显示	     
	}
RESTART:
 	Show_Str(60,10,200,16,"战舰 STM32开发板",16,0);				    	 
	Show_Str(60,30,200,16,"手写识别实验",16,0);				    	 
	Show_Str(60,50,200,16,"正点原子@ALIENTEK",16,0);				    	 
	Show_Str(60,70,200,16,"KEY0:MODE KEY2:Adjust",16,0);			    
	Show_Str(60,90,200,16,"识别结果:",16,0);			    
	LCD_DrawRectangle(19,114,220,315);
 	POINT_COLOR=BLUE;      
	Show_Str(96,207,200,16,"手写区",16,0);	 
	tcnt=100;
	while(1)
	{
//		key=KEY_Scan(0);
//		if(key==KEY_LEFT)
//		{
//			TP_Adjust();  	//屏幕校准
//			LCD_Clear(WHITE); 
//			goto RESTART;	//重新加载界面
//		}
//		if(key==KEY_RIGHT)	
//		{
//			LCD_Fill(20,115,219,314,WHITE);//清除当前显示
//			mode++;
//			if(mode>4)mode=1;
//			switch(mode)
//			{
//				case 1:
//					Show_Str(80,207,200,16,"仅识别数字",16,0);	
//					break;	 	    
//				case 2:
//					Show_Str(64,207,200,16,"仅识别大写字母",16,0);	
//					break;	 	    
//				case 3:
//					Show_Str(64,207,200,16,"仅识别小写字母",16,0);	
//					break;	 	    
//				case 4:
//					Show_Str(88,207,200,16,"全部识别",16,0);	
//					break;	 
//			}
//			tcnt=100;
//		}		   
 		tp_dev.scan(0);//扫描
 		if(tp_dev.sta&TP_PRES_DOWN)//有按键被按下
		{
			if(tcnt)
			{
				READ_BUF[pcnt].x=1;
				READ_BUF[pcnt].y=1; 
				pcnt++;
			}
			
			delay_ms(1);//必要的延时,否则老认为有按键按下.
 			tcnt=0;//松开时的计数器清空 	 		    
			if((tp_dev.x[0]<220&&tp_dev.x[0]>=20)&&(tp_dev.y[0]<315&&tp_dev.y[0]>=115))
			{			 
				TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],BLUE);//画图 
				if(pcnt<MAXDOT)//总点数少于200
				{
					if(pcnt>1)
					{
						fuck=MOD((signed int)(1-2));
						i=MOD(READ_BUF[pcnt-1].y-tp_dev.y[0]);
						j = ((unsigned int)MOD((signed int)(READ_BUF[pcnt-1].y-tp_dev.y[0])));
						k = MOD(READ_BUF[pcnt-1].y-tp_dev.y[0]);
						t= (unsigned int)MOD((signed int)(READ_BUF[pcnt-1].x-tp_dev.x[0]));
//						i = ((unsigned int)MOD(READ_BUF[pcnt-1].y-tp_dev.y[0])>GAP);
//						i = (MOD(READ_BUF[pcnt-1].y-tp_dev.y[0])<GAP);
//						i=5>4;
//						i=4<4;
						if((READ_BUF[pcnt-1].y!=tp_dev.y[0])&&(READ_BUF[pcnt-1].x!=tp_dev.x[0]))//x,y不相等
						{
							
							if( j>GAP || t>GAP )//
							{
										READ_BUF[pcnt].x=0;
										READ_BUF[pcnt].y=0; 
										pcnt++;
										READ_BUF[pcnt].x=1;
										READ_BUF[pcnt].y=1; 
										pcnt++;
							}
							READ_BUF[pcnt].x=tp_dev.x[0];
							READ_BUF[pcnt].y=tp_dev.y[0]; 
							pcnt++;
						}	
					}else 
					{
						READ_BUF[pcnt].x=tp_dev.x[0];
						READ_BUF[pcnt].y=tp_dev.y[0]; 
						pcnt++;
					}		  
				}   						  				  
			}  
		}else //按键松开了 
		{
			if(!tcnt)
			{
				READ_BUF[pcnt].x=0;
				READ_BUF[pcnt].y=0; 
				pcnt++;
			}
			tcnt++;
			delay_ms(10);	 // delay_ms(10)
			//延时识别
			i++;	 	    
			if(tcnt==40)
			{
				if(pcnt)//有有效的输入		 
				{
					LCD_Fill(20,115,219,314,WHITE);
					printf("总点数:%d\r\n",pcnt);
					WriteSignal(READ_BUF[1].x,READ_BUF[1].y,(char*)READ_BUF);
					MemClear((char*) READ_BUF, sizeof(READ_BUF));
//					alientek_ncr(READ_BUF,pcnt,6,mode,(char*)res);
//					printf("识别结果:%s\r\n",res);
					pcnt=0;	 			    			   
//	  			POINT_COLOR=BLUE;//设置画笔蓝色
//		 			LCD_ShowString(60+72,90,200,16,16,res);	    
				}else tcnt=1;
				delay_ms(5000);
				LCD_Fill(20,115,219,314,WHITE);
			} 
		}  
		if(i==30)
		{
			i=0;
			LED0=!LED0;
		}		   
	}     										    			    
}










