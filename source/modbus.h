#ifndef  __MODBUS__H___
#define  __MODBUS__H___
#include "sys.h"
typedef struct _mmodbus 
{
	u8 SlaveAddr;              	//从机地址
	u8 Order;					//modbus命令
	u8 Length;					//读写数据长度
	u8 mode;                    //0表示所有页面都触发，1表示指定页面触发，2表示VP内数据是0x5a时执行该命令
	u16 PageIDorVpAddr;         //模式1，模式2有效
	u16 waitTime;				//本条指令执行时间
	u16 VPaddr;                 //本条指令VP起始地址，如果高字节是0XFF，低字节为曲线通道，则读取的数据直接写入曲线缓存区
	u16 ModbusReg;				//本条指令Modbus寄存器起始地址
}MMODBUS;

#define START_TREAT_LENGTH 8
#define READ_REGISTER 0x03
#define WRITE_REGISTER 0x10
#define UART485 2

void Uart485RxTreat(void);
void ModbusCommuMintor(void);
#endif