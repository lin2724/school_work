#include "fontupd.h"
#include "ff.h"	  
#include "flash.h"   
#include "lcd.h"  
#include "malloc.h"

u32 FONTINFOADDR=(1024*6+500)*1024;//默认是6M的地址
//字库信息结构体. 
_font_info ftinfo=
{
	0,  					//UNICODEGBK 表存放首地址固定地址
	0,
	0,
	0,
};

//在sd卡中的路径
//在25Qxx中的路径
const u8 *GBK16_SDPATH="0:/SYSTEM/FONT/GBK16.FON";		//GBK16的存放位置
const u8 *GBK12_SDPATH="0:/SYSTEM/FONT/GBK12.FON";		//GBK16的存放位置
const u8 *UNIGBK_SDPATH="0:/SYSTEM/FONT/UNIGBK.BIN";	//UNIGBK.BIN的存放位置

const u8 *GBK16_25QPATH="1:/SYSTEM/FONT/GBK16.FON";		//GBK16的存放位置
const u8 *GBK12_25QPATH="1:/SYSTEM/FONT/GBK12.FON";		//GBK16的存放位置
const u8 *UNIGBK_25QPATH="1:/SYSTEM/FONT/UNIGBK.BIN";	//UNIGBK.BIN的存放位置

//显示当前字体更新进度
//x,y:坐标
//size:字体大小
//fsize:整个文件大小
//pos:当前文件指针位置
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
		LCD_ShowNum(x,y,t,3,size);//显示数值
	}
	return 0;					    
}

//更新某一个
//x,y:坐标
//size:字体大小
//fxpath:路径
//fx:更新的内容 0,ungbk;1,gbk12;2,gbk16;
//返回值:0,成功;其他,失败.
u8 updata_fontx(u16 x,u16 y,u8 size,u8 *fxpath,u8 fx)
{
	u32 flashaddr=0;
 	u32 tempsys[2];  //临时记录文件起始位置和文件大小
	FIL * fftemp;
	u8 *tempbuf;
 	u8 res;	
	u16 bread;
	u32 offx=0;
	u8 rval=0;	     
	fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//分配内存	
	if(fftemp==NULL)rval=1;
	tempbuf=mymalloc(SRAMIN,1024);	//分配1024个字节空间
	if(tempbuf==NULL)rval=1;
 	res=f_open(fftemp,(const TCHAR*)fxpath,FA_READ); 
 	if(res)rval=2;//打开文件失败  
 	if(rval==0)	 
	{
		if(fx==0)//更新UNIGBK.BIN
		{
	 		tempsys[0]=ftinfo.ugbkaddr;							//UNIGBK存放地址
			tempsys[1]=fftemp->fsize;	 	  					//UNIGBK 大小
			SPI_Flash_Write((u8*)tempsys,FONTINFOADDR+1,8);		//记录在地址FONTINFOADDR+1~8处
			flashaddr=ftinfo.ugbkaddr;
		}else if(fx==1)//GBK12
		{
			SPI_Flash_Read((u8*)tempsys,FONTINFOADDR+1,8);		//读出8个字节   
			flashaddr=tempsys[0]+tempsys[1];
			tempsys[0]=flashaddr;								//GBK12的起始地址
			tempsys[1]=fftemp->fsize;	 	  					//GBK12 大小
			SPI_Flash_Write((u8*)tempsys,FONTINFOADDR+1+8,8);	//记录在地址FONTINFOADDR+1~8+8处
		}else//GBK16
		{
			SPI_Flash_Read((u8*)tempsys,FONTINFOADDR+1+8,8);    //读出8个字节   
			flashaddr=tempsys[0]+tempsys[1];
			tempsys[0]=flashaddr;								//GBK16的起始地址
			tempsys[1]=fftemp->fsize;	 	  					//GBK16 大小
			SPI_Flash_Write((u8*)tempsys,FONTINFOADDR+1+16,8);	//记录在地址FONTINFOADDR+1~8+16处
		}	   
		while(res==FR_OK)//死循环执行
		{
	 		res=f_read(fftemp,tempbuf,1024,(UINT *)&bread);		//读取数据	 
			if(res!=FR_OK)break;								//执行错误
			SPI_Flash_Write(tempbuf,offx+flashaddr,1024);		//从0开始写入1024个数据  
	  		offx+=bread;	  
			fupd_prog(x,y,size,fftemp->fsize,offx);	 			//进度显示
			if(bread!=1024)break;								//读完了.
	 	} 	
		f_close(fftemp);		
	}			 
	myfree(SRAMIN,fftemp);	//释放内存
	myfree(SRAMIN,tempbuf);	//释放内存
	return res;
}

