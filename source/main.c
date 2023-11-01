#include "timer.h"
#include "ui.h"
#include "uart.h"
#include "dwin8283.h"
#include "canbus.h"
#include "ssd2828.h"
#include <string.h> 
#include <stdio.h>


#include "temp.h"



//заметки!!
//рабочий вариант записи по vp
//write_dgus_vp(0x1200, "fadsfsadfad\x00", sizeof("fadsfsadfad\x00"));

//Т1   время предв. продувки, с
data u16 some_rus_string[] = {0x0422, 0x0031, 0x0020, 0x0432, 0x0440, 0x0435, 0x043C, 0x044F, 0x0020, 0x043F, 0x0440, 0x0435, 0x0434, 0x0432, 0x002E, 0x0020, 0x043F, 0x0440, 0x043E, 0x0434, 0x0443, 0x0432, 0x043A, 0x0438, 0x002C, 0x0020, 0000};

              



struct text_field
{
    void *vp;
    u16 x;
    u16 y;
    u16 color;    
}text1;


u8 state, inbufind;
u8 bufin[255];
u8 len; 

u8 begfl,endfl;

#define UART_INIT 	{state = inbufind=0;  begfl=endfl=0;} 
#define UART_INIT_1 {state=1; inbufind=0; begfl=endfl=0;}
#define TEXT_VP (0x1200)
u8 temp_offset = 0;




u8  my_text[100];
u8 counter = 0;


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



u8 idx = 0;
   
     
xdata u8 dbg_string[50];
xdata u16 key_code;

const u16 KEY_CODE_VP = 0x2000;


void nothing(void)
{
                              
    
}
 
void draw_rectangle(const u16 x,const u16 y)
{
    xdata u16 draw_rect_cmd[30];
    draw_rect_cmd[0] = 0x0003;  //shape type
    draw_rect_cmd[1] = 0x0003;  //obj cnt
    draw_rect_cmd[2] = 0;       //x0
    draw_rect_cmd[3] = y;       //y0
    draw_rect_cmd[4] = 1023;    //x1
    draw_rect_cmd[5] = y + 100; //y1
    draw_rect_cmd[6] = 0xFFFF;  //color
    draw_rect_cmd[7] = 0 + 1;       //x0
    draw_rect_cmd[8] = y + 1;       //y0
    draw_rect_cmd[9] = 1023 - 1;    //x1
    draw_rect_cmd[10] = y + 100 - 1; //y1
    draw_rect_cmd[11] = 0xFFFF;  //color
    draw_rect_cmd[12] = 0 + 2;       //x0
    draw_rect_cmd[13] = y + 2;       //y0
    draw_rect_cmd[14] = 1023 - 2;    //x1
    draw_rect_cmd[15] = y + 100 - 2; //y1
    draw_rect_cmd[16] = 0xFFFF;  //color
    draw_rect_cmd[17] = 0xFF00;
    
    write_dgus_vp(0x3000, (u8*) draw_rect_cmd, sizeof(draw_rect_cmd) / 2); 
}

void draw_string(u8* s)
{
    xdata u16 str[50];
    u8 i;
    u8 len = strlen(s) + 1;
    for(i = 0; i < len; i++)
    {
        str[i] = s[i]; 
    }
    write_dgus_vp(0x1100, (u8*)str, len);
}

s16 rect_y_pos = 0;
void btn_0(void)
{
    //write_dgus_vp(0x1200, "F1\x00", sizeof("F1\x00"));
//    text1.x += 10;
//    text1.y += 10;
//    write_dgus_vp(0x9001, (u8*) &text1.x, sizeof(text1.x));
    
  rect_y_pos -= 100;
    if(rect_y_pos <= 0) 
        rect_y_pos = 700;
    draw_rectangle(0, rect_y_pos);    
}
void btn_1(void)
{
//    write_dgus_vp(0x1200, "F2\x00", sizeof("F2\x00"));
      rect_y_pos += 100;
    if(rect_y_pos == 800) 
        rect_y_pos = 0;
    draw_rectangle(0, rect_y_pos);
    draw_string("string 1");   
}

void btn_2(void)
{
    draw_string("string 2");
    write_dgus_vp(0x1200, "F3\x00", sizeof("F3\x00"));
}

void btn_3(void)
{
    static u16 n;
    u8 temp[50];
    sprintf((u8*)temp, "%s%10d","test", n);//
    n++;
    draw_string(temp);
    write_dgus_vp(0x1200, "F4\x00", sizeof("F4\x00"));    
}

void btn_4(void)
{
    draw_string("string 4");
    write_dgus_vp(0x1200, "F5\x00", sizeof("F5\x00"));
}
void btn_5(void)
{
    write_dgus_vp(0x1200, "F6\x00", sizeof("F6\x00"));
    
}


code void (*btn_func[])(void) = {
   btn_0, btn_1, btn_2,
   btn_3, btn_4, btn_5,
   nothing,nothing,nothing,nothing,nothing,nothing,nothing,nothing,nothing,nothing,    
};




print_utf_str(u16 vp, u8* str)
{
    xdata u16 out_str[30];
    u8 i;
    u8 len;
    
    len = strlen(str) + 1;
    
    for(i = 0; i < len; i++) {
        out_str[i] = str[i];
    }
    
    write_dgus_vp(vp, (u8*)out_str, sizeof(out_str));
}


code u16 string_offsets[] = {0x1000, 0x1200, 0x1400, 0x1600, 0x1800 ,0x2000, 0x2200, 0x2400};


