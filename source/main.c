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

static data u8 encoder_state_adsfl;

void ext_int0() interrupt 0
{ //p3.0
    do {
        delay_us(600);
    } while((P3 & (1 << 0)) == 0);
    do {
        delay_us(600);
    } while((P3 & (1 << 0)) == 0);
    do {
        delay_us(300);
    } while((P3 & (1 << 0)) == 0);
    
    if((P3 & (1 << 1)) == 0) {
        if(encoder_ticks > 80)
            encoder_state_adsfl = ENC_L1;
           // Process_Encoder(ENC_L1);
        else
            encoder_state_adsfl = ENC_L0;
            //Process_Encoder(ENC_L0);
        encoder_ticks = 100;
    }
    delay_ms(1);
   
    
    IE0 = 0;
    IE1 = 0;
}


void ext_int1() interrupt 2
{//p3.1
   
    do {
        delay_us(600);
    } while((P3 & (1 << 1)) == 0);
    do {
        delay_us(600);
    } while((P3 & (1 << 1)) == 0);
    do {
        delay_us(300);
    } while((P3 & (1 << 1)) == 0);
    
    if((P3 & (1 << 0)) == 0) {
        if(encoder_ticks > 80) 
            encoder_state_adsfl = ENC_R1;
            //Process_Encoder(ENC_R1);
       else
            encoder_state_adsfl = ENC_R0;
            //Process_Encoder(ENC_R0);
        encoder_ticks = 100;
    }
    delay_ms(1);
   
    
    IE0 = 0;
    IE1 = 0;
}


xdata par_tek[36];
//dbg
#define SETPAR23(aaa)   if(aaa == 23)                                               \
                        {                                                           \
                            par_tek[23]=0;                                          \
                            if(par_tek[18] == 2) par_tek[23] |= 0x0001;             \
                            if(par_tek[20] == 2) par_tek[23] |= 0x0002;             \
                            if(par_tek[17] == 2) par_tek[23] |= 0x0004;             \
                            if(par_tek[35] == 2) par_tek[23] |= 0x0008;             \
                            if(par_tek[19] == 2) par_tek[23] |= 0x0010;             \
                            if(par_tek[16] == 2) par_tek[23] |= 0x0040;             \
                            if(par_tek[28] == 2) par_tek[23] |= 0x0080;             \
                        }   


enum PAR_ID{
    EPID_MODES_SUBMODES,//0    режимы/подрежимы
    EPID_PRE_FLOW_T1,   //1    Т1  время предв. продувки, с
    EPID_START_I1,      //2    I1  начальный ток, А
    EPID_START_T2,      //3    T2  время начального тока, c
    EPID_UP_T3,         //4    Т3  время нарастания, с
    EPID_BASE_I2,       //5    I2  сварочный ток (ток базы), А
    EPID_BASE_T4,       //6    Т4  время тока базы, с
    EPID_IMPULSE_I3,    //7    I3  импульсный ток, А
    EPID_IMPULSE_T5,    //8    Т5  время тока импульса, с
    EPID_FREQ_F1,       //9    F1  частота, Гц
    EPID_BALANCE_D1,    //10   D1  баланс *
    EPID_DOWN_T6,       //11   Т6  время спада, с
    EPID_END_I4,        //12   I4  ток окончания сварки, А
    EPID_END_T7,        //13   T7  время конечного тока, с
    EPID_POST_FLOW_T8,  //14   Т8  время продувки в конце, с
    EPID_KZ_I5,         //15   I5  ток короткого замыкания, %
    EPID_BSN,           //16   БСН - вкл/выкл
    EPID_2T_4T,         //17   2t/4t
    EPID_IGNITION_TYPE, //18   Вид поджига - контактный/бесконтактный
    EPID_FLOW_SENSOR,   //19   Опрос датчика протока - вкл/выкл
    EPID_POLARITY,      //20   Полярность DC+/DC-
    EPID_WAVE_FORM,     //22   Тип огибающей
    EPID_ELECTRODE_D,   //21   Диаметр электрода, мм
    EPID_SERVICE_PAR,   //23   Служебные параметры (битовые) 
    EPID_PLUS_PULSE,    //24   + Pulse Between 
    EPID_MINUS_PULSE,   //25   - Pulse Between
    EPDI_I_MIN_MINUS,   //26   Imin - 
    EPID_I_MIN_PLUS,    //27   Imin + 
    EPID_OSCIL_TYPE,    //28   Тип осциллятора Китай/Завод    
    EPID_KC_RATIO,      //29   Коэффициент Кс 
    EPID_I_IGN_PULSE,   //30   I0  ток импульса поджига, А
    EPID_I_CLEAN,       //31   Ток очистки, А 
    EPID_T_CLEAN,       //32   Время очистки, мс
    EPID_IMPULSE_CNT,   //33   Импульсы подпитки, шт
    EPID_BASE2_I2X,     //I2х второй ток базы (только в 4Т+), А
    EPID_IS_PLUS_4T,    //35   плюс/не плюс (для 4Т)
};

