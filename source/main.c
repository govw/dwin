#include "timer.h"
#include "ui.h"
#include "uart.h"
#include "dwin8283.h"
#include "canbus.h"
#include "ssd2828.h"
#include <string.h> 
#include <stdio.h>


#include "temp.h"
#include "encoder.h"

#define UART_INIT 	{state = inbufind=0;  begfl=endfl=0;} 
#define UART_INIT_1 {state=1; inbufind=0; begfl=endfl=0;}

u8 state, inbufind;
u8 bufin[255];
u8 len; 
u8 begfl,endfl;





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
	P1=0xFD;	
}

 







u8 val; 


void ext_int0() interrupt 0
{ //p3.0
    EA = 0;// моежет и не нужно сдесь
    do
    {
        delay_us(800);
    }
    while((P3 & (1 << 0)) == 0);
    do
    {
        delay_us(800);
    }
    while((P3 & (1 << 0)) == 0);
    do
    {
        delay_us(300);
    }
    while((P3 & (1 << 0)) == 0);
    
   
    
    if((P3 & (1 << 1)) == 0) {
        if(encoder_ticks > 80)
           Process_Encoder(ENC_L1);
        else
            Process_Encoder(ENC_L0);
        encoder_ticks = 100;
    }
    delay_ms(1);
   
    
    IE0 = 0;
    IE1 = 0;
    EA = 1;
    
}


void ext_int1() interrupt 2
{//p3.1
    EA = 0;
    do
    {
        delay_us(800);
    }
    while((P3 & (1 << 1)) == 0);
    do
    {
        delay_us(800);
    }
    while((P3 & (1 << 1)) == 0);
    do
    {
        delay_us(300);
    }
    while((P3 & (1 << 1)) == 0);
    
    
    
    
    if((P3 & (1 << 0)) == 0) {
        if(encoder_ticks > 80) 
            Process_Encoder(ENC_R1);
       else
            Process_Encoder(ENC_R0);
        encoder_ticks = 100;
    }
    delay_ms(1);
   
    
    IE0 = 0;
    IE1 = 0;
    EA = 1;
}



