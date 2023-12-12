#include "temp.h"
#include "sys.h"
#include "string.h"
#include "stdio.h"
#include "drawing.h"
#include "encoder.h"

code const u8  LINE_HIGHT   = 13;//px
idata      u16 LINE_WIDTH   = 150;//px
idata      u16 FONT_SIZE    = 24;
idata      u8  LEVEL_HEIGHT = 90;//px
code const u8  TOUCH_HEIGHT = 100;//px
idata      u16 TIME_Y_LEVEL = 800 - 150;
code const u16 ICON_RECT_SZ = 128;



code const u32 menu_item_bm[] = { 
    ((u32)1 << EIID_TIG)       | ((u32)1 << EIID_MMA),      
    ((u32)1 << EIID_2T)        | ((u32)1 << EIID_4T)       | ((u32)1 << EIID_4T_PLUS)  | ((u32)1 << EIID_TIG_SPOT),    
    ((u32)1 << EIID_AC)        | ((u32)1 << EIID_AC_MIX)   | ((u32)1 << EIID_DC_MINUS) | ((u32)1 << EIID_DC_PLUS),                                             
    ((u32)1 << EIID_HF)        | ((u32)1 << EIID_LIFT),
    ((u32)1 << EIID_PULSE_OFF) | ((u32)1 << EIID_PULSE_ON),
    ((u32)1 << EIID_SIN)       | ((u32)1 << EIID_TRIG)     | ((u32)1 << EIID_RECT)     | ((u32)1 << EIID_TRAP),                                                
    ((u32)1 << EIID_D_10)      | ((u32)1 << EIID_D_16)     | ((u32)1 << EIID_D_20)     | ((u32)1 << EIID_D_24) | ((u32)1 << EIID_D_32) | ((u32)1 << EIID_D_40),
    ((u32)1 << EIID_H2O_OFF)   | ((u32)1 << EIID_H2O_ON),   
}; 

code const u32 active_items[] = {
    ((u32)1 << EIID_TIG)       | ((u32)1 << EIID_MMA)      |
    ((u32)1 << EIID_2T)        | ((u32)1 << EIID_4T)       | ((u32)1 << EIID_4T_PLUS)  | ((u32)1 << EIID_TIG_SPOT) |
    ((u32)1 << EIID_AC)        | ((u32)1 << EIID_AC_MIX)   | ((u32)1 << EIID_DC_MINUS) |
    ((u32)1 << EIID_HF)        | ((u32)1 << EIID_LIFT)     |
    ((u32)1 << EIID_PULSE_OFF) | ((u32)1 << EIID_PULSE_ON) |
    ((u32)1 << EIID_SIN)       | ((u32)1 << EIID_TRIG)     | ((u32)1 << EIID_RECT)     | ((u32)1 << EIID_TRAP) |
    ((u32)1 << EIID_D_10)      | ((u32)1 << EIID_D_16)     | ((u32)1 << EIID_D_20)     | ((u32)1 << EIID_D_24) | ((u32)1 << EIID_D_32) | ((u32)1 << EIID_D_40) |
    ((u32)1 << EIID_H2O_OFF)   | ((u32)1 << EIID_H2O_ON),    //tig;

    ((u32)1 << EIID_TIG)       | ((u32)1 << EIID_MMA)      |
    ((u32)1 << EIID_AC)        | ((u32)1 << EIID_DC_MINUS) | ((u32)1 << EIID_DC_PLUS) |
    ((u32)1 << EIID_PULSE_OFF) | ((u32)1 << EIID_PULSE_ON),   //mma
};


//те элементы которые будут отображены в горизонтальном меню и то какая сейчас настройка
data u32 main_menu_bm;
     u32 vertical_menu_bm = 0;
//битовая маска горизонтального меню на нижнем экране необходима первоначальная инициализация

icon_t cur_menu[48];
u32 cur_menu_active;
u8 touch_rect_cnt;

u8 idata cur_active_items_id = TIG;


u16 text_sp[EIID_SIZE]; //массив sp идентификаторов линий, картинок, чисел
s16 par[EIID_SIZE];
xdata s16 par_tek[36];//параметры для силовой части
u16 cur_par_id; //номер текущего параметра к отображению
u16 prev_par_id;
typedef struct {
    s16 min;
    s16 max;
}lim_t;
lim_t par_lim[EIID_SIZE];
s16   par_step[EIID_SIZE];

u16 Amp_text_sp; //поле для вывода тока во время сварки
u16 Volt_text_sp;//поле для вывода напраяжения во время сварки
u16 Par_big_text_sp;//поле для вывода значения текущего настраиваемого параметра
data u8 Welding_state;//текущее состояние силовой части
u16 Filled_rect_under_par_sp; //прямоугольник под параметрами

