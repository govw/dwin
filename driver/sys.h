
/******************************************************************************

                  版权所有 (C), 2019, 北京迪文科技有限公司

 ******************************************************************************
  文 件 名   : sys.h
  版 本 号   : V1.0
  作    者   : chenmeishu
  生成日期   : 2019.9.2
  功能描述   : 
  修改历史   :
  1.日    期   : 
    作    者   : 
    修改内容   : 
******************************************************************************/
#ifndef __SYS_H__
#define __SYS_H__
#include "T5LOS8051.h"
/*****************************************
*根据0X16地址判断滑动以及点击事件
*****************************************/
#define IDLE_PRESS     0//未触摸屏幕
#define FIRST_PRESS    1//首次按下
#define UNDER_PRESS  	 2//持续按压
#define RELEASE_PRESS  3//抬起


#define UNTOUCH 0              //未点击
#define UNSLIDE 1              //未滑动
#define CLICKTOUCH 2           //点击事件
#define VERTICAL_SLIDE 3       //竖向滑动事件
#define HORIZONTAL_SLIDE 4     //横向滑动事件
#define AUTOVERTICAL_SLIDE 5   //竖向滑动事件惯性
#define AUTOHORIZONTAL_SLIDE 6 //横向滑动事件惯性
#define SLIDE_THRESHOLD 9 //XY改变多少像素点算滑动
#define ANGLE_THRESHOLD 2 //横向滑动和纵向滑动的tan值或cot值

/*************************************************************
  memory assignment(0x0000 ~ 0xffff)

  0x0000~0x0fff  system reserved
	0x1000~0xffff  user   area
**********************************************************/
/*****************************************
*	    系统接口变量地址宏定义 (0x0000 0x03ff  )   
*****************************************/
#define		NOR_FLASH				0x0008
#define		SOFT_VERSION		0x000F
#define		RTC						  0x0010
#define		PIC_NOW					0x0014
#define		TP_STATUS				0x0016
#define		LED_NOW					0x0031
#define		AD_VALUE				0x0032
#define   LCD_HOR         0x007A
#define   LCD_VER         0x007B
#define		LED_CONFIG			0x0082
#define		PIC_SET					0x0084
#define 	RTC_Set					0x009C   
#define   RWFLASH         0x00AA
#define   SIMULATE_TP     0x00D4
#define   ENABLE_CURSOR   0x00D8
#define   SYS_CONFIG      0x0080
#define   AUDIO_PLAY      0X00F0


/*****************************************
*			wifi接口变量地址宏定义        *
*****************************************/
#define     WIFI_SWITCH            0x0400
#define		RMA						         0x0401
#define		EQUIPMENT_MODEL			   0x0416
#define		QR_CODE					       0x0450
#define		DISTRIBUTION_NETWORK	 0x0498
#define		MAC_ADDR				       0x0482
#define		WIFI_VER				       0x0487
#define		DISTRIBUTION_STATUS		 0x04A1
#define		NETWORK_STATUS			   0x04A2
#define		RTC_NETWORK				     0x04AC
#define		SSID					         0x04B0
#define		WIFI_PASSWORD			     0x04C0
#define     FLASH_ACCESS_CYCLE     50

/*****************************************
*			中断中是否访问VP      *
*****************************************/
//#define INTVPACTION


/***********************************************************
0x1000     0xffff 用户变量空间
******************************************************/
/*****************************************
*	     NORFLASH(eeprom)    read to RAM ,RAM address          *
*****************************************/
/***************************************************
	UI 
**************************************************/

#define FOSC     206438400UL
#define T1MS    (65536-FOSC/12/1000)

#define   READ_CARD_OK      0
#define   READ_CARD_ERR     1
#define   WITHOUT_CARD      2
#define   DATA_READY        1
typedef   signed   char s8;
typedef   signed	 int  s16;
typedef   signed   long s32;
typedef unsigned   char u8;
typedef unsigned   int  u16;
typedef unsigned   long u32;
#define NULL ((void *)0)
	
typedef struct _mNORFLASH
{
	u8 Mode;   //0x5a=读数据，0XA5等于写数据
	u32 FLAddr;  //flash起始地址，必须为偶数
	u16	VPAddr;	 //VP起始地址，必须为偶数
	u16 Len;		 //操作字长度，必须为偶数
	u8 *Buf;		 //读写数据缓存指针
}MNORFLASH;

typedef struct _mSPIFLASH
{
	u8 Mode;	//5A使能SPI操作
	u16 ID;			//操作的ID号，对于读为16-31
	u32 FLAddr;	//256K中的某个地址，必须为偶数
	u16	VPAddr;	//VP地址，必须为偶数
	u16 Len;		//操作长度
	u8 *Buf;		//数据存放指针
	u16 Delay;	//执行完写操作后，GUI延时时间
}MSPIFLASH;

typedef struct _mMUSIC
{
	u8 IdNmu[32];
	u8 PlayTail;
	u8 PlayHead;
	u8 PlayInterrupt;
}MMUSIC;

extern MNORFLASH Nor_Flash;
extern MSPIFLASH Spi_Flash;
extern MMUSIC MusicBuf;
extern s16 PosXChangeSpeed,PosYChangeSpeed,RealPosX,RealPosY;
extern s16 xdistance,ydistance,PressPosX,PressPosY,LastPosX,LastPosY;
extern u16 PressPageID;
extern u8 PressPosChange;
	
void INIT_CPU(void);
void write_dgus_vp(u16 addr,u8* buf,u16 len);
void read_dgus_vp(u16 addr,u8* buf,u16 len);
void NorFlash_Action(void);
void delay_us(unsigned int t);
void delay_ms(unsigned int t);
void SetPinOut(u8 Port,u8 Pin);
void SetPinIn(u8 Port,u8 Pin);
void PinOutput(u8 Port,u8 Pin,u8 value);
u8   GetPinIn(u8 Port,u8 Pin);
u16  GetPageID();
void Page_Change(u16 PageID);
void SPIFlash_Action(void);
void Music_Play(const u8 *MusicId, u8 len);
void MusicAnction(void);
void TouchSwitch(u16 PageID, u8 TouchType, u8 TouchID, u8 Status);
void MusicDirectPlay(u8 MusicId);
void StrClear(u8 *str1,u16 len);
void StrCopy(u8 *str1,u8 *str2, u16 len);
u8 StrCopare(u8 *str1,u8 *str2, u16 len);
void ResetT5L(void);
void ClearRAM(void);
u8 GetTouchAnction(u16 PageID);
#endif