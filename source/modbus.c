#include "modbus.h"
#include "uart.h"

static u8 ModbusBusy;      //指示是否有MODBUS命令正在发送
static MMODBUS *pNowOrder;//指示当前指令的描述参数

u16 Calculate_CRC16(u8 *updata, u16 len)
{
    u16 Reg_CRC=0xffff;
    u16 i;
    u8 j;

    for (i=0;i<len;i++)
    {
        Reg_CRC^=*updata++;
        for (j=0;j<8;j++)
        {
            if (Reg_CRC & 0x0001)
            {
               Reg_CRC=Reg_CRC>>1^0XA001;
            }
            else
            {
               Reg_CRC>>=1;
            }
        }
    }
		*updata++ = (u8)Reg_CRC;
		*updata = (u8)(Reg_CRC>>8);
		return Reg_CRC;
}

#if 0 //屏幕做从机
void Uart485RxTreat(void)
{
	u16 len,len1,i;
	u8 tmp[512];
	u16 headtmp;
	
	if(Uart_Struct[UART485].rx_tail != Uart_Struct[UART485].rx_head)
	{
		if(Uart_Struct[UART485].rx_buf[Uart_Struct[UART485].rx_tail]==0)
		{
			EA = 0;
			headtmp = Uart_Struct[UART485].rx_head;
			EA = 1;
            if(headtmp < Uart_Struct[UART485].rx_tail)
            {
                len = (headtmp+SERIAL_SIZE) - Uart_Struct[UART485].rx_tail;
            }
            else
            {
                len = headtmp - Uart_Struct[UART485].rx_tail;
            }
			if(len >= START_TREAT_LENGTH)
			{
				if(Uart_Struct[UART485].rx_buf[(Uart_Struct[UART485].rx_tail+1)&SERIAL_COUNT] == READ_REGISTER)
				{
					if(len>=8)
					{
						for(i=0;i<8;i++)
						{
							tmp[i] = Uart_Struct[UART485].rx_buf[Uart_Struct[UART485].rx_tail];
							Uart_Struct[UART485].rx_tail++;
							Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;
						}
						if(Calculate_CRC16(tmp,8)==0)
						{
				
						}
					}
				}
				else if(Uart_Struct[UART485].rx_buf[(Uart_Struct[UART485].rx_tail+1)&SERIAL_COUNT] == WRITE_REGISTER)
				{
					len1 = Uart_Struct[UART485].rx_buf[(Uart_Struct[UART485].rx_tail+6)&SERIAL_COUNT]+9;
					if(len >= len1)
					{
						for(i=0;i<len1;i++)
						{
							tmp[i] = Uart_Struct[UART485].rx_buf[Uart_Struct[UART485].rx_tail];
							Uart_Struct[UART485].rx_tail++;
							Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;
						}
						if(Calculate_CRC16(tmp,len1)==0)
						{
								Calculate_CRC16(tmp,6);
								Uatr_Send_Data(UART485,8,tmp);
						}
					}
				}
				else 
				{
					Uart_Struct[UART485].rx_tail++;
					Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;
				}
			}
		}
		else
		{
			Uart_Struct[UART485].rx_tail++;
			Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;
		}
	}
}
#else//屏幕做主机
void Uart485RxTreat(void)
{
	u16 len,len1,i,lentmp;
	u8 tmp[512];
	u16 headtmp;
	u8 tmp8;
	
	EA = 0;
	headtmp = Uart_Struct[UART485].rx_head;
	EA = 1;
	if(Uart_Struct[UART485].rx_tail != headtmp)
	{
		if(Uart_Struct[UART485].rx_buf[Uart_Struct[UART485].rx_tail]==pNowOrder->SlaveAddr)//查找当前正在发送命令的modbus地址
		{
            if(headtmp < Uart_Struct[UART485].rx_tail)
            {
                len = (headtmp+SERIAL_SIZE) - Uart_Struct[UART485].rx_tail;
            }
            else
            {
                len = headtmp - Uart_Struct[UART485].rx_tail;
            }
			if(len >= START_TREAT_LENGTH)//缓存到一定长度后才开始处理数据
			{
				if(Uart_Struct[UART485].rx_buf[(Uart_Struct[UART485].rx_tail+1)&SERIAL_COUNT] == pNowOrder->Order)//查找到了地址，并且后面是响应的指令
				{
					if(0x01==pNowOrder->Order)
					{
						len1 = Uart_Struct[UART485].rx_buf[(Uart_Struct[UART485].rx_tail+2)&SERIAL_COUNT];
						lentmp = pNowOrder->Length / 8;
						if((pNowOrder->Length%8)!=0)
							lentmp++;
						if(len1==lentmp)
						{
							if(len >= len1+5)
							{
								for(i=0;i<len1+5;i++)
								{
									tmp[i] = Uart_Struct[UART485].rx_buf[Uart_Struct[UART485].rx_tail];
									Uart_Struct[UART485].rx_tail++;
									Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;
								}
								if(Calculate_CRC16(tmp,len1+5)==0)//校验正确
								{
									ModbusBusy = 0;//标志位变成空闲，可以继续发送下一包数据
									for(i=0;i<pNowOrder->Length;i++)
									{
										tmp8 = 0x01<<(7-i%8);
										if(tmp[3+i/8]&tmp8)
											headtmp = 1;
										else
											headtmp = 0;
										write_dgus_vp(pNowOrder->VPaddr+i,(u8*)&headtmp,1);
									}
								}
							}
						}
						else
						{
							Uart_Struct[UART485].rx_tail++;
							Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;//继续搜索
						}
					}
					else if(0x03==pNowOrder->Order)
					{
						len1 = Uart_Struct[UART485].rx_buf[(Uart_Struct[UART485].rx_tail+2)&SERIAL_COUNT];
						lentmp = pNowOrder->Length << 1;
						if(len1==lentmp)
						{
							if(len >= len1+5)
							{
								for(i=0;i<len1+5;i++)
								{
									tmp[i] = Uart_Struct[UART485].rx_buf[Uart_Struct[UART485].rx_tail];
									Uart_Struct[UART485].rx_tail++;
									Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;
								}
								if(Calculate_CRC16(tmp,len1+5)==0)//校验正确
								{
									ModbusBusy = 0;//标志位变成空闲，可以继续发送下一包数据
									write_dgus_vp(pNowOrder->VPaddr,&tmp[3],pNowOrder->Length);
								}
							}
						}
						else
						{
							Uart_Struct[UART485].rx_tail++;
							Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;//继续搜索
						}
					}
					else if((0x05==pNowOrder->Order)||(0x06==pNowOrder->Order)||(0x10==pNowOrder->Order))
					{
						if(len >= 8)
						{
							for(i=0;i<8;i++)
							{
								tmp[i] = Uart_Struct[UART485].rx_buf[Uart_Struct[UART485].rx_tail];
								Uart_Struct[UART485].rx_tail++;
								Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;
							}
							if(Calculate_CRC16(tmp,8)==0)//校验正确
							{
								if(*(u16*)&tmp[2] == pNowOrder->ModbusReg)
								{
									ModbusBusy = 0;//标志位变成空闲，可以继续发送下一包数据
								}
							}
						}
					}
					else
					{
						Uart_Struct[UART485].rx_tail++;
						Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;//继续搜索
					}
				}
				else 
				{
					Uart_Struct[UART485].rx_tail++;
					Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;//继续搜索
				}
			}
		}
		else
		{
			Uart_Struct[UART485].rx_tail++;
			Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;//继续搜索
		}
	}
}
#endif