u16 icon_sp_bottom_menu[16]; //sp картинок нижнего горизонтального меню // для их изменения

void (*cur_menu_fanc)(u8 item_pos);
void cur_par_value_change(u16 cur_par_id, s8 shift);
void display_par(u16 par_id);
void par_select(u8 pair_id);
void bottom_level_controls(u8 item_pos);


void make_lim(lim_t *lim, s16 min, s16 max) {
    lim->min = min;
    lim->max = max;
}

void add_touch_place(u16 x0, u16 y0, u16 x1, u16 y1, u8 touch_id)
{
    if(touch_rect_cnt > ARR_SIZE(cur_menu)) return;

    cur_menu[touch_rect_cnt].id = touch_id;
    cur_menu[touch_rect_cnt].r.x0 = x0;
    cur_menu[touch_rect_cnt].r.y0 = y0;
    cur_menu[touch_rect_cnt].r.x1 = x1;
    cur_menu[touch_rect_cnt].r.y1 = y1; // размер кнопки
    touch_rect_cnt++;        
}


void draw_number_wtih_touch_centered(u16 x, u16 y, u16 font_size, u8 touch_id)
{
    text_sp[touch_id] = Draw_text(x, 
                                  y - (font_size * 2), 
                                  x + LINE_WIDTH, 
                                  y, 
                                  0,0,
                                  font_size,
                                  font_size * 2, 
                                  TEXT_INTERVAL_1 | TEXT_ALIGNMENT_CENTER | TEXT_ALIGNMENT_VERTICAL_UP | TEXT_ENC_BIG5,
                                  0x0000);
    display_par(touch_id);
    add_touch_place(x, y - (font_size * 2), x + LINE_WIDTH, y, touch_id);
}



void dummy_line_up(point_t *p)
{
    Draw_line(p->x, p->y, p->x, (p->y -= LEVEL_HEIGHT), LINE_HIGHT, GREEN);
}

void dummy_line_down(point_t *p)
{
    Draw_line(p->x, p->y, p->x, (p->y += LEVEL_HEIGHT), LINE_HIGHT, GREEN);
}

void add_t_preflow(point_t *p) //возвращает конечную точку после отрисовки данного участка
{
    Draw_line(p->x, p->y, p->x + LINE_WIDTH, p->y, LINE_HIGHT, GREEN);
    draw_number_wtih_touch_centered(p->x, TIME_Y_LEVEL, 36, EIID_PRE_FLOW_T1); //время продувки
    Draw_line(p->x, 650 - 80, p->x, 650, 5, 0xDEDB); //вертикальная черта
    p->x += LINE_WIDTH;
}

void add_t_start_i(point_t *p) 
{
    Draw_line(p->x, p->y, p->x + LINE_WIDTH, p->y,  LINE_HIGHT, GREEN);
    draw_number_wtih_touch_centered(p->x, p->y, FONT_SIZE, EIID_START_I1);//начальный ток
    draw_number_wtih_touch_centered(p->x, TIME_Y_LEVEL, 36, EIID_START_T2);//время начального тока
    Draw_line(p->x, 650 - 80, p->x, 650, 5, 0xDEDB); //вертикальная черта
    p->x += LINE_WIDTH;
}

void add_t_up(point_t *p) 
{
    Draw_line(p->x, p->y - 1, p->x + LINE_WIDTH, p->y - LEVEL_HEIGHT,  LINE_HIGHT, GREEN);
    draw_number_wtih_touch_centered(p->x, TIME_Y_LEVEL, 36, EIID_UP_T3); //время наростания
    Draw_line(p->x, 650 - 80, p->x, 650, 5, 0xDEDB); //вертикальная черта
    p->x += LINE_WIDTH;
    p->y -= LEVEL_HEIGHT;
}

void add_base_i1_i2_t(point_t *p) 
{
    Draw_line(p->x, p->y, p->x + LINE_WIDTH, p->y,  LINE_HIGHT, GREEN);
    draw_number_wtih_touch_centered(p->x, p->y, FONT_SIZE, EIID_BASE_I2); //ток базы
    if(main_menu_bm & (u32)1 << EIID_PULSE_ON) {
        draw_number_wtih_touch_centered(p->x, TIME_Y_LEVEL, 30, EIID_BASE_T4); //время базы
    }
    if(main_menu_bm & (u32)1 << EIID_4T_PLUS) 
        draw_number_wtih_touch_centered(p->x, p->y + (FONT_SIZE * 2) + (LINE_HIGHT / 2) , FONT_SIZE, EIID_BASE2_I2X); //второй ток базы
    Draw_line(p->x, 650 - 80, p->x, 650, 5, 0xDEDB); //вертикальная черта
    p->x += LINE_WIDTH;
}

