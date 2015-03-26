#include "fontupd.h"
#include "ff.h"	  
#include "flash.h"   
#include "lcd.h"  
#include "malloc.h"

u32 FONTINFOADDR=(1024*6+500)*1024;//Ĭ����6M�ĵ�ַ
//�ֿ���Ϣ�ṹ��. 
_font_info ftinfo=
{
	0,  					//UNICODEGBK �����׵�ַ�̶���ַ
	0,
	0,
	0,
};

//��sd���е�·��
//��25Qxx�е�·��
const u8 *GBK16_SDPATH="0:/SYSTEM/FONT/GBK16.FON";		//GBK16�Ĵ��λ��
const u8 *GBK12_SDPATH="0:/SYSTEM/FONT/GBK12.FON";		//GBK16�Ĵ��λ��
const u8 *UNIGBK_SDPATH="0:/SYSTEM/FONT/UNIGBK.BIN";	//UNIGBK.BIN�Ĵ��λ��

const u8 *GBK16_25QPATH="1:/SYSTEM/FONT/GBK16.FON";		//GBK16�Ĵ��λ��
const u8 *GBK12_25QPATH="1:/SYSTEM/FONT/GBK12.FON";		//GBK16�Ĵ��λ��
const u8 *UNIGBK_25QPATH="1:/SYSTEM/FONT/UNIGBK.BIN";	//UNIGBK.BIN�Ĵ��λ��

//��ʾ��ǰ������½���
//x,y:����
//size:�����С
//fsize:�����ļ���С
//pos:��ǰ�ļ�ָ��λ��
u32 fupd_prog(u16 x,u16 y,u8 size,u32 fsize,u32 pos)
{
	float prog;
	u8 t=0XFF;
	prog=(float)pos/fsize;
	prog*=100;
	if(t!=prog)
	{
		LCD_ShowString(x+3*size/2,y,240,320,size,"%");		
		t=prog;
		if(t>100)t=100;
		LCD_ShowNum(x,y,t,3,size);//��ʾ��ֵ
	}
	return 0;					    
}

//����ĳһ��
//x,y:����
//size:�����С
//fxpath:·��
//fx:���µ����� 0,ungbk;1,gbk12;2,gbk16;
//����ֵ:0,�ɹ�;����,ʧ��.
u8 updata_fontx(u16 x,u16 y,u8 size,u8 *fxpath,u8 fx)
{
	u32 flashaddr=0;
 	u32 tempsys[2];  //��ʱ��¼�ļ���ʼλ�ú��ļ���С
	FIL * fftemp;
	u8 *tempbuf;
 	u8 res;	
	u16 bread;
	u32 offx=0;
	u8 rval=0;	     
	fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//�����ڴ�	
	if(fftemp==NULL)rval=1;
	tempbuf=mymalloc(SRAMIN,1024);	//����1024���ֽڿռ�
	if(tempbuf==NULL)rval=1;
 	res=f_open(fftemp,(const TCHAR*)fxpath,FA_READ); 
 	if(res)rval=2;//���ļ�ʧ��  
 	if(rval==0)	 
	{
		if(fx==0)//����UNIGBK.BIN
		{
	 		tempsys[0]=ftinfo.ugbkaddr;							//UNIGBK��ŵ�ַ
			tempsys[1]=fftemp->fsize;	 	  					//UNIGBK ��С
			SPI_Flash_Write((u8*)tempsys,FONTINFOADDR+1,8);		//��¼�ڵ�ַFONTINFOADDR+1~8��
			flashaddr=ftinfo.ugbkaddr;
		}else if(fx==1)//GBK12
		{
			SPI_Flash_Read((u8*)tempsys,FONTINFOADDR+1,8);		//����8���ֽ�   
			flashaddr=tempsys[0]+tempsys[1];
			tempsys[0]=flashaddr;								//GBK12����ʼ��ַ
			tempsys[1]=fftemp->fsize;	 	  					//GBK12 ��С
			SPI_Flash_Write((u8*)tempsys,FONTINFOADDR+1+8,8);	//��¼�ڵ�ַFONTINFOADDR+1~8+8��
		}else//GBK16
		{
			SPI_Flash_Read((u8*)tempsys,FONTINFOADDR+1+8,8);    //����8���ֽ�   
			flashaddr=tempsys[0]+tempsys[1];
			tempsys[0]=flashaddr;								//GBK16����ʼ��ַ
			tempsys[1]=fftemp->fsize;	 	  					//GBK16 ��С
			SPI_Flash_Write((u8*)tempsys,FONTINFOADDR+1+16,8);	//��¼�ڵ�ַFONTINFOADDR+1~8+16��
		}	   
		while(res==FR_OK)//��ѭ��ִ��
		{
	 		res=f_read(fftemp,tempbuf,1024,(UINT *)&bread);		//��ȡ����	 
			if(res!=FR_OK)break;								//ִ�д���
			SPI_Flash_Write(tempbuf,offx+flashaddr,1024);		//��0��ʼд��1024������  
	  		offx+=bread;	  
			fupd_prog(x,y,size,fftemp->fsize,offx);	 			//������ʾ
			if(bread!=1024)break;								//������.
	 	} 	
		f_close(fftemp);		
	}			 
	myfree(SRAMIN,fftemp);	//�ͷ��ڴ�
	myfree(SRAMIN,tempbuf);	//�ͷ��ڴ�
	return res;
}