typedef struct list_par_t{
    u8  name[30];
    u16 val;
};


   
void main()
{
	INIT_CPU();            //CPU ��ʼ����ע���ʼ�������л������xdata������ʼ����0����������ı����г�ʼֵ����Ҫ�ڸú����������¸�ֵ
	T0_Init();						 //��ʱ��0��ʼ��
	EA = 1; //interrupt on
	
    //StartTimer(0,100);
	//StartTimer(1,50);
	//-------------------
	io_init();
//	delay_ms(20);
	ssd2828_init();
//________________________________________________________________________ __________	
	UartInit(UART5, 115200);
	UART_INIT //MARCROS
	
    draw_bottom_menu();
  
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
               write_dgus_vp(0x4000, (u8*) &action, 1); //show action type
               write_dgus_vp(0x5200, (u8*) &touch_data.x, 2); //show coords of touch point
           }
    
           switch(touch_data.action_type)
           {
               case ETPS_RELEASE:
               {
                   u8 i;
                   for(i = 0; i < cur_menu->size; i++) { //if touch coords in recatnle area
                        if( ((cur_menu->active & (0x01 << i)) != 0) &&
                            touch_data.x >= cur_menu->items[i].r.x0 && touch_data.x <= cur_menu->items[i].r.x1 &&  
                            touch_data.y >= cur_menu->items[i].r.y0 && touch_data.y <= cur_menu->items[i].r.y1 ) {
                                //поиск из прямоуг. областей
                                break;
                            }
                        }
                    {
                        u16 dummy = i;
                        write_dgus_vp(0x2500, (u8*) &dummy, 1); //отобразить код выбранной функции
                    }
                    last_selected_menu_item = i;
                    cur_menu->func(i); 
                   
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
    //working cmd 2407 0003 0000 02bc 0001  0064 02bc 0001   00c8 02bc 0003 FF00
    // {         
    //         struct {
    //           u16 x;
    //           u16 y;  
    //         } rect[] = {
    //           {0, 700},
    //           {100, 700},
    //           {200, 700},  
    //           {300, 700},  
    //           {400, 700},  
    //           {500, 700},  
    //           {600, 700},  
    //           {700, 700},  
    //           {800, 700},  
    //           {900, 700},  
               
    //         };
            
          
    //         while(1)
    //         {
    //             u8 i;
    //             struct {
    //                 u8 status; //0x5A if some action done
    //                 u8 action_type;
    //                 u16 x;
    //                 u16 y;
    //             } touch_data;
                
                
    //             read_dgus_vp(0x16,(u8*)&touch_data, sizeof(touch_data) / 2); //info about touch status and coords
    //             if(touch_data.status == 0x5A) {                                     
                    
    //                 //process touch action
    //                 for(i = 0; i < sizeof(rect) / sizeof(rect[0]); i++) { //if touch coords in recatnle area
    //                 if(touch_data.x >= rect[i].x && touch_data.x <= rect[i].x + 100 &&
    //                    touch_data.y >= rect[i].y && touch_data.y <= rect[i].y + 100) {
                                                                                                  
    //                        write_dgus_vp(0x5200, (u8*) &touch_data.x, 2); //show coords of touch point
    //                        if(i == 0) {// если выбран 0 элемент
    //                          static u8 cnt = 0;
    //                          if((cnt & 0x01) == 1) {
    //                             draw_vertical_menu();
    //                          } else {
    //                             u16 dummpy = 0x0000;
    //                             write_dgus_vp(0x9500, (u8*) &dummpy, 1);
    //                          }
    //                          cnt++; //for switch                             
    //                        }
                        
    //                        break;
    //                    }                           
    //                 }
                    
                    
    //                 //wait button up
                    
    //                 while(touch_data.action_type != 0x02) //manual page 52 tp controll
    //                 {
    //                     read_dgus_vp(0x16,(u8*)&touch_data, 1); //info about touch status and coords
    //                 }
    //                 touch_data.status = 0x00;
    //                 write_dgus_vp(0x16, (u8*) &touch_data.x, 1); //display coords of touch point
                    
    //             }        
                
    //         }        
    // }    
   
    

//     {
//         u8 i;
//         u8 str[60];
//         struct list_par_t list_par[] = {
//            {"par 1   ",  100},
//            {"par 2sdf",  200},
//            {"par 3   ",  300},
//            {"par 4   ",  400},
//            {"par 5a  ",  500},
//            {"par 6b  ",  600},
//            {"par 7c  ",  700},
//            {"par 8   ",  800},
//            {"par 9   ",  900},
//            {"par 10  ", 1000},
//        }; 
//            
//        
//                 
//          for(i = 0; i < sizeof(list_par) / sizeof(list_par[0]) ; i++){
//              //sprintf(str, "%-30s %05d", list_par[i].name, list_par[i].val);   
//              sprintf(str, "%s%d", list_par[i].name, list_par[i].val);   
//              print_utf_str(string_offsets[i], str); 
//          }
//         
//          
//      }          
      
        
  
     
    
     
    
    // dbg_string[0] = 0xff;
    // dbg_string[1] = 0xff;
    // dbg_string[2] = 0xff;
    // dbg_string[3] = 0xff;
    // dbg_string[4] = 0xff;
    // dbg_string[5] = 0xff;
    // while(1)
    // {
    //     //Uatr_Send_Data(UART5, sizeof(dbg_string), dbg_string);  
    //     delay_ms(5000);
    // }
    
   
    
    
    
    key_code = 0x0010; //16 кнопка не нажата
    write_dgus_vp(0x5000, (u8*) &key_code, sizeof(key_code));
    while(1)
    {//обработчик кнопок
        read_dgus_vp(0x5000, (u8*)&key_code, sizeof(key_code));//чтение кода кноки
        if(key_code < 0x0010)
        {
			(*btn_func[key_code])();
            key_code = 0x0010;            
            write_dgus_vp(0x5000, (u8*) &key_code, sizeof(key_code));
        }
        
        
        
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