void add_i_t_impulse(point_t *p) 
{
    Draw_line(p->x, p->y, p->x + LINE_WIDTH, p->y,  LINE_HIGHT, GREEN);
    draw_number_wtih_touch_centered(p->x, TIME_Y_LEVEL, 30, EIID_IMPULSE_T5);// время импульса
    draw_number_wtih_touch_centered(p->x, p->y, FONT_SIZE, EIID_IMPULSE_I3);  //ток импульса
    Draw_line(p->x, 650 - 80, p->x, 650, 5, 0xDEDB); //вертикальная черта
    p->x += LINE_WIDTH;
}

void add_t_down(point_t *p)
{
    Draw_line(p->x + 1, p->y, p->x + LINE_WIDTH, p->y + LEVEL_HEIGHT,  LINE_HIGHT, GREEN);
    draw_number_wtih_touch_centered(p->x, TIME_Y_LEVEL, 36, EIID_DOWN_T6); //время спада
    Draw_line(p->x, 650 - 80, p->x, 650, 5, 0xDEDB); //вертикальная черта
    p->x += LINE_WIDTH;
    p->y += LEVEL_HEIGHT;
}

void add_end_i_t(point_t *p)
{
    Draw_line(p->x, p->y, p->x + LINE_WIDTH, p->y,  LINE_HIGHT, GREEN);
    draw_number_wtih_touch_centered(p->x, TIME_Y_LEVEL, 36, EIID_END_T7); //время конечного тока
    draw_number_wtih_touch_centered(p->x, p->y, FONT_SIZE, EIID_END_I4);  //конечный ток
    Draw_line(p->x, 650 - 80, p->x, 650, 5, 0xDEDB); //вертикальная черта
    p->x += LINE_WIDTH;
}

void add_t_postflow(point_t *p)
{
    Draw_line(p->x, p->y, p->x + LINE_WIDTH, p->y,  LINE_HIGHT, GREEN);
    draw_number_wtih_touch_centered(p->x, TIME_Y_LEVEL, 36, EIID_POST_FLOW_T8);//время конечной продувки
    Draw_line(p->x, 650 - 80, p->x, 650, 5, 0xDEDB); //вертикальная черта
    p->x += LINE_WIDTH;
}

void add_i_kz(point_t *p)
{
    Draw_line(p->x, p->y, p->x + LINE_WIDTH, p->y - LEVEL_HEIGHT, LINE_HIGHT, GREEN);
    draw_number_wtih_touch_centered(p->x, p->y - LEVEL_HEIGHT, FONT_SIZE, EIDD_KZ_I5);//время конечной продувки
    Draw_line(p->x, 650 - 80, p->x, 650, 5, 0xDEDB); //вертикальная черта
    p->x += LINE_WIDTH;
    p->y -= LEVEL_HEIGHT;

}

void display_par(u16 par_id) 
{
    u16 par_color;
    u8 code *format[] = {
        "", 
        "%.1fs", "%0.0fs", "%0.0fms", "%0.0fA", "%.0f",        
    };
    u8 code *format_big[] = {
        "", 
        "%.1fs\r\n", "%.0fs\r\n", "%.0fms\r\n", "%.0fA\r\n", "%.0f\r\n",    
    };
    u8 format_id = 0;
    float par_value = par[par_id];

    switch (par_id)
    {
        case EIID_PRE_FLOW_T1:
        case EIID_START_T2:     
        case EIID_UP_T3:           
        case EIID_DOWN_T6:      
        case EIID_END_T7: {
            u16 tmp = par[par_id];
            if(tmp < 100) {
                format_id = 1;
                par_value /= 10;           
            } else {
                par_value = 10;
                format_id = 2;             
            }
            par_color = CYAN; 
        } break;       

        case EIID_BASE_T4:      
        case EIID_IMPULSE_T5: {
            format_id = 3;
            par_color = CYAN; 
        } break;

        case EIID_POST_FLOW_T8: {
            format_id = 2;
            par_color = CYAN; 
        } break;
           
        case EIID_START_I1:     
        case EIID_BASE_I2:      
        case EIID_BASE2_I2X:
        case EIID_IMPULSE_I3:   
        case EIID_END_I4:       
        case EIDD_KZ_I5: {
            format_id = 4;
            par_color = YELLOW; 
        } break;       

        case EIID_FREQ_F1:      
        case EIID_BALANCE_D1: {
            format_id = 5;
        } break; 

        default: return;  
    }

    if(format_id) {
        Draw_text_set_text(format[format_id], text_sp[par_id], par_value);
        if(par_id == cur_par_id)
            par_color = PINK;        
        
        Draw_text_set_color(text_sp[par_id], par_color);//заменить вернуть основоной цвет
        //если сварка не отображать параметр большими цифрами
        if(Welding_state == 0) { //Idle state
            Draw_text_set_text(format_big[format_id], Par_big_text_sp, par_value);
        }
    }
}