void udgu_par_init(void)
{
    par_tek[EPID_MODES_SUBMODES] = 0x0100;
    par_tek[EPID_PRE_FLOW_T1]    = 53;   // t gaz
    par_tek[EPID_START_I1]       = 50;   // I begin
    par_tek[EPID_START_T2]       = 20;   // t begin
    par_tek[EPID_UP_T3]          = 33;   // increase
    par_tek[EPID_BASE_I2]        = 100;  // I базы
    par_tek[EPID_BASE_T4]        = 2;    // t базы
    par_tek[EPID_IMPULSE_I3]     = 150;  // I имп
    par_tek[EPID_IMPULSE_T5]     = 3;    // t имп
    par_tek[EPID_FREQ_F1]        = 100;  // F
    par_tek[EPID_BALANCE_D1]     = 6;   // balans
    par_tek[EPID_DOWN_T6]        = 11;  // Decrease
    par_tek[EPID_END_I4]         = 60;   // I end
    par_tek[EPID_END_T7]         = 11;   // t end
    par_tek[EPID_POST_FLOW_T8]   = 3;   // t gaz
    par_tek[EPID_KZ_I5]          = 55;  // I kz
    par_tek[EPID_BSN]            = 1;   // BSN 1-off 2-on
    par_tek[EPID_2T_4T]          = 1;   // 2T-4T
    par_tek[EPID_IGNITION_TYPE]  = 2;
    par_tek[EPID_FLOW_SENSOR]    = 1;
    par_tek[EPID_POLARITY]       = 2;
    par_tek[EPID_ELECTRODE_D]    = 3;
    par_tek[EPID_SERVICE_PAR]    = 1;   // sin
    par_tek[23]=0;                    // flags
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

//debug
static data u8  welding_state;
static data u16 amp;
static data u16 volt;
static data u16 time;
static data u16 touch_coord_x;
static data u16 touch_coord_y;
static data u16 touch_state;
static data u16 last_touch_item;
static u16 good_packages_cnt;
static u16 bad_packages_cnt;
void print_debug_info(void)
{
    static u8 buf[100];
    code u8 *welding_states_tig[] = {
        "Preflow", "Nothing", "Ignition", "Begin I", "Increase", "Welding", "Decreaseing",
        "End I", "Cleaning", "Flow end",
    };

    //memset(buf, 0, ARR_SIZE(buf));

    sprintf(buf, "%s               %d %d %d %d\n\r%dA\n\r%.1fV\n\r%.1fs\n\rgood: %d\n\rbad: %d",welding_states_tig[welding_state], touch_coord_x, touch_coord_y, touch_state, last_touch_item, amp, (float)volt / 10.0, (float)time / 10.0, good_packages_cnt, bad_packages_cnt);
    write_dgus_vp(0x2500, (u8*) &buf, sizeof(buf) / 2);  
}
//debug









u16 ind_par_to_power_par(u8 power_par_id)
{
    switch (power_par_id) {
        case EPID_MODES_SUBMODES: {
            struct {
                u16 d0    : 4; //пусто, не используется.   
                u16 pulse : 4; //1-пульс 2-не пульс
                u16 pol   : 4; //1-DC  2-AC  3-MIX
                u16 mode  : 4; //1-TIG 2-MMA 3-TIG Spot
            } modes_submodes;
            
            modes_submodes.d0 = 0;
            if(main_menu_bm & (u32)1 << EIID_PULSE_ON)
                modes_submodes.pulse = 1; //pulse
            else 
                modes_submodes.pulse = 2; //no pulse

            if((main_menu_bm & (u32)1 << EIID_DC_MINUS) || (main_menu_bm & (u32)1 << EIID_DC_PLUS)) {
                modes_submodes.pol = 1; //DC
            } else if(main_menu_bm & (u32)1 << EIID_AC) {
                modes_submodes.pol = 2; //AC
            } else if(main_menu_bm & (u32)1 << EIID_AC_MIX) {
                modes_submodes.pol = 3; //MIX
            }

            if(main_menu_bm & (u32)1 << EIID_TIG) {
                modes_submodes.mode = 1; //1 - TIG
            } else if (main_menu_bm & (u32)1 << EIID_TIG_SPOT) {
                modes_submodes.mode = 3; //2 - MMA
            } else if (main_menu_bm & (u32)1 << EIID_MMA) {
                modes_submodes.mode = 2; //3 - TIG Spot
            }
            return (*(u16*)&modes_submodes);
        } break;

        case EPID_PRE_FLOW_T1:  return par[EIID_PRE_FLOW_T1];
        case EPID_START_I1:     return par[EIID_START_I1];
        case EPID_START_T2:     return par[EIID_START_T2];
        case EPID_UP_T3:        return par[EIID_UP_T3];
        case EPID_BASE_I2:      return par[EIID_BASE_I2];
        case EPID_BASE_T4:      return par[EIID_BASE_T4];
        case EPID_IMPULSE_I3:   return par[EIID_IMPULSE_I3];
        case EPID_IMPULSE_T5:   return par[EIID_IMPULSE_T5];
        case EPID_FREQ_F1:      return par[EIID_FREQ_F1];
        case EPID_BALANCE_D1:   return par[EIID_BALANCE_D1];
        case EPID_DOWN_T6:      return par[EIID_DOWN_T6];
        case EPID_END_I4:       return par[EIID_END_I4];
        case EPID_END_T7:       return par[EIID_END_T7];
        case EPID_POST_FLOW_T8: return par[EIID_POST_FLOW_T8];
        case EPID_KZ_I5:        return par[EIDD_KZ_I5];
        case EPID_BASE2_I2X:    return par[EIID_BASE2_I2X];
        
        case EPID_BSN: {
            return 0;
        } break;
          
        case EPID_2T_4T: {
            if(main_menu_bm & (u32)1 << EIID_2T)
                return 1; //2t
            else
                return 2; //4t
        } break;
         
        case EPID_IGNITION_TYPE: {//Вид поджига - контактный/бесконтактный
            if(main_menu_bm & (u32)1 << EIID_LIFT)
                return 1; //lif
            else
                return 2; //hf
        } break;
 
        case EPID_FLOW_SENSOR: {
            if(main_menu_bm & (u32)1 << EIID_H2O_OFF)
                return 1; //OFF
            else
                return 2; //ON
        } break;
  
        case EPID_POLARITY: {
            if(main_menu_bm & (u32)1 << EIID_DC_PLUS)
                return 1; //DC+
            else
                return 2; //DC-
        } break;
    
        case EPID_ELECTRODE_D: {
            if(main_menu_bm & (u32)1 << EIID_D_10)
                return 1; 
            else if(main_menu_bm & (u32)1 << EIID_D_16)
                return 2; 
            else if(main_menu_bm & (u32)1 << EIID_D_20)
                return 3;
            else if(main_menu_bm & (u32)1 << EIID_D_24)  
                return 4;
            else if(main_menu_bm & (u32)1 << EIID_D_32)  
                return 5;
            else if(main_menu_bm & (u32)1 << EIID_D_40)  
                return 6;
        } break;
   
        case EPID_WAVE_FORM: {
            if(main_menu_bm & (u32)1 << EIID_SIN)
                return 1; 
            else if(main_menu_bm & (u32)1 << EIID_RECT)
                return 2; 
            else if(main_menu_bm & (u32)1 << EIID_TRIG)
                return 3;
            else if(main_menu_bm & (u32)1 << EIID_TRAP)  
                return 4;
        } break;
     
        case EPID_SERVICE_PAR: {
            return 1;
        } break;
   
        case EPID_PLUS_PULSE: 
        case EPID_MINUS_PULSE:
        case EPDI_I_MIN_MINUS: 
        case EPID_I_MIN_PLUS: 
        case EPID_KC_RATIO: 
        case EPID_I_IGN_PULSE: 
        case EPID_I_CLEAN: 
        case EPID_T_CLEAN: 
        case EPID_IMPULSE_CNT: {
            return 0;
        } break;

        case EPID_OSCIL_TYPE: return 1; //китайский осцилятор 
        
        case EPID_IS_PLUS_4T: {
            if(main_menu_bm & (u32)1 << EIID_4T_PLUS)
                return 1; 
            else 
                return 2; 
        } break;
    
    }
}





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
           
            //debug
            touch_coord_x = touch_data.x;
            touch_coord_y = touch_data.y;
            touch_state   =  touch_data.action_type;
            //debug
            
        
            switch(touch_data.action_type)
            {
                case ETPS_RELEASE: {
                    u8 i;
                    for(i = 0; i < cur_menu_size; i++) { //if touch coords in recatnle area                        
                            if( touch_data.x >= cur_menu[i].r.x0 && touch_data.x <= cur_menu[i].r.x1 &&  
                                touch_data.y >= cur_menu[i].r.y0 && touch_data.y <= cur_menu[i].r.y1 ) {
                                    cur_menu_fanc(i);
                                    break;  
                            }
                        }
                        last_touch_item = i; //debug
                } break;
                case ETPS_FIRST_PRESS: break;
                case ETPS_LIFT:        break;
                case ETPS_PRESSING:    break;
                default: break;
            }
        }
}





