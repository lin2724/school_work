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
#include "moto.h"
//ALIENTEKս��STM32������ʵ��46
//��дʶ�� ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
//����¼�Ĺ켣����
atk_ncr_point READ_BUF[MAXDOT];					 
int fuck=0;
 int main(void)
 {	 
 	unsigned int i=0,j=0,k=0,t=0,b=0;	    	
	u8 tcnt=0;    
	u8 res[10];
	u8 key;		    
	u16 pcnt=0;
	u8 mode=4;	//Ĭ���ǻ��ģʽ	    	    

	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
 	LCD_Init();				//��ʼ��Һ�� 
	LED_Init();         	//LED��ʼ��	 
	KEY_Init();				//������ʼ��	  													    
	TP_Init();     			//��������ʼ��
 	mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��	
	alientek_ncr_init();	//��ʼ����дʶ��

	POINT_COLOR=RED;      
 	while(font_init()) 				//����ֿ�
	{	    
		LCD_ShowString(60,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(60,50,240,66,WHITE);//�����ʾ	     
	}
RESTART:
 	Show_Str(60,10,200,16,"ս�� STM32������",16,0);				    	 
	Show_Str(60,30,200,16,"��дʶ��ʵ��",16,0);				    	 
	Show_Str(60,50,200,16,"����ԭ��@ALIENTEK",16,0);				    	 
	Show_Str(60,70,200,16,"KEY0:MODE KEY2:Adjust",16,0);			    
	Show_Str(60,90,200,16,"ʶ����:",16,0);			    
	LCD_DrawRectangle(19,114,220,315);
 	POINT_COLOR=BLUE;      
	Show_Str(96,207,200,16,"��д��",16,0);	 
	tcnt=1;
	 moto_init();
//	while(1)
//	{
//		key=KEY_Scan(0);
//		if(key==KEY_LEFT)
//		{
//			xmotostep(0, 20);
//		}
//		if(key==KEY_RIGHT)	
//		{
//			xmotostep(1, 20);
//		}
//		
//		if(key==KEY_UP)	
//		{
//			while(1)
//			{
//			xmotostep(0, 20);
//			key=KEY_Scan(0);
//			if(key==KEY_UP)break;
//			}
//		}
//		
//		if(key==KEY_DOWN)	
//		{
//			while(1)
//			{
//			xmotostep(1, 20);
//			key=KEY_Scan(0);
//			if(key==KEY_DOWN)break;
//			}
//		}
//	}
	while(1)
	{
//		key=KEY_Scan(0);
//		if(key==KEY_LEFT)
//		{
//			TP_Adjust();  	//��ĻУ׼
//			LCD_Clear(WHITE); 
//			goto RESTART;	//���¼��ؽ���
//		}
  
 		tp_dev.scan(0);//ɨ��
 		if(tp_dev.sta&TP_PRES_DOWN)//�а���������
		{
			if(tcnt)
			{
				READ_BUF[pcnt].x=1;
				READ_BUF[pcnt].y=1; 
				pcnt++;
			}
			
			delay_ms(1);//��Ҫ����ʱ,��������Ϊ�а�������.
 			tcnt=0;//�ɿ�ʱ�ļ�������� 	 		    
			if((tp_dev.x[0]<220&&tp_dev.x[0]>=20)&&(tp_dev.y[0]<315&&tp_dev.y[0]>=115))
			{			 
				//TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],BLUE);//��ͼ 
				POINT_COLOR=BLUE;
				LCD_DrawPoint(tp_dev.x[0],tp_dev.y[0]);
				if(pcnt<MAXDOT)//�ܵ�������200
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
						if((READ_BUF[pcnt-1].y!=tp_dev.y[0])&&(READ_BUF[pcnt-1].x!=tp_dev.x[0]))//x,y�����
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
		}else //�����ɿ��� 
		{
			if(!tcnt)
			{
				READ_BUF[pcnt].x=0;
				READ_BUF[pcnt].y=0; 
				pcnt++;
			}
			tcnt++;
			delay_ms(10);	 // delay_ms(10)
			//��ʱʶ��
			i++;	 	    
			if(tcnt>=40)
			{
				if(pcnt)//����Ч������		 
				{
					LCD_Fill(20,115,219,314,WHITE);
					printf("�ܵ���:%d\r\n",pcnt);
					WriteSignal(READ_BUF[1].x,READ_BUF[1].y,(char*)READ_BUF);
					MemClear((char*) READ_BUF, sizeof(READ_BUF));

					pcnt=0;
					tcnt=1;					
 
				while(1)
					{

					key=KEY_Scan(0);
					if(key==KEY_DOWN)break;
					}
					
				}else tcnt=1;
//				delay_ms(5000);
				
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