//更新字体文件,UNIGBK,GBK12,GBK16一起更新
//x,y:提示信息的显示地址
//size:字体大小
//提示信息字体大小
//src:0,从SD卡更新.
//	  1,从25QXX更新											  
//返回值:0,更新成功;
//		 其他,错误代码.	  
u8 update_font(u16 x,u16 y,u8 size,u8 src)
{	
	u8 *gbk16_path;
	u8 *gbk12_path;
	u8 *unigbk_path;
	u8 res;		  
	if(src)//从25qxx更新
	{
		unigbk_path=(u8*)UNIGBK_25QPATH;
		gbk12_path=(u8*)GBK12_25QPATH;
		gbk16_path=(u8*)GBK16_25QPATH;
	}else//从sd卡更新
	{
		unigbk_path=(u8*)UNIGBK_SDPATH;
		gbk12_path=(u8*)GBK12_SDPATH;
		gbk16_path=(u8*)GBK16_SDPATH;
	}   
 	res=0XFF;
  	SPI_Flash_Write(&res,FONTINFOADDR,1);//清除之前的标志.防止更新到一半重启,导致的字库部分数据丢失.
	LCD_ShowString(x,y,240,320,size,"Updating UNIGBK.BIN");		
	res=updata_fontx(x+20*size/2,y,size,unigbk_path,0);			//更新UNIGBK.BIN
	if(res)return 1;
 	LCD_ShowString(x,y,240,320,size,"Updating GBK12.BIN  ");
	res=updata_fontx(x+20*size/2,y,size,gbk12_path,1);			//更新GBK12.FON
	if(res)return 2;
	LCD_ShowString(x,y,240,320,size,"Updating GBK16.BIN  ");
	res=updata_fontx(x+20*size/2,y,size,gbk16_path,2);			//更新GBK16.FON
	if(res)return 3;	   
	//全部更新好了
	res=0XAA;
   	SPI_Flash_Write(&res,FONTINFOADDR,1);//写入字库存在标志	0XAA
	return 0;//无错误.		 
} 
//初始化字体
//返回值:0,字库完好.
//		 其他,字库丢失
u8 font_init(void)
{			  
	u32 tempsys[2];//临时记录文件起始位置和文件大小
	u8 fontok=0;
	SPI_Flash_Init();
	if(SPI_FLASH_TYPE==W25Q64)FONTINFOADDR=(1024*6+500)*1024;//W25Q64,6M以后
	else FONTINFOADDR=(1024*2+500)*1024;//W25Q32,2M以后
	
	ftinfo.ugbkaddr=FONTINFOADDR+25;//UNICODEGBK 表存放首地址固定地址
	SPI_Flash_Read(&fontok,FONTINFOADDR,1);//读出字库标志位,看是否存在字库
	if(fontok!=0XAA)return 1;//字库错误.
 	SPI_Flash_Read((u8*)tempsys,FONTINFOADDR+1,8);//读出8个字节   
// 	printf("tempsysgbk[0]:%d\n",tempsys[0]);
//	printf("tempsysgbk[1]:%d\n",tempsys[1]);
	ftinfo.ugbksize=tempsys[1];	//UNIGBK的大小

	SPI_Flash_Read((u8*)tempsys,FONTINFOADDR+1+8,8);//读出8个字节   
//	printf("tempsysf12[0]:%d\n",tempsys[0]);
//	printf("tempsysf12[1]:%d\n",tempsys[1]);
	ftinfo.f12addr=tempsys[0];	//FONT12的起始地址
	
	SPI_Flash_Read((u8*)tempsys,FONTINFOADDR+1+16,8);//读出8个字节   
//	printf("tempsysf16[0]:%d\n",tempsys[0]);
//	printf("tempsysf16[1]:%d\n",tempsys[1]);
	ftinfo.f16addr=tempsys[0];	//FONT16的起始地址
	return 0;		    
}





























