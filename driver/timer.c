/******************************************************************************

                  ��Ȩ���� (C), 2019, �������ĿƼ����޹�˾

 ******************************************************************************
  �� �� ��   : timer.c
  �� �� ��   : V1.0
  ��    ��   : chenmeishu
  ��������   : 2019.9.2
  ��������   : ʵ���˶�ʱ��    Ӳ����ʱ��0    ʵ����8����ʱ��
  �޸���ʷ   :
  1.��    ��   : 
    ��    ��   : 
    �޸�����   : 
******************************************************************************/
#include "timer.h"

//ע�⣺��ֹ˽�Է�����Щ������ֻ���ýӿں�������
//u8  EnableTimer;   //8����ʱ���Ƿ�ʹ��
//u8  OutTimeFlag;   //8����ʱ���Ƿ�ʱ
//u32 TimerTime[8];  //8����ʱ����ʱʱ��
u8  EnablePWM;
u16 PWMTotal[8];
u16 PWMLow[8];
u16 PWMTicks[8];
u8  PWMPort[8];
u8  PWMPin[8];
//ע�⣺��ֹ˽�Է�����Щ������ֻ���ýӿں�������
u8 data EnableTimer;   //8����ʱ���Ƿ�ʹ��
u8 data OutTimeFlag;   //8����ʱ���Ƿ�ʱ
u16 data TimerTime[8];  //8����ʱ����ʱʱ��
u8 data SysTick;
u16 data SysTick_RTC=0;

u8 data encoder_ticks; //
/*****************************************************************************
 �� �� ��  : void T0_Init(void)
 ��������  : ��ʱ��0��ʼ��	��ʱ���1ms
 �������  :	 
 �������  : 
 �޸���ʷ  :
  1.��    ��   : 2019��9��2��
    ��    ��   :  ������
    �޸�����   : ����
*****************************************************************************/

void T0_Init(void)
{
	encoder_ticks = 0; 
    
    
    TMOD=0x11;          //16λ��ʱ��
    //T0
    TH0=0x00;
    TL0=0x00;
    TR0=0x00;
	
	  OutTimeFlag=0;
	  EnableTimer=0;
	
    TMOD|=0x01;
    TH0=T1MS>>8;        //1ms��ʱ��
    TL0=T1MS;
    ET0=1;              //������ʱ��0�ж�
//    EA=1;               //�����ж�
    TR0=1;              //������ʱ��
}
/*****************************************************************************
 �� �� ��  : void T0_ISR_PC(void)    interrupt 1
 ��������  : ��ʱ��0����������������
 �������  :	 
 �������  : 
 �޸���ʷ  :
  1.��    ��   : 2019��9��2��
    ��    ��   : ������
    �޸�����   : ����
*****************************************************************************/
void T0_ISR_PC(void)    interrupt 1
{
	 u8 data i;
	
    EA=0;
    
    if(encoder_ticks != 0) {
        encoder_ticks--;
    }
    
    TH0=T1MS>>8;
    TL0=T1MS;
		SysTick++;
		for(i=0;i<8;i++)
		{
			if(EnableTimer&(0x01<<i))
			{
				TimerTime[i]--;
				if(TimerTime[i]==0)
				{
					OutTimeFlag |= 0x01<<i;
					EnableTimer &= ~(0x01<<i); 					 
				}
			}
		}
    EA=1;
}

void timer2_Isr() interrupt 5
{
	EA = 0;
	TF2 = 0;
	EA = 1;
	SysTick_RTC++;
}


/*****************************************************************************
 �� �� ��  : void StartTimer(u8 ID, u16 nTime)
 ��������  : ����һ����ʱ����
 �������  :	 u8 ID         ��ʱ��ID
               u16 nTime     ��ʱms��

 �������  : 
 �޸���ʷ  :
  1.��    ��   : 2019��9��2��
    ��    ��   : ������
    �޸�����   : ����
*****************************************************************************/
void StartTimer(u8 ID, u16 nTime)
{
	  EA=0;
	  EnableTimer=EnableTimer|(1<<ID);
	  TimerTime[ID]=nTime;
	  OutTimeFlag&=~(1<<ID);
	  EA=1; 
}