void cur_par_value_change(u16 par_id, s8 shift)
{
    if(shift != 0) {
        par[par_id] += ((s16)par_step[par_id] * shift); 
        if(par[par_id] > par_lim[par_id].max)       
            par[par_id] = par_lim[par_id].max;
        else if(par[par_id] < par_lim[par_id].min) 
            par[par_id] = par_lim[par_id].min;
    } 
   
    switch (par_id)
    {
        case EIID_PRE_FLOW_T1  :
        case EIID_START_T2     :
        case EIID_UP_T3        :
        case EIID_DOWN_T6      :
        case EIID_END_T7       :  
        case EIID_POST_FLOW_T8 : 
        case EIID_START_I1     :     
        case EIID_BASE_I2      :
        case EIID_BASE2_I2X    :  
        case EIID_IMPULSE_I3   : 
        case EIID_END_I4       :
        case EIDD_KZ_I5        :
        case EIID_FREQ_F1      :
        case EIID_BALANCE_D1   :
        case EIID_BASE_T4      : 
        case EIID_IMPULSE_T5   : {
            u16 tmp = par[par_id];
            switch (par_id)
            {
                case EIID_PRE_FLOW_T1  : par_tek[EPID_PRE_FLOW_T1]  = tmp; break;    
                case EIID_START_T2     : par_tek[EPID_START_T2]     = tmp; break;    
                case EIID_UP_T3        : par_tek[EPID_UP_T3]        = tmp; break;    
                case EIID_DOWN_T6      : par_tek[EPID_DOWN_T6]      = tmp; break;    
                case EIID_END_T7       : par_tek[EPID_END_T7]       = tmp; break;
                case EIID_POST_FLOW_T8 : par_tek[EPID_POST_FLOW_T8] = tmp; break;
                case EIID_START_I1     : par_tek[EPID_START_I1]     = tmp; break;    
                case EIID_BASE_I2      : par_tek[EPID_BASE_I2]      = tmp; break;
                case EIID_BASE2_I2X    : par_tek[EPID_BASE2_I2X]    = tmp; break; 
                case EIID_IMPULSE_I3   : par_tek[EPID_IMPULSE_I3]   = tmp; break;
                case EIID_END_I4       : par_tek[EPID_END_I4]       = tmp; break;
                case EIDD_KZ_I5        : par_tek[EPID_KZ_I5]        = tmp; break;
                case EIID_FREQ_F1      : par_tek[EPID_FREQ_F1]      = tmp; break;
                case EIID_BALANCE_D1   : par_tek[EPID_BALANCE_D1]   = tmp; break;
                case EIID_BASE_T4      : par_tek[EPID_BASE_T4]      = tmp; break;
                case EIID_IMPULSE_T5   : par_tek[EPID_IMPULSE_T5]   = tmp; break;    
            }  
            display_par(par_id);
        } break;

        case EIID_PULSE_OFF:
        case EIID_PULSE_ON :
        case EIID_AC       :
        case EIID_AC_MIX   :
        case EIID_DC_MINUS :
        case EIID_DC_PLUS  : 
        case EIID_TIG      :       
        case EIID_MMA      : 
        case EIID_TIG_SPOT : {
            struct mod_submod{
                u16 d0    : 4; //пусто, не используется.   
                u16 pulse : 4; //0-пульс 1-не пульс
                u16 pol   : 4; //0-DC  1-AC  2-MIX
                u16 mod   : 4; //0-TIG 1-MMA 2-TIG Spot
            } tmp;
            *(u16*)&tmp = par_tek[EPID_MODES_SUBMODES]; 
            switch (par_id)
            {
                case EIID_PULSE_ON : tmp.pulse = 0; break;
                case EIID_PULSE_OFF: tmp.pulse = 1; break; 

                case EIID_DC_MINUS : tmp.pol   = 0; par_tek[EPID_POLARITY] = 1; par_tek[EPID_SERVICE_PAR] &= ~0x0002; break;
                case EIID_DC_PLUS  : tmp.pol   = 0; par_tek[EPID_POLARITY] = 2; par_tek[EPID_SERVICE_PAR] |=  0x0002; break;
                case EIID_AC       : tmp.pol   = 1; break;
                case EIID_AC_MIX   : tmp.pol   = 2; break;

                case EIID_TIG      : tmp.mod   = 0; break;
                case EIID_MMA      : tmp.mod   = 1; break;
                case EIID_TIG_SPOT : tmp.mod   = 2; break;
            }
            par_tek[EPID_MODES_SUBMODES] = *(u16*)&tmp;
        } break;
    
        case EIID_2T     : par_tek[EPID_2T_4T] = 1; par_tek[EPID_IS_PLUS_4T] = 1; par_tek[EPID_SERVICE_PAR] &= ~0x0004; break;
        case EIID_4T     : par_tek[EPID_2T_4T] = 2; par_tek[EPID_IS_PLUS_4T] = 1; par_tek[EPID_SERVICE_PAR] |=  0x0004; par_tek[EPID_SERVICE_PAR] &= ~0x0008; break;
        case EIID_4T_PLUS: par_tek[EPID_2T_4T] = 2; par_tek[EPID_IS_PLUS_4T] = 2; par_tek[EPID_SERVICE_PAR] |=  0x0008; break;
      

        case EIID_LIFT: par_tek[EPID_IGNITION_TYPE]  = 1; par_tek[EPID_SERVICE_PAR] &= ~0x0001; break;
        case EIID_HF  : par_tek[EPID_IGNITION_TYPE]  = 2; par_tek[EPID_SERVICE_PAR] |=  0x0001; break;
        
        case EIID_H2O_OFF: par_tek[EPID_FLOW_SENSOR] = 1; par_tek[EPID_SERVICE_PAR] &= ~0x0010; break; 
        case EIID_H2O_ON : par_tek[EPID_FLOW_SENSOR] = 2; par_tek[EPID_SERVICE_PAR] |=  0x0010; break;

        case EIID_D_10: par_tek[EPID_ELECTRODE_D]    = 1; break;    
        case EIID_D_16: par_tek[EPID_ELECTRODE_D]    = 2; break;
        case EIID_D_20: par_tek[EPID_ELECTRODE_D]    = 3; break;    
        case EIID_D_24: par_tek[EPID_ELECTRODE_D]    = 4; break;
        case EIID_D_32: par_tek[EPID_ELECTRODE_D]    = 5; break;
        case EIID_D_40: par_tek[EPID_ELECTRODE_D]    = 6; break;
   
        case EIID_SIN : par_tek[EPID_WAVE_FORM]      = 1; break;
        case EIID_RECT: par_tek[EPID_WAVE_FORM]      = 2; break;  
        case EIID_TRIG: par_tek[EPID_WAVE_FORM]      = 3; break;
        case EIID_TRAP: par_tek[EPID_WAVE_FORM]      = 4; break;
        default : return;
    }
}




