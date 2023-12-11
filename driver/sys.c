
/******************************************************************************

                  ��Ȩ���� (C), 2019, �������ĿƼ����޹�˾

 ******************************************************************************
  �� �� ��   : sys.c
  �� �� ��   : V1.0
  ��    ��   : chenmeishu
  ��������   : 2019.9.2
  ��������   : 
  �޸���ʷ   :
  1.��    ��   : 
    ��    ��   : 
    �޸�����   : 
******************************************************************************/
#include "sys.h"
MNORFLASH Nor_Flash;
MSPIFLASH Spi_Flash;
MMUSIC MusicBuf;
s16 PosXChangeSpeed,PosYChangeSpeed,RealPosX,RealPosY;
s16 xdistance,ydistance,PressPosX,PressPosY,LastPosX,LastPosY;
u16 PressPageID;
u8 PressPosChange,Touchstate;

/*****************************************************************************
 �� �� ��  : void read_dgus_vp(u32 addr,u8* buf,u16 len)
 ��������  : ��dgus��ַ��ֵ
 �������  :	 addr��dgus��ֵַ  len�������ݳ���
 �������  : buf�����ݱ��滺����
 �޸���ʷ  :
  1.��    ��   : 2019��4��2��
    ��    ��   : chengjing
    �޸�����   : ����
*****************************************************************************/
#define INTVPACTION
void read_dgus_vp(u16 addr, u8 *buf, u16 len)
{
	u16 OS_addr = 0;
	u16 OS_addr_offset = 0;
	u16 OS_len = 0, OS_len_offset = 0;
	u32 LenLimit;
	
	if(0==len)
		return;
	LenLimit = 0xffffU - addr + 1;
	if(LenLimit < len)
	{
		len = LenLimit;
	}
	OS_addr = addr >> 1;
	OS_addr_offset = addr & 0x01;
#ifdef INTVPACTION	
	EA = 0;
#endif
	ADR_H = 0;
	ADR_M = (u8)(OS_addr >> 8);
	ADR_L = (u8)OS_addr;
	ADR_INC = 1;	
	RAMMODE = 0xAF; 
	while (!APP_ACK);			
	if (OS_addr_offset)
	{
		APP_EN = 1;
		while (APP_EN);
		*buf++ = DATA1;
		*buf++ = DATA0;
		len--;
	}
	OS_len = len >> 1;
	OS_len_offset = len & 0x01;
	while (OS_len--)
	{
		APP_EN = 1;
		while (APP_EN);
		*buf++ = DATA3;
		*buf++ = DATA2;
		*buf++ = DATA1;
		*buf++ = DATA0;
	}
	if (OS_len_offset)
	{
		APP_EN = 1;
		while (APP_EN);
		*buf++ = DATA3;
		*buf++ = DATA2;
	}
	RAMMODE = 0x00;
#ifdef INTVPACTION	
	EA = 1;
#endif
}

/*void read_dgus_vp(u32 addr,u8* buf,u16 len)
{
    u32 xdata OS_addr;
	  u32 xdata OS_addr_offset;
    u16 xdata OS_len;
	  u16 xdata OS_len_offset;
	
		if(addr >= 0x10000)
			return;
		EA = 0;
    OS_addr=addr/2;
    OS_addr_offset=addr%2;
    ADR_H=(u8)(OS_addr>>16)&0xFF;
    ADR_M=(u8)(OS_addr>>8)&0xFF;
    ADR_L=(u8)OS_addr&0xFF;
    ADR_INC=1;                 //DGUS  OS�洢����ַ�ڶ�д���Զ���1
    RAMMODE=0xAF;               //������ģʽ
    if(OS_addr_offset==1)       //�׵�ַ��ƫ�ƣ�����
    {
        while(APP_ACK==0);      //�ȴ�
        APP_EN=1;
        while(APP_EN==1); 
        *buf++=DATA1;
        *buf++=DATA0;              
        len--;
        OS_addr_offset=0;
    }
    OS_len=len/2;
    OS_len_offset=len%2;
    if(OS_len_offset==1)
    {
         OS_len++;
    }
    while(OS_len--)
    {
        if((OS_len_offset==1)&&(OS_len==0))
        {          
            while(APP_ACK==0);
            APP_EN=1;
            while(APP_EN==1);       //��дִ�н���
            *buf++=DATA3;
            *buf++=DATA2;           
            break;    
        } 
        while(APP_ACK==0);
        APP_EN=1;
        while(APP_EN==1);       //��дִ�н��� 
        *buf++=DATA3;
        *buf++=DATA2;
        *buf++=DATA1;
        *buf++=DATA0;                          
    }   
    RAMMODE=0x00;           //��д��ɺ�RAMMODE��������
		EA = 1;
		delay_us(100);
}*/




