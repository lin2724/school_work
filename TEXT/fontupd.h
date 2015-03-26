#ifndef __FONTUPD_H__
#define __FONTUPD_H__	 
#include "sys.h"

//前面2M/4M被fatfs占用了.
//2M/4M以后紧跟的500K字节,用户可以随便用.
//2M/4M+500K字节以后的字节,被字库占用了,不能动!
//字体信息保存地址,占25个字节,第1个字节用于标记字库是否存在.后续每8个字节一组,分别保存起始地址和文件大小														   
extern u32 FONTINFOADDR;	
//字库信息结构体定义
__packed typedef struct 
{
	u32 ugbkaddr; 			//unigbk的地址
	u32 f16addr;			//gbk16地址
	u32 f12addr;			//gbk12地址	
	u32 ugbksize;			//unigbk的大小	 
}_font_info;																   

extern _font_info ftinfo;	//字库信息结构体

u32 fupd_prog(u16 x,u16 y,u8 size,u32 fsize,u32 pos);//显示更新进度
u8 updata_fontx(u16 x,u16 y,u8 size,u8 *fxpath,u8 fx);//更新指定字库
u8 update_font(u16 x,u16 y,u8 size,u8 src);//更新全部字库
u8 font_init(void);//初始化字库
#endif





















