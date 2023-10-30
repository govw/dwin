#include "timer.h"
#include "ui.h"
#include "uart.h"
#include "dwin8283.h"
#include "canbus.h"
#include "ssd2828.h"


//IO引脚初始化
void io_init()
{
	 PORTDRV = 0x01;//驱动电流为8mA
	//P1MDOUT |= 0x02;//将P1.1设置为输出,用于驱动LED1灯
	//P1MDOUT &= 0xFE;//将P1.0设置为输入,用于读取引脚的电平变化
	
	//sbit csx = P1^0;		//p1.0
	//sbit sdo = P1^1;		//p1.1
	//sbit sdi = P1^2;		//p1.2
	//sbit sck = P1^3;		//p1.3
	//sbit sdc = P1^4;		//p1.4
	
	P1MDOUT = 0xFD;	
}

void main()
{
	INIT_CPU();            //CPU 初始化，注意初始化过程中会把所有xdata变量初始化成0。如果声明的变量有初始值，需要在该函数后面重新赋值
	T0_Init();						//定时器0初始化
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
//			// PageFunction();//每一页对应自己的一个子函数。有界面的开发使用该框架
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