/*****************************************************************************
 �� �� ��  : void write_dgus_vp(u32 addr,u8* buf,u16 len)
 ��������  : дdgus��ַ����
�������  :	 addr��д��ֵַ	buf��д������ݱ��滺���� len���ֳ���
 �������  : 
 �޸���ʷ  :
  1.��    ��   : 2019��4��2��
    ��    ��   : chengjing
    �޸�����   : ����
*****************************************************************************/
void write_dgus_vp(u16 addr, u8 *buf, u16 len)
{
	
	while (len--) {
		ADR_H = 0x00;
		ADR_M = (u8)( ( addr/2)>>8);
		ADR_L = (u8)(addr/2);
		RAMMODE = 0xAF; 
		while(!APP_ACK); //Операция чтения
		APP_EN = 1; 
		while(APP_EN); 
		RAMMODE = 0;
		ADR_M = ( u8)((addr/2)>>8);
		ADR_L = (u8)(addr/2);
		RAMMODE = 0x8F; 
		while(!APP_ACK); //Операция записи
		if(addr%2){
			DATA1 = *buf++;
			DATA0 = *buf++;
		} else {
			DATA3 = *buf++;
			DATA2 = *buf++;
		}
		addr++; 
		APP_EN = 1; 
		while(APP_EN); 
		RAMMODE = 0;
	}
	
	
	
	

//working
// 	u16 OS_addr = 0;
// 	u16 OS_addr_offset = 0;
// 	u16 OS_len = 0,OS_len_offset = 0;
// 	u16 LenLimit;
	
// 	if(0==len)
// 		return;
// 	LenLimit = 0xffffU - addr + 1;
// 	if(LenLimit < len)
// 	{
// 		len = LenLimit;
// 	}
// 	OS_addr = addr >> 1;
// 	OS_addr_offset = addr & 0x01;
// #ifdef INTVPACTION	
// 	EA = 0;
// #endif
// 	ADR_H = 0;
// 	ADR_M = (u8)(OS_addr >> 8);
// 	ADR_L = (u8)OS_addr;
// 	ADR_INC = 0x01; 
// 	RAMMODE = 0x83;
// 	while (!APP_ACK);
// 	if (OS_addr_offset)
// 	{
// 		DATA1 = *buf++;
// 		DATA0 = *buf++;
// 		APP_EN = 1;
// 		while (APP_EN);
// 		len--;
// 	}
// 	OS_len = len >> 1;
// 	OS_len_offset = len & 0x01;
// 	RAMMODE = 0x8F;
// 	while (OS_len--)
// 	{
// 		DATA3 = *buf++;
// 		DATA2 = *buf++;
// 		DATA1 = *buf++;
// 		DATA0 = *buf++;
// 		APP_EN = 1;
// 		while (APP_EN);
// 	}
// 	if (OS_len_offset)
// 	{
// 		RAMMODE = 0x8C;
// 		DATA3 = *buf++;
// 		DATA2 = *buf++;
// 		APP_EN = 1;
// 		while (APP_EN);
// 	}
// 	RAMMODE = 0x00;
// #ifdef INTVPACTION	
// 	EA = 1;
// #endif
}