/*****************************************************************************
 �� �� ��  : void KillTimer(u8 ID)
 ��������  : ֹͣһ����ʱ����
 �������  :	u8 ID  ��ʱ��ID
 �������  : 
 �޸���ʷ  :
  1.��    ��   : 2019��9��2��
    ��    ��   : ������
    �޸�����   : ����
*****************************************************************************/
void KillTimer(u8 ID)
{
	  EA=0;
	  EnableTimer&=~(1<<(ID));
	  OutTimeFlag&=~(1<<ID);
	  EA=1;
}

/*****************************************************************************
 �� �� ��  : u8 GetTimeOutFlag(u8 ID)
 ��������  : ��ö�ʱ���Ƿ�ʱ
 �������  :	u8 ID  ��ʱ��ID
 �������  :  0  δ��ʱ    ��0  ��ʱ
 �޸���ʷ  :
  1.��    ��   : 2019��9��2��
    ��    ��   : ������
    �޸�����   : ����
*****************************************************************************/
u8 GetTimeOutFlag(u8 ID)
{
  u8 flag;
	EA=0;
	flag=OutTimeFlag&(1<<ID);
	EA=1;
	return flag;
	
}

#ifdef PWMENABLE
void StartPWM(u8 PWMID, u16 VPWMTotal,u16 VPWMLow,u8 Port,u8 Pin )
{
	  EA=0;
	  SetPinOut(Port,Pin);
	  PWMTicks[PWMID]=0;
	  PWMTotal[PWMID]=VPWMTotal;
    PWMLow[PWMID]=VPWMLow;
    PWMPort[PWMID]=Port;
    PWMPin[PWMID]=Pin;	
	  EnablePWM|=(1<<PWMID);
	  EA=1;
}

void StopPWM(u8 PWMID)
{
	  EA=0;
	  EnablePWM&=~(1<<PWMID);	
	  PinOutput(PWMPort[PWMID],PWMPin[PWMID],0);
	  EA=1;
}

void StopAllPWM()
{
	 u8 i;
	 EnablePWM=0;
	 for(i=0;i<8;i++)
	{
		PinOutput(PWMPort[i],PWMPin[i],0);
	}
}
#endif



//

void delayus(unsigned char t)
{	
	char i;
	while(t)
	{	
		for(i=0;i<50;i++)
		{
			i=i;
		}
		t--;
	}
}



void SDA_IN(void)
{	
	P3MDOUT=P3MDOUT&0xF7;
}

void SDA_OUT(void)
{   
	P3MDOUT=P3MDOUT|0x08;
}

//
sbit RTC_SDA = P3^3;
sbit RTC_SCL = P3^2;


void i2cstart(void)
{	
	SDA_OUT();
	RTC_SDA=1;
	RTC_SCL=1;
	delayus(15);

	RTC_SDA=0;
	delayus(15);
	RTC_SCL=0;
	delayus(15);
}

void i2cstop(void)
{	
	SDA_OUT();
	RTC_SDA=0;
	RTC_SCL=1;
	delayus(15);
	RTC_SDA=1;
	delayus(15);
	SDA_IN();
}

void mack(void)
{	
	SDA_OUT();
	RTC_SDA=0;
	delayus(5);
	RTC_SCL=1;
	delayus(5);
	RTC_SCL=0;
	delayus(5);
}

void mnak(void)
{	
	SDA_OUT();
	RTC_SDA=1;
	delayus(5);
	RTC_SCL=1;
	delayus(5);
	RTC_SCL=0;
	delayus(5);
}

void cack(void)
{	
	unsigned char i;
	SDA_IN();
	RTC_SDA=1;
	delayus(5);
	RTC_SCL=1;
	delayus(5);
	for(i=0;i<50;i++)
	{   
		if(!RTC_SDA) 
		{
			break;
		}
		delayus(5);
	}
	RTC_SCL=0;
	delayus(5);
	SDA_OUT();
}