void process_uart(void) 
{
    //обработчик uart
        if(Uart_Struct[UART5].tx_flag == 0) //если передача завершена
		{
            while(Uart_Struct[UART5].rx_tail != Uart_Struct[UART5].rx_head)
			{ 
                u8 byte = Uart_Struct[UART5].rx_buf[Uart_Struct[UART5].rx_tail];
                
				Uart_Struct[UART5].rx_tail++;
				Uart_Struct[UART5].rx_tail &= SERIAL_COUNT;
				

                switch (state)
				{
                    case 0: {
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
                            bad_packages_cnt++;//dbg
                            break; 
                        }

                        good_packages_cnt++; //dbg
                    
                        { 
                        switch (bufin[1]) //обработчик номера команды
                        {
                            case 0x25: {
                                u8 ch = bufin[2];
                                u16 par_to_send;
                                if(ch > 36) break;
                                bufout[0] = 0xA5;      // BEGIN
                                bufout[1] = 0xA5;                           
                                bufout[2] = 0x04;      // Len
                                bufout[3] = 0x80+0x25; // INF1
                                bufout[4] = 0x25;      // Answer code
                                par_to_send = bufout[5] = ch;        // Parameter Number
                                par_to_send = ind_par_to_power_par(ch);
                                bufout[6] = (unsigned char)(par_to_send & 0x00FF);     // Parameter
                                bufout[7] = (unsigned char)(par_to_send >> 8);
                                bufout[8] = bufout[4]+bufout[5]+bufout[6]+bufout[7];  // CRC        
                                bufout[9] = bufout[10]=0x5A;
                                Uatr_Send_Data(UART5, 11, bufout);                            // END
                            } break;
                            
                            case 0x26: {//запрос параметров в с 0 по по bufin[2]
                                u8 ch=bufin[2]; // по какой параметр
                                u8 ch1;
                                u8 ch2;
                                if(ch > 36) break;
                                bufout[0] = bufout[1] = 0xA5;                         // BEGIN
                                bufout[2] = (ch<<1) + 2;                              // Len 
                                bufout[3] = 0x80+0x26;                                // INF1
                                bufout[4] = 0x26;                                     // Answer code
                                bufout[5] = ch;                                       // Parameters Number
                                ch2=bufout[4]+bufout[5];                              // CRC  
                                    for(ch1 = 0; ch1<ch; ch1++) {
                                        u16 par_to_send;
                                        par_to_send = ind_par_to_power_par(ch1);
                                        SETPAR23(ch1);
                                        bufout[(unsigned char)((ch1<<1)+4)] = (unsigned char)(par_to_send & 0x00FF);
                                        bufout[(unsigned char)((ch1<<1)+5)] = (unsigned char)(par_to_send >> 8);   
                                        //bufout[(unsigned char)((ch1<<1)+4)] = (unsigned char)(par_tek[ch1] & 0x00FF);     // Parameter
                                        //bufout[(unsigned char)((ch1<<1)+5)] = (unsigned char)(par_tek[ch1] >> 8); 
                                        ch2=ch2+bufout[(unsigned char)((ch1<<1)+4)]+bufout[(unsigned char)((ch1<<1)+5)];
                                    }
                                bufout[(unsigned char)((ch1<<1)+6)]=ch2;                             // CRC
                                bufout[(unsigned char)((ch1<<1)+7)]=bufout[(unsigned char)((ch1<<1)+8)]=0x5A;         // END
                                
                                Uatr_Send_Data(UART5, (unsigned char)((ch1<<1)+9), bufout);                            // END
                            } break;       

                            case 0x33: {
                                if(bufin[2] != 88)
                                    welding_state = bufin[2] - 1;//dbg
                            } break;

                            case 0x3D: { //запрос на отображение параметра
                                u16 value = ((unsigned int)(bufin[4]) << 8) + bufin[3];
                                if(bufin[2] == 0x40) { //amp
                                    amp = value; //dbg
                                } else if (bufin[2] == 0x41) {//vlots
                                    volt = value; //dbg
                                } else if(bufin[2] == 0x42) { //time
                                    time = value; //dbg
                                }

                            } break;

                            default:
                                break;
                        }
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
        EA = 0;
        if(encoder_state_adsfl != ENC_STOP) {
            Process_Encoder(encoder_state_adsfl);
            encoder_state_adsfl = ENC_STOP;
        }
        //Process_Encoder(ENC_R0);
        EA = 1;

        process_uart();
        process_touch();
    
        print_debug_info();
    }
        
      
        


   
}