void Encoder_process_code(u8 state) 
{
    code const s8 shifts[] = {-1, -5, +1, +5};
    cur_par_value_change(cur_par_id, shifts[state]);     
}




void par_select(u8 par_id) 
{
    u8 old_par_id;
    {//перерисовать прямоугольник под параметрами
        rect_t r;
        Draw_text_get_pos(text_sp[par_id], &r);
        Draw_filled_rect_redraw(Filled_rect_under_par_sp, r.x0, 220, r.x1, SCREEN_HEIGHT - ICON_RECT_SZ, 0x528A);
    }
    display_par(par_id); 
    Draw_text_set_color(text_sp[par_id], PINK);//заменить на цвет "выбранного элемента"
    if(cur_par_id == par_id) return;
    display_par(cur_par_id);
    old_par_id = cur_par_id;
    cur_par_id = par_id;
    display_par(old_par_id);
    display_par(cur_par_id);
}







void init_par_udgu(void)
{
    make_lim(&par_lim[EIID_PRE_FLOW_T1],  1,   100  ); //Т1  время предв. продувки, с 
    make_lim(&par_lim[EIID_START_I1],     5,   350  );
    make_lim(&par_lim[EIID_START_T2],     1,   100  );
    make_lim(&par_lim[EIID_UP_T3],        1,   100  );
    make_lim(&par_lim[EIID_BASE_I2],      5,   350  );
    make_lim(&par_lim[EIID_BASE_T4],      1,   10000);
    make_lim(&par_lim[EIID_IMPULSE_I3],   5,   350  );
    make_lim(&par_lim[EIID_IMPULSE_T5],   1,   10000);
    make_lim(&par_lim[EIID_FREQ_F1],      20,  200  );
    make_lim(&par_lim[EIID_BALANCE_D1],   30,  80   );
    make_lim(&par_lim[EIID_DOWN_T6],      1,   100  );
    make_lim(&par_lim[EIID_END_I4],       5,   350  );
    make_lim(&par_lim[EIID_END_T7],       1,   100  );
    make_lim(&par_lim[EIID_POST_FLOW_T8], 1,   60   );
    make_lim(&par_lim[EIDD_KZ_I5],        0,   100  );
    make_lim(&par_lim[EIID_BASE2_I2X],    5,   350  );

    par_step[EIID_PRE_FLOW_T1]  =  1  ; //ms //100ms
    par_step[EIID_START_I1]     =  1  ; //A  
    par_step[EIID_START_T2]     =  1  ; //ms //100ms  
    par_step[EIID_UP_T3]        =  1  ; //ms //100ms     
    par_step[EIID_BASE_I2]      =  1  ; //A    
    par_step[EIID_BASE_T4]      =  1  ; //ms   
    par_step[EIID_IMPULSE_I3]   =  1  ; //A
    par_step[EIID_IMPULSE_T5]   =  1  ; //ms 
    par_step[EIID_FREQ_F1]      =  1  ;   
    par_step[EIID_BALANCE_D1]   =  1  ;
    par_step[EIID_DOWN_T6]      =  1  ; //ms //100ms   
    par_step[EIID_END_I4]       =  1  ; //A     
    par_step[EIID_END_T7]       =  1  ; //ms //100ms    
    par_step[EIID_POST_FLOW_T8] =  1  ; //
    par_step[EIDD_KZ_I5]        =  1  ; //%     
    par_step[EIID_BASE2_I2X]    =  1  ; //A 



    {
        u8 i;
        s16 *p_tmp;

        p_tmp = par; 
        for (i = 0; i < ARR_SIZE(par); i++)
            *p_tmp++ = 0;
        
        p_tmp = par_tek;
        for (i = 0; i < ARR_SIZE(par_tek); i++)
            *p_tmp++ = 0;        
    }
 
   
  main_menu_bm = (((u32)1 << EIID_TIG)              |
                         ((u32)1 << EIID_4T)        |
                         ((u32)1 << EIID_AC)        |
                         ((u32)1 << EIID_HF)        |
                         ((u32)1 << EIID_PULSE_OFF) |
                         ((u32)1 << EIID_SIN)       |
                         ((u32)1 << EIID_D_32)      |
                         ((u32)1 << EIID_H2O_ON)      
                    );
    cur_par_value_change(EIID_TIG,        0);
    cur_par_value_change(EIID_4T,         0);
    cur_par_value_change(EIID_AC,         0);
    cur_par_value_change(EIID_HF,         0);
    cur_par_value_change(EIID_PULSE_OFF,  0);
    cur_par_value_change(EIID_D_32,       0);
    cur_par_value_change(EIID_SIN,        0);
    cur_par_value_change(EIID_H2O_ON,     0);

        
    par[EIID_PRE_FLOW_T1]        = par_tek[EPID_PRE_FLOW_T1]    = 1;   // t gaz
    par[EIID_START_I1]           = par_tek[EPID_START_I1]       = 50;   // I begin
    par[EIID_START_T2]           = par_tek[EPID_START_T2]       = 1;   // t begin
    par[EIID_UP_T3]              = par_tek[EPID_UP_T3]          = 1;   // increase
    par[EIID_BASE_I2]            = par_tek[EPID_BASE_I2]        = 101;  // I базы
    par[EIID_BASE_T4]            = par_tek[EPID_BASE_T4]        = 2;    // t базы
    par[EIID_IMPULSE_I3]         = par_tek[EPID_IMPULSE_I3]     = 150;  // I имп
    par[EIID_IMPULSE_T5]         = par_tek[EPID_IMPULSE_T5]     = 3;    // t имп
    par[EIID_FREQ_F1]            = par_tek[EPID_FREQ_F1]        = 50;  // F
    par[EIID_BALANCE_D1]         = par_tek[EPID_BALANCE_D1]     = 60;   // balans
    par[EIID_DOWN_T6]            = par_tek[EPID_DOWN_T6]        = 1;   // Decrease
    par[EIID_END_I4]             = par_tek[EPID_END_I4]         = 60;   // I end
    par[EIID_END_T7]             = par_tek[EPID_END_T7]         = 1;    // t end
    par[EIID_POST_FLOW_T8]       = par_tek[EPID_POST_FLOW_T8]   = 1;    // t gaz
    par[EIDD_KZ_I5]              = par_tek[EPID_KZ_I5]          = 55;   // I kz
                                   par_tek[EPID_BSN]            = 1;    // BSN 1-off 2-on
  

    par_tek[EPID_MODES_SUBMODES] = 0x0110;
}