/*void write_dgus_vp(u32 addr,u8* buf,u16 len)
{
    u32 xdata OS_addr;
	  u32 xdata OS_addr_offset;
    u16 xdata OS_len;
	  u16 xdata OS_len_offset;
	
		if(addr >= 0x10000)
			return;
    EA=0;
    OS_addr=addr/2;
    OS_addr_offset=addr%2; 
    ADR_H=(u8)(OS_addr>>16)&0xFF;
    ADR_M=(u8)(OS_addr>>8)&0xFF;
    ADR_L=(u8)OS_addr&0xFF;
    ADR_INC=0x01;                 //DGUS  OS�洢����ַ�ڶ�д���Զ���1
    RAMMODE=0x8F;               //����дģʽ 
    if(OS_addr_offset==1)
    {
        ADR_INC=0;
        RAMMODE=0xAF;
        while(APP_ACK==0);
        APP_EN=1;
        while(APP_EN==1);       //��дִ�н���
        ADR_INC=0x01; 
        RAMMODE=0x8F;
        while(APP_ACK==0);      
        DATA1=*buf++;
        DATA0=*buf++;
        APP_EN=1;
        while(APP_EN==1);       //��дִ�н���
        len--;
        OS_addr_offset=0;
    }
    OS_len=len/2;
    OS_len_offset=len%2; 
    if(OS_len_offset==1)
    {
         OS_len++;
    }
    while(OS_len--)
    {
        if((OS_len_offset==1)&&(OS_len==0))
        {
            ADR_INC=0;
            RAMMODE=0xAF;
            while(APP_ACK==0);
            APP_EN=1;                //����һ�������̣�����д����ʱ�Ὣ��һ������д0
            while(APP_EN==1);       //��дִ�н���
            ADR_INC=0x01;
            RAMMODE=0x8F;
            while(APP_ACK==0);           
            DATA3=*buf++;
            DATA2=*buf++;
            APP_EN=1;
            while(APP_EN==1);       //��дִ�н���
            break;
        }
        while(APP_ACK==0);        
        DATA3=*buf++;
        DATA2=*buf++;
        DATA1=*buf++;
        DATA0=*buf++;
        APP_EN=1;
        while(APP_EN==1);       //��дִ�н���
    } 
    RAMMODE=0x00;       //��д��ɺ�RAMMODE��������
    EA=1; 
		delay_us(100);		
}  */


/*****************************************************************************
 �� �� ��  : void INIT_CPU(void)
 ��������  : CPU��ʼ������
			����ʵ��ʹ�������޸Ļ򵥶�����
 �������  :	
 �������  : 
 �޸���ʷ  :
  1.��    ��   : 2019��4��1��
    ��    ��   : chengjing
    �޸�����   : ����
*****************************************************************************/ 
void INIT_CPU(void)
{
    EA=0;
    RS0=0;
    RS1=0;

    CKCON=0x00;
    T2CON=0x70;
	TH2=0X00;
	TL2=0X00;
	TRL2H=0XBC;
	TRL2L=0XCD;
	ET2=1;
	TR2=1;
    DPC=0x00;
    PAGESEL=0x01;
    D_PAGESEL=0x02;   //DATA RAM  0x8000-0xFFFF
    MUX_SEL=0x60;   //UART2,UART2������WDT�ر�
    RAMMODE=0x00;
    PORTDRV=0x01;   //����ǿ��+/-8mA
    IEN0=0x00;      //�ر������ж�
    IEN1=0x00;
    IEN2=0x00;
    IP0=0x00;      //�ж����ȼ�Ĭ��
    IP1=0x00;
/*****************
    WDT_OFF();      //�رտ��Ź�
************************************/
	 ClearRAM();									//��ʼ��RAMΪ0
}

void ClearRAM(void)
{
	u8 xdata*ptr;
	u16 data i;
	
	ptr = 0x8000;
	
	for(i=0;i<0x8000;i++)
		*ptr++ = 0;
}

void ResetT5L(void)
{
	u32 tmp;
	
	tmp = 0x55aa5aa5;
	write_dgus_vp(0x04,(u8*)&tmp,2);
}

