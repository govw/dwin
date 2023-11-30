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
xdata u8 bufin[100];
xdata u8 bufout[100];
u8 uart_len; 
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
    do
    {
        delay_us(600);
    }
    while((P3 & (1 << 0)) == 0);
    do
    {
        delay_us(600);
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
}


void ext_int1() interrupt 2
{//p3.1
   
    do
    {
        delay_us(600);
    }
    while((P3 & (1 << 1)) == 0);
    do
    {
        delay_us(600);
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
}


xdata par_tek[36];
//dbg
#define SETPAR23(aaa)   if(aaa == 23)                                       \
                        {                                                   \
                        par_tek[23]=0;                                          \
                            if(par_tek[18] == 2) par_tek[23] |= 0x0001;             \
                            if(par_tek[20] == 2) par_tek[23] |= 0x0002;             \
                            if(par_tek[17] == 2) par_tek[23] |= 0x0004;             \
                            if(par_tek[35] == 2) par_tek[23] |= 0x0008;             \
                            if(par_tek[19] == 2) par_tek[23] |= 0x0010;             \
                            if(par_tek[16] == 2) par_tek[23] |= 0x0040;             \
                            if(par_tek[28] == 2) par_tek[23] |= 0x0080;             \
                        }  
void udgu_par_init(void)
{
    par_tek[0]=0x0100;
    par_tek[1]=53;   // t gaz
    par_tek[2]=50;   // I begin
    par_tek[3]=20;    // t begin
    par_tek[4]=33;    // increase
    par_tek[5]=100;  // I базы
    par_tek[6]=2;    // t базы
    par_tek[7]=150;  // I имп
    par_tek[8]=3;    // t имп
    par_tek[9]=100;  // F
    par_tek[10]=6;   // balans
    par_tek[11]=11;   // Decrease
    par_tek[12]=60;  // I end
    par_tek[13]=11;   // t end
    par_tek[14]=3;   // t gaz
    par_tek[15]=55;  // I kz
    par_tek[16]=1;   // BSN 1-off 2-on
    par_tek[17]=1;   // 2T-4T
    par_tek[18]=2;
    par_tek[19]=1;
    par_tek[20]=2;
    par_tek[21]=3;
    par_tek[22]=1;   // sin
    par_tek[23]=0;   // flags
    par_tek[24]=24;
    par_tek[25]=25;
    par_tek[26]=26;
    par_tek[27]=27;
    par_tek[28]=2;   // Oscill  1-our, 2-China
    par_tek[29]=29;
    par_tek[30]=30;
    par_tek[31]=31;
    par_tek[32]=32;
    par_tek[33]=33;
    par_tek[34]=120; // 4T+ - второй ток базы
    par_tek[35]=1;   // 4T+  1-off, 2-on    
}
//dbg




void process_touch(void)
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





void process_uart(void) 
{
    //обработчик uart
      
        if(Uart_Struct[UART5].tx_flag == 0) //если передача завершена
		{
            static u8 buf_idx = 0;
            static u8 local_buf[40];
            EA = 0;	
            while(Uart_Struct[UART5].rx_tail != Uart_Struct[UART5].rx_head)
			{ 
                u8 byte;
                byte = Uart_Struct[UART5].rx_buf[Uart_Struct[UART5].rx_tail];
				Uart_Struct[UART5].rx_tail++;
				Uart_Struct[UART5].rx_tail &= SERIAL_COUNT;
				
                //local_buf[buf_idx++] = byte;

                //if(buf_idx > ARR_SIZE(local_buf))
                    //buf_idx = 0;
               

             

                switch (state)
				{
                    case 0: {
                        buf_idx = 0;//dbg
                        if(byte == 0xA5) 
                            state=1;
                    }break;
                        
                    case 1: {
                        if(byte == 0xA5) 
                            state=4;
                        else 
                            UART_INIT
                    }break;
                        
                    case 4:
                        if(byte == 0xA5) 
                            begfl = 1;
                        else if(byte == 0x5A)
                            endfl = 1;
                        uart_len = byte; 
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

                        if(inbufind == uart_len + 2)
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
                        u16 temp_buf[30];
                        if(byte != 0x5A)
                        {
                            if(byte == 0xA5) 
                                UART_INIT_1 
                            else             
                                UART_INIT
                            break; 
                        }
                

                        for(i = 1; i <= uart_len; i++) {
                            crc += bufin[i];
                        }
                        if(crc != bufin[uart_len+1]) { 
                            UART_INIT
                            write_dgus_vp(0x2500, (u8*) "BAD\0\0\0", 3);
                            break; 
                        }

                       

                       
                        //decode
                        // {//dbg
                        //     static u16 byte_cnt = 0;
                        //     u8 str[20];
                        //     memset(str, 0, 20);
                        //     sprintf(str, "%d\0", byte_cnt++);
                        //     write_dgus_vp(0x2500, (u8*) &str, 10);
                        // }//dbg 
                        switch (bufin[1]) //обработчик номера команды
                        {
                            case 0x25: {
                                u8 ch = bufin[2];
                                if(ch > 36) break;
                                bufout[0]=bufout[1]=0xA5;                           // BEGIN
                                bufout[2]=0x04;                                     // Len
                                bufout[3]=0x80+0x25;                                // INF1
                                bufout[4]=0x25;                                     // Answer code
                                bufout[5]=ch;                                       // Parameter Number
                                bufout[6]=(unsigned char)(par_tek[ch] & 0x00FF);     // Parameter
                                bufout[7]=(unsigned char)(par_tek[ch] >> 8);
                                bufout[8]=bufout[4]+bufout[5]+bufout[6]+bufout[7];  // CRC        
                                bufout[9]=bufout[10]=0x5A;
                                
                                Uatr_Send_Data(UART5, 11, bufout);                            // END
                            } break;
                            
                            case 0x26: {
                                u8 ch=bufin[2];
                                u8 ch1;
                                u8 ch2;
                                if(ch > 36) break;
                                bufout[0]=bufout[1]=0xA5;                           // BEGIN
                                bufout[2]=(ch<<1)+2;                                // Len 
                                bufout[3]=0x80+0x26;                                // INF1
                                bufout[4]=0x26;                                     // Answer code
                                bufout[5]=ch;                                       // Parameters Number
                                ch2=bufout[4]+bufout[5];                            // CRC  
                                    for(ch1=1; ch1<ch; ch1++)
                                    {
                                    SETPAR23(ch1);
                                    bufout[(unsigned char)((ch1<<1)+4)]=(unsigned char)(par_tek[ch1] & 0x00FF);     // Parameter
                                    bufout[(unsigned char)((ch1<<1)+5)]=(unsigned char)(par_tek[ch1] >> 8); 
                                    ch2=ch2+bufout[(unsigned char)((ch1<<1)+4)]+bufout[(unsigned char)((ch1<<1)+5)];
                                    }
                                bufout[(unsigned char)((ch1<<1)+6)]=ch2;                             // CRC
                                bufout[(unsigned char)((ch1<<1)+7)]=bufout[(unsigned char)((ch1<<1)+8)]=0x5A;         // END
                                
                                Uatr_Send_Data(UART5, (unsigned char)((ch1<<1)+9), bufout);                            // END
                            } break;       
                            

                            case 0x33: {
                                code u8 *welding_states_tig[] = {
                                    "Preflow\0\0\0",
                                    "Nothing\0\0\0",
                                    "Ignition\0\0",
                                    "Begin I\0\0\0",
                                    "Increase\0\0",
                                    "Welding\0\0\0",
                                    "Decreaseing\0\0\0",
                                    "End I\0\0\0",
                                    "Cleaning\0\0",
                                    "Flow end\0\0",
                                };

                                if(bufin[2] != 88)
                                    write_dgus_vp(0x2500, (u8*) welding_states_tig[bufin[2] - 1], strlen(welding_states_tig[bufin[2]]) / 2);
                            } break;

                            case 0x3D: {
                               if(bufin[2] == 0x40) {
                                    u16 value = ((unsigned int)(bufin[4]) << 8) + bufin[3];
                                    //u16 value = *(u16*)& bufin[3];
                                    u8 buf[10];
                                    memset(buf, 0, 10);
                                    sprintf(buf, "%d\0\0", value);
                                    write_dgus_vp(0x2500, (u8*) &buf, 5);
                               }        
                            } break;

                            default:
                                break;
                        }
                        //////////////////////////////////////////////////////////////////////////////////////////////////
                        
                        // for(i = 0; i < buf_idx; i++)
                        // {  
                        //     sprintf((u8*) &temp_buf[i], "%02X", (u8)local_buf[i]);                        
                        // }
                        // temp_buf[i] = 0x0000;
                        // write_dgus_vp(0x2500, (u8*) &temp_buf, 10);
                        // buf_idx = 0; 
                        
                        //write_dgus_vp(0x2500, (u8*) "OK\0\0", 2);
                        //////////////////////////////////////////////////////////////////////////////////////////////////
                    
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



void main()
{
	u16 uart_cmd_show_text_sp; //dbg
    
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
    
    //draw_cyclogramm();
    
    make_scene();
    
    
   
    udgu_par_init();
    while(1)    
    {
        process_uart();
        process_touch();
    }
        
      
        


   
}