void bottom_level_controls(u8 item_pos)
{ //вызовется после выбора элемента в горизонтальном меню
    if(item_pos == touch_rect_cnt) return;
    switch (cur_menu[item_pos].id) {
        case EIID_TIG: case EIID_MMA:  case EIID_AC:        case EIID_AC_MIX:   case EIID_DC_MINUS: case EIID_DC_PLUS: case EIID_2T:   case EIID_4T:   case EIID_4T_PLUS: case EIID_TIG_SPOT:
        case EIID_HF:  case EIID_LIFT: case EIID_PULSE_OFF: case EIID_PULSE_ON: case EIID_D_10:     case EIID_D_16:    case EIID_D_20: case EIID_D_24: case EIID_D_32:    case EIID_D_40:
        case EIID_SIN: case EIID_TRIG: case EIID_RECT:      case EIID_TRAP:     case EIID_H2O_ON:   case EIID_H2O_OFF: {
            //изменение параметра для индикации, горизонтальнео меню
            u8 i;
            u8 id;
            id = cur_menu[item_pos].id;
            
            if(id == EIID_TIG)
                cur_active_items_id = TIG;
            else if(id == EIID_MMA) {
                cur_active_items_id = MMA;
                main_menu_bm &= ~(menu_item_bm[MEN_POL_MOD]);
                main_menu_bm |= (u32)1 << EIID_DC_MINUS;
                cur_par_value_change(EIID_DC_MINUS, 0); //принудительный переход в режим dc в mma .. Пока будет так, потом убрать
            }

            if(main_menu_bm & ((u32)1 << id)) {//если выбор был сделан из горизонтального меню , сделать битовое поле для вертикального меню
                for(i = 0; i < ARR_SIZE(menu_item_bm); i++) {
                    if((u32)menu_item_bm[i] & (u32)((u32)1 << id)) { //поиск меню к которму относится данный элемент
                        vertical_menu_bm = active_items[cur_active_items_id] & menu_item_bm[i];//убрать из отображения эелементы которые не доступны в данном режиме 
                        break;
                    }
                }
            } else { //выбор был из вертикального меню
                for(i = 0; i < ARR_SIZE(menu_item_bm); i++) {
                    if( menu_item_bm[i] & (u32)1 << id) {
                        main_menu_bm &= (u32)~menu_item_bm[i]; //очисть область связанную с этим подменю в гавном меню
                        main_menu_bm |= ((u32)1 << id); //вставить текущий выбранный элемент в главное меню
                        break;
                    }
                }
            }
            cur_par_value_change(cur_menu[item_pos].id, 0); //изменение параметра для силовой части
            make_scene();
        } break;

        case EIID_PRE_FLOW_T1:  case EIID_START_I1:   case EIID_START_T2: case  EIID_UP_T3:     case EIID_BASE_I2: case EIID_BASE_T4:
        case EIID_IMPULSE_I3:   case EIID_IMPULSE_T5: case EIID_FREQ_F1:  case EIID_BALANCE_D1: case EIID_DOWN_T6: case EIID_END_I4: case EIID_END_T7:       
        case EIID_POST_FLOW_T8: case EIDD_KZ_I5:      case EIID_BASE2_I2X: {
            par_select(cur_menu[item_pos].id);
        } break;

        default: return;
    }    
}