void delay_us(unsigned int t)
{ 
 u8 i;

 while(t)
 {
		for(i=0;i<24;i++) {
		i=i;
		}
		t--;
 }
}

void TouchSwitch(u16 PageID, u8 TouchType, u8 TouchID, u8 Status)
{
	u8 k_data[8];
	
	*(u16*)k_data = 0x5aa5;
	*(u16*)&k_data[2] = PageID;
	k_data[4] = TouchID;
	k_data[5] = TouchType;
	if(Status)
		*(u16*)&k_data[6] = 1;
	else
		*(u16*)&k_data[6] = 0;
	write_dgus_vp(0xb0,k_data,4);
	do
	{
		delay_us(500);
		read_dgus_vp(0xb0,k_data,1);
	}while(k_data[0]!=0);
}

void MusicPlay(u8 MusicId)
{
	u8 k_data[4];
	
	read_dgus_vp(0xa0,k_data,2);
	k_data[0] = MusicId;
	k_data[1] = 0x01;
	k_data[3] = 0;
	write_dgus_vp(0xa0,k_data,2);
}

//void StrCopy(u8 *str1,u8 *str2, u16 len)
//{
//	u16 i;
//	
//	if((str1==NULL)||(str2==NULL))
//		return;
//	for(i=0;i<len;i++)
//	{
//		str1[i] = str2[i];
//	}
//}

//u8 StrCopare(u8 *str1,u8 *str2, u16 len)
//{
//	u16 i;
//	
//	if((str1==NULL)||(NULL == str2))
//		return 0;
//	for(i=0;i<len;i++)
//	{
//		if(*str1++ != *str2++)
//			return 1;
//	}
//	return 0;
//}

//void StrClear(u8 *str1,u16 len)
//{
//	u16 i;
//	
//	if(str1==NULL)
//		return;
//	for(i=0;i<len;i++)
//	{
//		*str1++ = 0;
//	}
//}

void NorFlash_Action(void)
{
	u8 buf[8];	
	
	buf[0] = Nor_Flash.Mode;
	buf[1] = (u8)(Nor_Flash.FLAddr>>16);
	buf[2] = (u8)(Nor_Flash.FLAddr>>8);
	buf[3] = (u8)Nor_Flash.FLAddr;
	buf[4] = (u8)(Nor_Flash.VPAddr>>8);
	buf[5] = (u8)Nor_Flash.VPAddr;
	buf[6] = (u8)(Nor_Flash.Len>>8);
	buf[7] = (u8)Nor_Flash.Len;
	if(Nor_Flash.Mode == 0xa5)//д����
	{
		if(Nor_Flash.Buf != NULL)
			write_dgus_vp(Nor_Flash.VPAddr,Nor_Flash.Buf,Nor_Flash.Len);
	}
	write_dgus_vp(0x08,buf,4);
	do
	{
		delay_us(500);
		read_dgus_vp(0x08,buf,1);
	}while(buf[0]!=0);
	if(Nor_Flash.Mode == 0x5a)//������
	{
		if(Nor_Flash.Buf != NULL)
			read_dgus_vp(Nor_Flash.VPAddr,Nor_Flash.Buf,Nor_Flash.Len);
	}
	delay_ms(FLASH_ACCESS_CYCLE);
}

