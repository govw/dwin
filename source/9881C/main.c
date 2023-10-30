#include "timer.h"
#include "ui.h"
#include "uart.h"
#include "dwin8283.h"
#include "canbus.h"
#include "ssd2828.h"


//IO���ų�ʼ��
void io_init()
{
	 PORTDRV = 0x01;//��������Ϊ8mA
	//P1MDOUT |= 0x02;//��P1.1����Ϊ���,��������LED1��
	//P1MDOUT &= 0xFE;//��P1.0����Ϊ����,���ڶ�ȡ���ŵĵ�ƽ�仯
	
	//sbit csx = P1^0;		//p1.0
	//sbit sdo = P1^1;		//p1.1
	//sbit sdi = P1^2;		//p1.2
	//sbit sck = P1^3;		//p1.3
	//sbit sdc = P1^4;		//p1.4
	
	P1MDOUT = 0xFD;	
}

void main()
{
	INIT_CPU();            //CPU ��ʼ����ע���ʼ�������л������xdata������ʼ����0����������ı����г�ʼֵ����Ҫ�ڸú����������¸�ֵ
	T0_Init();						//��ʱ��0��ʼ��
//	Pro8283Init();
	EA = 1;
	StartTimer(0,100);
	StartTimer(1,50);

	
	//-------------------
	io_init();
	delay_ms(20);
	ssd2828_init();
	//-------------------
	
	while(1)
	{
//		if(GetTimeOutFlag(0))
//		{
//			// PageFunction();//ÿһҳ��Ӧ�Լ���һ���Ӻ������н���Ŀ���ʹ�øÿ��
//			StartTimer(0,100);
//		}
//		if(GetTimeOutFlag(1))
//		{
//			Pro8283Deal();
//			StartTimer(1,10);
//			
//			Clock();
//		}	
//	CanErrorReset();
	//	Clock();
	}
}