//I2C д��1���ֽ�
void i2cbw(unsigned char dat)
{	
	char i;
	SDA_OUT();
	for(i=0;i<8;i++)
	{	
		if(dat&0x80)
		{
			RTC_SDA=1;
		}
		else 
		{
			RTC_SDA=0;
		}
		dat=(dat<<1);
		delayus(5);
		RTC_SCL=1;
		delayus(5);
		RTC_SCL=0;
		delayus(5);
	}
	cack();
}

//i2c ��ȡ1���ֽ�����
unsigned char i2cbr(void)
{	
	char i;
	unsigned char dat;
	SDA_IN();
	for(i=0;i<8;i++)
	{	
		delayus(5);
		RTC_SCL=1;
		delayus(5);
		dat=(dat<<1);
		if(RTC_SDA) 
		{
			dat=dat|0x01;
		}
		else 
		{
			dat=dat&0xFE;
		}
		//dat=(dat<<1);
		RTC_SCL=0;
		delayus(5);
	}
	return(dat);
}

//���SD2058��û�е��磬�������ʼ��SD2058������ʱ��Ϊ2021.01.01 ������ 00:00:00
void init_rtc(void)
{	
	unsigned char dat1,dat2;
	//�����û�е���
	i2cstart();
	i2cbw(0x64);
	i2cbw(0x0f);		//0x0f���λ
	i2cstart();
	i2cbw(0x65);
	dat2 = i2cbr();
	mack();
	dat1 = i2cbr();
	mnak();
	i2cstop();
	if(dat2&0x01)
	{
		if(dat2&0x84)		//WRTC2 WRTC3�Ƿ�Ϊ0���ǵĻ���д0
		{
			dat2 &= ~0x84;
			i2cstart();		
			i2cbw(0x64);
			i2cbw(0x0f);
			i2cbw(dat2);
			i2cstop();
		}
		dat2 &= ~0x01;
		//��WRTC1�Ƿ�Ϊ0
		if(dat1 & 0x80)		//WRTC1�Ƿ�Ϊ1
		{
			dat1 &= ~0x80;
			i2cstart();		
			i2cbw(0x64);
			i2cbw(0x10);
			i2cbw(dat1);
			i2cstop();
		}
		
		//дʹ��
		dat1 |= 0x80;
		i2cstart();		
		i2cbw(0x64);
		i2cbw(0x10);
		i2cbw(dat1);
		i2cstop();
		dat2 |= 0x84;
		i2cstart();		
		i2cbw(0x64);
		i2cbw(0x0f);
		i2cbw(dat2);
		i2cstop();
		
		
		//��������ʱ��
		i2cstart();		//10-16=RTC����ֵ BCD��ʽ
		i2cbw(0x64);
		i2cbw(0x00);
		i2cbw(0x00);	//��
		i2cbw(0x00);	//��
		i2cbw(0x80);	//ʱ��Сʱ���λ��24-12Сʱ����λ
		i2cbw(0x05);	//����
		i2cbw(0x01);	//��
		i2cbw(0x01);	//��
		i2cbw(0x21);	//��
		i2cstop();
		dat2 &= ~0x84;
		dat1 &= ~0x80;
		i2cstart();		
		i2cbw(0x64);
		i2cbw(0x10);
		i2cbw(dat1);
		i2cstop();
		i2cstart();		
		i2cbw(0x64);
		i2cbw(0x0f);
		i2cbw(dat2);
		i2cstop();
	}
}