void SPIFlash_Action(void)//����д������Ҫ��ǰ׼����32K��VP����
{
	u8 buf[12];
	
	buf[0] = 0x5a;
	buf[1] = Spi_Flash.Mode;
	if(Spi_Flash.Mode == 1)
	{
		buf[2] = Spi_Flash.ID;
		buf[3] = (u8)(Spi_Flash.FLAddr>>16);
		buf[4] = (u8)(Spi_Flash.FLAddr>>8);
		buf[5] = (u8)Spi_Flash.FLAddr;
		buf[6] = (u8)(Spi_Flash.VPAddr>>8);
		buf[7] = (u8)Spi_Flash.VPAddr;
		buf[8] = (u8)(Spi_Flash.Len>>8);
		buf[9] = (u8)Spi_Flash.Len;
		buf[10] = 0;
		buf[11] = 0;
	}
	else if(Spi_Flash.Mode == 2)
	{
		buf[2] = (u8)(Spi_Flash.ID>>8);
		buf[3] = (u8)Spi_Flash.ID;
		buf[4] = (u8)(Spi_Flash.VPAddr>>8);
		buf[5] = (u8)Spi_Flash.VPAddr;
		buf[6] = (u8)(Spi_Flash.Delay>>8);
		buf[7] = (u8)Spi_Flash.Delay;
		buf[8] = 0;
		buf[9] = 0;
		buf[10] = 0;
		buf[11] = 0;
	}	
	write_dgus_vp(0xaa,buf,6);
	do
	{
		delay_us(500);
		read_dgus_vp(0xaa,buf,1);
	}while(buf[0]!=0);
	if(Spi_Flash.Mode == 1)//������
	{
		if(Spi_Flash.Buf != NULL)
			read_dgus_vp(Spi_Flash.VPAddr,Spi_Flash.Buf,Spi_Flash.Len);
	}
	delay_ms(FLASH_ACCESS_CYCLE);
}

//��õ�ǰҳ��
u16 GetPageID()
{
	  u16  nPage;
	
    read_dgus_vp(PIC_NOW,(u8*)(&nPage),1);	
	  return nPage;
}

void Page_Change(u16 PageID)
{
	u8 buf[4];
  
	buf[0] = 0x5a;
	buf[1] = 0x01;
	buf[2] = (u8)(PageID >> 8);
	buf[3] = (u8)PageID;
  write_dgus_vp(0x84,buf,2);
	do
	{
		delay_us(500);
		read_dgus_vp(0x14,buf,2);
	}while(*(u16*)buf!=PageID);
	buf[0] = 0;
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = 0;
	write_dgus_vp(0x16,buf,2);
}

void delay_ms(u16 t)
{
	u16 i,j;
	  for(i=0;i<t;i++)
	  {
			for(j=0;j<300;j++)
	     delay_us(1);
		}
}

void SetPinOut(u8 Port,u8 Pin)
{
	  switch(Port)
		{
			  case 0: P0MDOUT|=(1<<Pin);
					      break;
				case 1: P1MDOUT|=(1<<Pin);
					      break;
				case 2: P2MDOUT|=(1<<Pin);
					      break;
				case 3: P3MDOUT|=(1<<Pin);
					      break;	
        default:break;				
		}
		
}

void SetPinIn(u8 Port,u8 Pin)
{
	  switch(Port)
		{
			  case 0: P0MDOUT&=~(1<<Pin);
					      break;
				case 1: P1MDOUT&=~(1<<Pin);
					      break;
				case 2: P2MDOUT&=~(1<<Pin);
					      break;
				case 3: P3MDOUT&=~(1<<Pin);
					      break;	
        default:break;				
		}
		
}



void  PinOutput(u8 Port,u8 Pin,u8 value)
{
	  switch(Port)
		{
			  case 0: if(value) P0|=(1<<Pin);
					      else      P0&=~(1<<Pin);
					      break;
				case 1: if(value) P1|=(1<<Pin);
					      else      P1&=~(1<<Pin);
					      break;
				case 2: if(value) P2|=(1<<Pin);
					      else      P2&=~(1<<Pin);
					      break;
				case 3: if(value) P3|=(1<<Pin);
					      else      P3&=~(1<<Pin);
					      break;	
        default:break;				
		}
}



u8 GetPinIn(u8 Port,u8 Pin)
{	 
  	u8 value;
	  switch(Port)
		{
			  case 0: value=P0&(1<<Pin);
					      break;
				case 1: value=P1&(1<<Pin);
					      break;
				case 2: value=P2&(1<<Pin);
					      break;
				case 3: value=P3&(1<<Pin);
					      break;	
        default:
					      value=0;
					    break;				
		}
		return value;
}