void main()
{
	INIT_CPU();            //CPU ��ʼ����ע���ʼ�������л������xdata������ʼ����0����������ı����г�ʼֵ����Ҫ�ڸú����������¸�ֵ
    T0_Init();						 //��ʱ��0��ʼ��
	EA = 0; //interrupt off

    
  

     //int0
    IP0 &= 0xFE;//Clear bit0
	IP1 &= 0xFE;//Clear bit0
    
	IE0 = 0;//Clear the interrupt flag bit of external interrupt 0
	
	IT0 = 1;//1 is set as falling edge trigger, 0 is set as low level trigger
	EX0 = 1;//Enable external interrupt 0, the corresponding pin is P3.0
    
   
    //int1
    IE1 = 0;
    
    IT1 = 1;
    EX1 = 1;
    EA = 1;   
    

    //StartTimer(0,100);
	//StartTimer(1,50);
	//-------------------
	io_init();
//	delay_ms(20);
	ssd2828_init();
//__________________________________________________________________________________	
	UartInit(UART5, 115200);
	UART_INIT //MARCROS
//__________________________________________________________________________________		  
    
    // draw_cyclogramm();
    
    place_numbers_on_cyclogramm();
    
    
   
   
 
while(1)    
{
   
   enum ETP_STATE 
   {
       ETPS_WAIT,
       ETPS_RELEASE,
       ETPS_FIRST_PRESS,
       ETPS_LIFT,
       ETPS_PRESSING,
   };
   
   static u8 tp_state = ETPS_WAIT;
   
   struct {
       u8 status;
       u8 action_type;
       u16 x;
       u16 y;
   } touch_data;
    

   while(1)
   {    
        read_dgus_vp(0x16,(u8*)&touch_data, sizeof(touch_data) / 2); //info about touch status and coords
        if(touch_data.status == 0x5A) //if status byte 0x05 some action done
        {
            touch_data.status = 0x00;
            write_dgus_vp(0x16, (u8*) &touch_data.x, 1); //clear status byte to 00;
            {
                    u16 action = touch_data.action_type; 
                    write_dgus_vp(0x4000, (u8*) &touch_data.x, 2); //show coords of touch point
                    write_dgus_vp(0x4002, (u8*) &action, 1); //show action type
            }
        
            switch(touch_data.action_type)
            {
                case ETPS_RELEASE:
                {
                    u8 i;
                    for(i = 0; i < cur_menu_size; i++) { //if touch coords in recatnle area                        
                            if( touch_data.x >= cur_menu[i].r.x0 && touch_data.x <= cur_menu[i].r.x1 &&  
                                touch_data.y >= cur_menu[i].r.y0 && touch_data.y <= cur_menu[i].r.y1 ) {
                                    cur_menu_fanc(i);
                                    break;  
                            }
                        }
                        
                        {//для отладки
                            u16 dummy = i;
                            write_dgus_vp(0x4003, (u8*) &dummy, 1); //отобразить код выбранной функции
                        }
                    break;
                }
                    
                case ETPS_FIRST_PRESS:
                {
                    
                        break;
                }
                case ETPS_LIFT:
                    break;

                case ETPS_PRESSING:
                {
                        break;
                }
                default:
                    break;
            }
        }
       
   }
}
    
      
        
  









    
    // while(1)
    // {
    //     //Uatr_Send_Data(UART5, sizeof(dbg_string), dbg_string);  
    //     delay_ms(5000);
    // }
    
   
    while(1)
    {
        //обработчик uart
        //delay_ms(100);
        if(Uart_Struct[UART5].tx_flag == 0) //если передача завершена
		{
            static u8 buf_idx = 0;            
            EA = 0;	
            while(Uart_Struct[UART5].rx_tail != Uart_Struct[UART5].rx_head)
			{ 
                u8 byte;
                byte = Uart_Struct[UART5].rx_buf[Uart_Struct[UART5].rx_tail];
				Uart_Struct[UART5].rx_tail++;
				Uart_Struct[UART5].rx_tail &= SERIAL_COUNT;
				
                //buf[buf_idx] = byte;
                //buf_idx++;
                
                // if(Uart_Struct[UART5].rx_tail == Uart_Struct[UART5].rx_head)
                // {
                //     u8 i;
                //     xdata u16 temp_buf[30];
                //     for(i = 0; i < buf_idx; i++)
                //     {  
                //         sprintf((u8*) &temp_buf[i], "%02X", (u8)buf[i]);                        
                //     } 
                //     temp_buf[i] = 0x0000;
                    
                    
                //     write_dgus_vp(0x1200, (u8*) &temp_buf, (buf_idx * 2) + 2);
                //     buf_idx = 0;
                // }
                
                switch (state)
				{
				case 0:
                        if(byte == 0xA5) 
						state=1;
					break; 
				
				case 1:
					if(byte == 0xA5) 
						state=4;
                    else 
						UART_INIT
                	break; 
				
				case 4:
					if(byte == 0xA5) 
						begfl = 1;
					else if(byte == 0x5A)
                    	endfl = 1;
					len = byte; 
					state = 6; 
                	break;  

				case 6:
					if(byte == 0xA5)
                    {
						if (begfl == 1) 
							state = 4;
                        else           
							begfl = 1; 
                    }
                    else 
						begfl=0;

                    if(byte == 0x5A)
                    {
                        if(endfl == 1) 
						{
							UART_INIT 
							break;
						} else
                        	endfl = 1; 
                    } else 
						endfl = 0;

                	bufin[inbufind] = byte; 
					inbufind++;

                    if(inbufind == len + 2)
                    { 
                         state = 10;
                    } 
						
                	break;

				case 10:
					if(byte == 0x5A) {
                        state = 11; 
						break;
					}    
                    if(byte == 0xA5) {	
						UART_INIT_1 
						break;
					}
					UART_INIT 
					break;

				case 11:
				{
                    u8 i;
					u8 crc = 0;
                    xdata u16 temp_buf[30];
					if(byte != 0x5A)
                    {
                        if(byte == 0xA5) 
							UART_INIT_1 
                        else             
							UART_INIT
                    	break; 
                    }
               

                    for(i = 1; i <= len; i++) {
						crc += bufin[i];
					}
                    if(crc != bufin[len+1]) { 
						UART_INIT
						break; 
					}
                    //decode
                    for(i = 0; i < len; i++)
                    {  
                        sprintf((u8*) &temp_buf[i], "%02X", (u8)bufin[i + 1]);                        
                    } 
                    temp_buf[i] = 0x0000;
                    write_dgus_vp(0x1200, (u8*) &temp_buf, (len * 2) + 2);
                    
                //     
                    //decode
                    // if(bufin[1] == 0x31) {
                    //     u8 i, j;
                    //     u8 temp_buf[30];
                    //     for(i = 2, j = 0; i < len; i++, j++) {
                    //         temp_buf[j] = bufin[i];         
                    //     }
                    //     temp_buf[j] = 0x00;
                    //     print_machine_name(temp_buf);
                    // }
                   
					UART_INIT
					break;
				}
				
				default: 
					break;
			}
			
			
			EA = 1;                
            }
           
            
        }        

    }
    


   
}