void rtc_config(u8* prtc_set)
{
	u8 dat,dat1;
	i2cstart();
	i2cbw(0x64);
	i2cbw(0x0f);		//0x10
	i2cstart();
	i2cbw(0x65);
	dat = i2cbr();
	mack();
	dat1 = i2cbr();
	mnak();
	i2cstop();
	dat1 |= 0x80;
	i2cstart();		
	i2cbw(0x64);
	i2cbw(0x10);
	i2cbw(dat1);
	i2cstop();
	dat |= 0x84;
	i2cstart();		
	i2cbw(0x64);
	i2cbw(0x0f);
	i2cbw(dat);
	i2cstop();
	

	i2cstart();		//10-16=RTC����ֵ BCD��ʽ
	i2cbw(0x64);
	i2cbw(0x00);
	i2cbw(prtc_set[6]);	//��
	i2cbw(prtc_set[5]);	//��
	i2cbw(prtc_set[4]);	//ʱ
	i2cbw(prtc_set[3]);	//����
	i2cbw(prtc_set[2]);	//��
	i2cbw(prtc_set[1]);	//��
	i2cbw(prtc_set[0]);	//��
	i2cstop();
	dat &= ~0x84;
	dat1 &= ~0x80;
	i2cstart();		
	i2cbw(0x64);
	i2cbw(0x10);
	i2cbw(dat1);
	i2cstop();
	i2cstart();		
	i2cbw(0x64);
	i2cbw(0x0f);
	i2cbw(dat);
	i2cstop();
}

//��RTC��ȡ������д��DGUS��Ӧ�ı����ռ䣬��������ÿ0.5�����һ��
void rdtime(void)
{	
	unsigned char rtcdata[8];
	unsigned char i,n,m;
	i2cstart();
	i2cbw(0x64);
	i2cbw(0x00);
	i2cstart();
	i2cbw(0x65);
	for(i=6;i>0;i--)
	{	
		rtcdata[i]=i2cbr();
		mack();
	}
	rtcdata[0]=i2cbr();
	mnak();
	i2cstop();
	rtcdata[4] &= 0x7F;
	for(i=0;i<3;i++)	//������ת����HEX
	{	
		n=rtcdata[i]/16;
		m=rtcdata[i]%16;
		rtcdata[i]=n*10+m;
	}
	for(i=4;i<7;i++)	//ʱ����ת����HEX
	{	
		n=rtcdata[i]/16;
		m=rtcdata[i]%16;
		rtcdata[i]=n*10+m;
	}
	//���ڿ��Բ��ô���
	rtcdata[7]=0;
	write_dgus_vp(0x0010,(u8*)rtcdata,4);	//д��DGUS�����ռ�
}

//

u8 BCD(u8 dat)
{
	  return ((dat/10)<<4)|(dat%10);
}

u8 IBCD(u8 dat)
{
	 return (dat>>4)*10+(dat&0x0f);
}

//



u8 rtc_get_week(u8 year,u8 month,u8 day)
{	
	u16 tmp,mon,y;
	u8 week;
	if((month == 1) || (month == 2))
	{
		mon = month + 12;
		y = year - 1;
	}
	else 
	{
		mon = month;
		y = year;
	}
	tmp = y + (y / 4) +(((mon + 1) * 26) / 10) + day - 36;
	week = tmp % 7;
	return week;
}
//



 void check_rtc_set(void)
 {
 	u8 rtc_parm[8],rtc_set[8];
	read_dgus_vp(0x9c,rtc_parm,4);
	if((rtc_parm[0] == 0x5A) && (rtc_parm[1] == 0xA5))		//����д������
 	{
 		rtc_set[0] = BCD(rtc_parm[2]);
		rtc_set[1] = BCD(rtc_parm[3]);
		rtc_set[2] = BCD(rtc_parm[4]);
		rtc_set[3] = rtc_get_week(rtc_parm[2],rtc_parm[3],rtc_parm[4]);
 		rtc_set[4] = BCD(rtc_parm[5]);
 		rtc_set[4] |= 0x80;
 		rtc_set[5] = BCD(rtc_parm[6]);
 		rtc_set[6] = BCD(rtc_parm[7]);
 		rtc_config(rtc_set);
 		rtc_parm[0] = 0;
 		rtc_parm[1] = 0;
 		write_dgus_vp(0x9c,rtc_parm,1);
 	}
 }


void Clock(void)
{	
	SetPinOut(3,2);
	SetPinOut(3,3);

	
		
			rdtime();								//����Ӳ��RTCʱ��
			SysTick_RTC = 0;
		

	
		check_rtc_set();		//��ָ�����ֱ��д����

}
//