u8 GetTouchStatus(void)
{
	u8 k_data[8];
	
	read_dgus_vp(0x16,k_data,4);
	if(k_data[0]==0x5a)
	{
		if(k_data[1]==0x01)//��һ�ΰ���
		{
			LastPosX = PressPosX = *(s16*)&k_data[2];
			LastPosY = PressPosY = *(s16*)&k_data[4];
			Touchstate = FIRST_PRESS;
		}
		else if(k_data[1]==0x03)//����
		{
			LastPosY = RealPosY;
			LastPosX = RealPosX;
			Touchstate = UNDER_PRESS;
		}
		else//̧��
		{
			Touchstate = RELEASE_PRESS;
		}
		RealPosX = *(s16*)&k_data[2];
		RealPosY = *(s16*)&k_data[4];
		k_data[0] = 0;
		write_dgus_vp(0x16,k_data,2);
		return Touchstate;
	}
	else
	{
		if((k_data[1]==0x02)||(k_data[1]==0x00))
		{
			Touchstate = IDLE_PRESS;
		}
		return Touchstate;
	}
}

u8 GetTouchAnction(u16 PageID)
{
	u8 status;
	u32 x2distance,y2distance;
	
	status = GetTouchStatus();
	if(IDLE_PRESS==status)
	{
		PressPosChange = UNTOUCH;
		return PressPosChange;
	}
	else if(FIRST_PRESS==status)//�״ΰ��£���¼����ҳ�棬��ֹ�л�������ҳ�����Ӱ��
	{
		read_dgus_vp(0x14,(u8*)&PressPageID,1);
		PressPosChange = UNSLIDE;
		return UNSLIDE;
	}
	else
	{
		if(PageID != PressPageID)
		{
			return UNSLIDE;
		}
		xdistance = (PressPosX - RealPosX);
		x2distance = xdistance * xdistance;
		ydistance = (PressPosY - RealPosY);
		y2distance = ydistance * ydistance;
		if(UNDER_PRESS==status)//������
		{
			if(UNSLIDE == PressPosChange)//δ����
			{
				if((y2distance >= SLIDE_THRESHOLD)||(x2distance >= SLIDE_THRESHOLD))
				{
					if(y2distance*ANGLE_THRESHOLD >= x2distance)//   Y/X > 0.5����30�Ƚ���Ϊ���򻬶��������򻬶��ķֽ���
					{
						PressPosChange = VERTICAL_SLIDE;
						return VERTICAL_SLIDE;//�������򻬶�
					}
					else//���һ���
					{
						PressPosChange = HORIZONTAL_SLIDE;
						return HORIZONTAL_SLIDE;
					}
				}
			}
			return PressPosChange;
		}
		else//̧��״̬
		{
			if(UNSLIDE == PressPosChange)//δ����
			{
				if((y2distance >= SLIDE_THRESHOLD)||(x2distance >= SLIDE_THRESHOLD))//�����ٻ���ʱ���������������볤��״̬��̧��ʱͬ���ж�һ����������ı����
				{
					if(y2distance*ANGLE_THRESHOLD >= x2distance)//   Y/X > 0.5����30�Ƚ���Ϊ���򻬶��������򻬶��ķֽ���
					{
						PressPosChange = VERTICAL_SLIDE;//�������򻬶�
					}
					else//���һ���
					{
						PressPosChange = HORIZONTAL_SLIDE;
					}
				}
				else
				{
					return CLICKTOUCH;//��������¼�
				}
			}
			if(VERTICAL_SLIDE==PressPosChange)
			{
				PosYChangeSpeed = LastPosY-RealPosY;//�ƶ��ٶȣ�����PosX_ChangeSpeed,PosY_ChangeSpeed��������һ��������̧���������߶γ���
				return AUTOVERTICAL_SLIDE;//�����ƶ��ٶȴ�������
			}
			else
			{
				PosXChangeSpeed = LastPosX-RealPosX;//�ƶ��ٶ�,����
				return AUTOHORIZONTAL_SLIDE;//�����ƶ��ٶȴ�������
			}
		}
	}
}