u8 AnalysisMosbusOrder(u8 *pBuf)//根据当前指令的格式将要发送的数据放到Pbuf里面，并返回发送长度
{
	u8 len;
	u8 tmp[4];

	len = 0;
	switch (pNowOrder->Order)
	{
		case 0x01://读位状态寄存器
			if(pNowOrder->Length)
			{
				pBuf[0] = pNowOrder->SlaveAddr;//从机地址
				pBuf[1] = 0x01;//功能码
				pBuf[2] = (u8)(pNowOrder->ModbusReg>>8);
				pBuf[3] = (u8)(pNowOrder->ModbusReg);//位寄存器起始地址
				pBuf[4] = 0;
				pBuf[5] = pNowOrder->Length;//位寄存器长度，最大支持255
				len = 6;
			}
			else 
			{
				len = 0;
			}
			break;
		case 0x03://读保持寄存器
			if(pNowOrder->Length)
			{
				pBuf[0] = pNowOrder->SlaveAddr;//从机地址
				pBuf[1] = 0x03;//功能码
				pBuf[2] = (u8)(pNowOrder->ModbusReg>>8);
				pBuf[3] = (u8)(pNowOrder->ModbusReg);//寄存器起始地址
				pBuf[4] = 0;
				pBuf[5] = pNowOrder->Length;//位寄存器长度，最大支持255
				len = 6;
			}
			else 
			{
				len = 0;
			}
			break;
		case 0x05://写单个位寄存器
			pBuf[0] = pNowOrder->SlaveAddr;//从机地址
			pBuf[1] = 0x05;//功能码
			pBuf[2] = (u8)(pNowOrder->ModbusReg>>8);
			pBuf[3] = (u8)(pNowOrder->ModbusReg);//位寄存器起始地址
			read_dgus_vp(pNowOrder->VPaddr,tmp,1);
			if(tmp[1])
				pBuf[4] = 0xff;//位转态为ON
			else
				pBuf[4] = 0;//位状态为OFF
			pBuf[5] = 0x00;
			len = 6;
			break;	
		case 0x06://写单个寄存器
			pBuf[0] = pNowOrder->SlaveAddr;//从机地址
			pBuf[1] = 0x06;//功能码
			pBuf[2] = (u8)(pNowOrder->ModbusReg>>8);
			pBuf[3] = (u8)(pNowOrder->ModbusReg);//寄存器起始地址
			read_dgus_vp(pNowOrder->VPaddr,&pBuf[4],1);
			len = 6;
			break;
		case 0x10://写多个寄存器
			if((pNowOrder->Length>0)&&(pNowOrder->Length<0x7b))
			{
				pBuf[0] = pNowOrder->SlaveAddr;//从机地址
				pBuf[1] = 0x10;//功能码
				pBuf[2] = (u8)(pNowOrder->ModbusReg>>8);
				pBuf[3] = (u8)(pNowOrder->ModbusReg);//寄存器起始地址
				pBuf[4] = 0;
				pBuf[5] = pNowOrder->Length;//位寄存器长度，最大支持0x7b
				pBuf[6] = pNowOrder->Length<<1;//写入寄存器字节数
				len = pBuf[6] + 7;
				read_dgus_vp(pNowOrder->VPaddr,&pBuf[7],pNowOrder->Length);//实际数据
			}
			else
			{
				len = 0;
			}
			break;
		default:
			break;
	}
	if(len)
	{
		Calculate_CRC16(pBuf,len);
		return len+2;//返回发送长度+2字节CRC
	}
	return 0;//0表示本条指令无效
}