//���������ļ�,UNIGBK,GBK12,GBK16һ�����
//x,y:��ʾ��Ϣ����ʾ��ַ
//size:�����С
//��ʾ��Ϣ�����С
//src:0,��SD������.
//	  1,��25QXX����											  
//����ֵ:0,���³ɹ�;
//		 ����,�������.	  
u8 update_font(u16 x,u16 y,u8 size,u8 src)
{	
	u8 *gbk16_path;
	u8 *gbk12_path;
	u8 *unigbk_path;
	u8 res;		  
	if(src)//��25qxx����
	{
		unigbk_path=(u8*)UNIGBK_25QPATH;
		gbk12_path=(u8*)GBK12_25QPATH;
		gbk16_path=(u8*)GBK16_25QPATH;
	}else//��sd������
	{
		unigbk_path=(u8*)UNIGBK_SDPATH;
		gbk12_path=(u8*)GBK12_SDPATH;
		gbk16_path=(u8*)GBK16_SDPATH;
	}   
 	res=0XFF;
  	SPI_Flash_Write(&res,FONTINFOADDR,1);//���֮ǰ�ı�־.��ֹ���µ�һ������,���µ��ֿⲿ�����ݶ�ʧ.
	LCD_ShowString(x,y,240,320,size,"Updating UNIGBK.BIN");		
	res=updata_fontx(x+20*size/2,y,size,unigbk_path,0);			//����UNIGBK.BIN
	if(res)return 1;
 	LCD_ShowString(x,y,240,320,size,"Updating GBK12.BIN  ");
	res=updata_fontx(x+20*size/2,y,size,gbk12_path,1);			//����GBK12.FON
	if(res)return 2;
	LCD_ShowString(x,y,240,320,size,"Updating GBK16.BIN  ");
	res=updata_fontx(x+20*size/2,y,size,gbk16_path,2);			//����GBK16.FON
	if(res)return 3;	   
	//ȫ�����º���
	res=0XAA;
   	SPI_Flash_Write(&res,FONTINFOADDR,1);//д���ֿ���ڱ�־	0XAA
	return 0;//�޴���.		 
} 
//��ʼ������
//����ֵ:0,�ֿ����.
//		 ����,�ֿⶪʧ
u8 font_init(void)
{			  
	u32 tempsys[2];//��ʱ��¼�ļ���ʼλ�ú��ļ���С
	u8 fontok=0;
	SPI_Flash_Init();
	if(SPI_FLASH_TYPE==W25Q64)FONTINFOADDR=(1024*6+500)*1024;//W25Q64,6M�Ժ�
	else FONTINFOADDR=(1024*2+500)*1024;//W25Q32,2M�Ժ�
	
	ftinfo.ugbkaddr=FONTINFOADDR+25;//UNICODEGBK �����׵�ַ�̶���ַ
	SPI_Flash_Read(&fontok,FONTINFOADDR,1);//�����ֿ��־λ,���Ƿ�����ֿ�
	if(fontok!=0XAA)return 1;//�ֿ����.
 	SPI_Flash_Read((u8*)tempsys,FONTINFOADDR+1,8);//����8���ֽ�   
// 	printf("tempsysgbk[0]:%d\n",tempsys[0]);
//	printf("tempsysgbk[1]:%d\n",tempsys[1]);
	ftinfo.ugbksize=tempsys[1];	//UNIGBK�Ĵ�С

	SPI_Flash_Read((u8*)tempsys,FONTINFOADDR+1+8,8);//����8���ֽ�   
//	printf("tempsysf12[0]:%d\n",tempsys[0]);
//	printf("tempsysf12[1]:%d\n",tempsys[1]);
	ftinfo.f12addr=tempsys[0];	//FONT12����ʼ��ַ
	
	SPI_Flash_Read((u8*)tempsys,FONTINFOADDR+1+16,8);//����8���ֽ�   
//	printf("tempsysf16[0]:%d\n",tempsys[0]);
//	printf("tempsysf16[1]:%d\n",tempsys[1]);
	ftinfo.f16addr=tempsys[0];	//FONT16����ʼ��ַ
	return 0;		    
}





