void make_scene(void)
{
    Draw_start();
    touch_rect_cnt = 0; //убрать все области касания

    Draw_filled_rect(0, 0, 1279, 799, GRAY);//прямоугольник под задний фон 

    Filled_rect_under_par_sp = Draw_filled_rect(0, 0, 0, 0, 0x528A); // рамка под параметрами создание области под нее заготовка   
    
    Draw_filled_rect(0, 210, 1279, 210 + 10, BLACK);//полоска под напряжением и током

    {
        u16 start_x;
        u16 start_y;
        u32 bm;
        u8 i;
        u8 id; 

        bm = main_menu_bm & active_items[cur_active_items_id];

        if(main_menu_bm & ((u32)1 << EIID_DC_MINUS) || main_menu_bm & ((u32)1 << EIID_DC_PLUS)) { //если dc+ или dc- убрать выбор формы волны
            bm &= ~(menu_item_bm[MEN_PLS_WAVE_SHAPE_MOD]);
        }

        if(main_menu_bm & ((u32)1 << EIID_TIG_SPOT)) { //в режиме tig spot нет выбора pulse
            bm &= ~(menu_item_bm[MEN_PLS_MOD]);  
        }

        start_x = 0;
        start_y = SCREEN_HEIGHT - ICON_RECT_SZ;
        i = 0;
        id = 0;
        while (bm != 0) {//создание , отрисовка горизонтального меню
            if((u32)bm & (u32)1) { 
                if(vertical_menu_bm & ((u32)1 << id)) { //если при отрисовке горизонтального меню в в вертикальном меню есть подбный элемент отрисовать над этим элементом вертикально меню
                    u8 v_id = 0;
                    u16 v_start_x = ICON_RECT_SZ * i;
                    u16 v_start_y = SCREEN_HEIGHT - (ICON_RECT_SZ * 2);
                    vertical_menu_bm &= ~((u32)1 << id);//убрать из вертикального меню текущий элемен горизонтального меню
                    while (vertical_menu_bm != 0) {//создание , отрисовка вертикального меню
                        if((u32)vertical_menu_bm & (u32)1) {
                            add_touch_place(v_start_x, v_start_y, v_start_x + ICON_RECT_SZ, v_start_y + ICON_RECT_SZ, v_id);
                            Draw_image(v_start_x, v_start_y, v_id);
                            v_start_y -= ICON_RECT_SZ; 
                        }
                        vertical_menu_bm >>= 1; //после отрисовки вертикальное меню будет пустым и его не надо принудительно обнулять
                        v_id++;      
                    }
                }
                
                add_touch_place(start_x, start_y, start_x + ICON_RECT_SZ, start_y + ICON_RECT_SZ, id);
                Draw_image(start_x, start_y, id);
                start_x += ICON_RECT_SZ;
                i++;
            }
            bm >>= 1;
            id++;
        }
        cur_menu_fanc = bottom_level_controls;//функция которая вызывается при касании по области touch
    }

    //отрисовка циклограммы
    {
        point_t p;
        point_t start_p;
        if(main_menu_bm & (u32)1 << EIID_TIG) {
            FONT_SIZE  = 35; 
            if(main_menu_bm & (u32)1 << EIID_TIG_SPOT) {
                LINE_WIDTH = 200;
            } else {
                if(main_menu_bm & (u32)1 << EIID_PULSE_ON)
                    LINE_WIDTH = 150;
                else
                    LINE_WIDTH = 171;
            }
            start_p = p = make_point(40, 550);
            add_t_preflow(&p);
            dummy_line_up(&p);
            add_t_start_i(&p);
            add_t_up(&p);
            add_base_i1_i2_t(&p);
        if(main_menu_bm & (u32)1 << EIID_TIG_SPOT) {
            LEVEL_HEIGHT <<= 1; // * 2
            add_t_down(&p);
            LEVEL_HEIGHT >>= 1; // div 2
            add_t_postflow(&p);
        } else {
            if(main_menu_bm & (u32)1 << EIID_PULSE_ON) {
                dummy_line_down(&p);
                add_i_t_impulse(&p);
                dummy_line_up(&p);
            } 
            add_t_down(&p);
            add_end_i_t(&p);
            dummy_line_down(&p);
            add_t_postflow(&p);
        }
        } else if(main_menu_bm & (u32)1 << EIID_MMA) {//mma
            if(main_menu_bm & (u32)1 << EIID_PULSE_ON) {
                LINE_WIDTH = 300;
                FONT_SIZE = 38;//48
            } else {
                LINE_WIDTH = 400;
                FONT_SIZE = 38;//64
            }
            start_p = p = make_point(40, 500);
            dummy_line_up(&p);
            add_t_start_i(&p);
            dummy_line_down(&p);
            add_base_i1_i2_t(&p);
            if(main_menu_bm & (u32)1 << EIID_PULSE_ON) {
                dummy_line_up(&p);
                add_i_t_impulse(&p);
            }
            add_i_kz(&p);
            dummy_line_down(&p);
        }
        Draw_line(p.x, 650 - 80, p.x, 650, 5, 0xDEDB); //вертикальная черта в конце
        Draw_line(40, 650 - 80, 1240, 650 - 80, 5, 0xDEDB);//2 горизонтальные линии
        Draw_line(40, 650, 1240, 650, 5, 0xDEDB);
    }

    if(Welding_state == 0) { //режим настройкаи параметров
        Par_big_text_sp = Draw_text(
        0, 
        0,
        SCREEN_WIDTH - 1,
        255,  
        0,38, 
        175,255, 
        TEXT_INTERVAL_1 | TEXT_ALIGNMENT_CENTER | TEXT_ALIGNMENT_VERTICAL_UP | TEXT_ENC_ASCII, //for big unicode font use ascii encode and icl is font id
        PINK);

        prev_par_id = 0;
        cur_par_id  = EIID_BASE_I2;
        par_select(EIID_BASE_I2);
        prev_par_id = EIID_BASE_I2;
        
    } else if (Welding_state == 8) {
        Amp_text_sp = Draw_text(
        0, 
        0,
        SCREEN_WIDTH - 300,
        255,  
        0,38, 
        175,255, 
        TEXT_INTERVAL_1 | TEXT_ALIGNMENT_RIGHT | TEXT_ALIGNMENT_VERTICAL_UP | TEXT_ENC_ASCII, //for big unicode font use ascii encode and icl is font id
        RED); 

        Volt_text_sp = Draw_text(
        930, 
        74,
        SCREEN_WIDTH - 1,
        128 + 74, 
        0,0, 
        64,128, 
        TEXT_INTERVAL_1 | TEXT_ALIGNMENT_LEFT | TEXT_ALIGNMENT_VERTICAL_UP | TEXT_ENC_GBK, //for big unicode font use ascii encode and icl is font id
        DARK_GREEN);
    }

  
    //par_select(EIID_BASE_I2); //по умолчанию выбран ток базы для отображения и настройки после отрисовки циклограммы

    Draw_end(); //зачистка не используемых элементов
} 






