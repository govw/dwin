#include "temp.h"
#include "sys.h"
#include "string.h"
#include "drawing.h"


code const u8 LINE_HIGHT   = 13;//px
idata u16 LINE_WIDTH  = 150;//px
idata u16 FONT_SIZE = 24;
code const u8 LEVEL_HEIGHT = 120;//px
code const u8 TOUCH_HEIGHT = 100;//px
idata u16 TIME_Y_LEVEL = 800 - 150;

typedef enum { //для id картинок //не адресации в списке параметров
    EIID_TIG,       EIID_TIG_SPOT, EIID_MMA,
    EIID_AC,        EIID_AC_MIX,   EIID_DC_MINUS, EIID_DC_PLUS,
    EIID_2T,        EIID_4T,       EIID_4T_PLUS,
    EIID_HF,        EIID_LIFT,
    EIID_PULSE_OFF, EIID_PULSE_ON,
    EIID_D_10,      EIID_D_16,     EIID_D_20,     EIID_D_24, EIID_D_32, EIID_D_40,
    EIID_SIN,       EIID_TRIG,     EIID_RECT,     EIID_TRAP,
    EIID_H2O_ON,    EIID_H2O_OFF,
    
    EIID_PRE_FLOW_T1,  //Т1  время предв. продувки, с
    EIID_START_I1,     //I1  начальный ток, А
    EIID_START_T2,     //T2  время начального тока, c
    EIID_UP_T3,        //Т3  время нарастания, с
    EIID_BASE_I2,      //I2  сварочный ток (ток базы), А
    EIID_BASE_T4,      //Т4  время тока базы, с
    EIID_IMPULSE_I3,   //I3  импульсный ток, А
    EIID_IMPULSE_T5,   //Т5  время тока импульса, с
    EIID_FREQ_F1,      //F1  частота, Гц
    EIID_BALANCE_D1,   //D1  баланс *
    EIID_DOWN_T6,      //Т6  время спада, с
    EIID_END_I4,       //I4  ток окончания сварки, А
    EIID_END_T7,       //T7  время конечного тока, с
    EIID_POST_FLOW_T8, //Т8  время продувки в конце, с
    EIDD_KZ_I5,        //I5  ток короткого замыкания, %
    EIID_BASE2_I2X,    //I2х второй ток базы (только в 4Т+), А
    EIID_SIZE,
};

enum {
    MEN_WELD_MOD, 
    MEN_BTN_MOD,
    MEN_POL_MOD, 
    MEN_IGN_MOD,
    MEN_PLS_MOD,
    MEN_PLS_WAVE_SHAPE_MOD,  
    MEN_D_MOD,   
    MEN_FLOW_MOD,
};

enum {
    TIG,
    TIG_SPOT,
    MMA,
};

code const u32 active_items[] = {
    ((u32)1 << EIID_TIG)       | ((u32)1 << EIID_TIG_SPOT) | ((u32)1 << EIID_MMA)      |
    ((u32)1 << EIID_2T)        | ((u32)1 << EIID_4T)       | ((u32)1 << EIID_4T_PLUS)  |
    ((u32)1 << EIID_AC)        | ((u32)1 << EIID_AC_MIX)   | ((u32)1 << EIID_DC_MINUS) |
    ((u32)1 << EIID_HF)        | ((u32)1 << EIID_LIFT)     |
    ((u32)1 << EIID_PULSE_OFF) | ((u32)1 << EIID_PULSE_ON) |
    ((u32)1 << EIID_SIN)       | ((u32)1 << EIID_TRIG)     | ((u32)1 << EIID_RECT)     | ((u32)1 << EIID_TRAP) |
    ((u32)1 << EIID_D_10)      | ((u32)1 << EIID_D_16)     | ((u32)1 << EIID_D_20)     | ((u32)1 << EIID_D_24) | ((u32)1 << EIID_D_32) | ((u32)1 << EIID_D_40) |
    ((u32)1 << EIID_H2O_OFF)   | ((u32)1 << EIID_H2O_ON),    //tig;

    ((u32)1 << EIID_TIG)       | ((u32)1 << EIID_TIG_SPOT) | ((u32)1 << EIID_MMA)      |
    ((u32)1 << EIID_2T)        | 
    ((u32)1 << EIID_AC)        | ((u32)1 << EIID_AC_MIX)   | ((u32)1 << EIID_DC_MINUS) |
    ((u32)1 << EIID_HF)        | ((u32)1 << EIID_LIFT)     |
    ((u32)1 << EIID_PULSE_OFF) | ((u32)1 << EIID_PULSE_ON) |
    ((u32)1 << EIID_SIN)       | ((u32)1 << EIID_TRIG)     | ((u32)1 << EIID_RECT)     | ((u32)1 << EIID_TRAP) |
    ((u32)1 << EIID_D_10)      | ((u32)1 << EIID_D_16)     | ((u32)1 << EIID_D_20)     | ((u32)1 << EIID_D_24) | ((u32)1 << EIID_D_32) | ((u32)1 << EIID_D_40) |
    ((u32)1 << EIID_H2O_OFF)   | ((u32)1 << EIID_H2O_ON),     //tig /spot;

    ((u32)1 << EIID_TIG)       | ((u32)1 << EIID_TIG_SPOT) | ((u32)1 << EIID_MMA)     |
    ((u32)1 << EIID_AC)        | ((u32)1 << EIID_DC_MINUS) | ((u32)1 << EIID_DC_PLUS) |


    ((u32)1 << EIID_PULSE_OFF) | ((u32)1 << EIID_PULSE_ON),     //mma
};

code const u32 menu_item_bm[] = { 
    ((u32)1 << EIID_TIG)       | ((u32)1 << EIID_TIG_SPOT) | ((u32)1 << EIID_MMA),      
    ((u32)1 << EIID_2T)        | ((u32)1 << EIID_4T)       | ((u32)1 << EIID_4T_PLUS),    
    ((u32)1 << EIID_AC)        | ((u32)1 << EIID_AC_MIX)   | ((u32)1 << EIID_DC_MINUS) | ((u32)1 << EIID_DC_PLUS),                                             
    ((u32)1 << EIID_HF)        | ((u32)1 << EIID_LIFT),
    ((u32)1 << EIID_PULSE_OFF) | ((u32)1 << EIID_PULSE_ON),
    ((u32)1 << EIID_SIN)       | ((u32)1 << EIID_TRIG)     | ((u32)1 << EIID_RECT)     | ((u32)1 << EIID_TRAP),                                                
    ((u32)1 << EIID_D_10)      | ((u32)1 << EIID_D_16)     | ((u32)1 << EIID_D_20)     | ((u32)1 << EIID_D_24) | ((u32)1 << EIID_D_32) | ((u32)1 << EIID_D_40),
    ((u32)1 << EIID_H2O_OFF)   | ((u32)1 << EIID_H2O_ON),   
}; 

//те элементы которые будут отображены в горизонтальном меню
idata u32 main_menu_bm = (((u32)1 << EIID_TIG)       |
                         ((u32)1 << EIID_2T)        |
                         ((u32)1 << EIID_AC)        |
                         ((u32)1 << EIID_HF)        |
                         ((u32)1 << EIID_PULSE_OFF) |
                         ((u32)1 << EIID_SIN)       |
                         ((u32)1 << EIID_D_10)      |
                         ((u32)1 << EIID_H2O_OFF)      
                               
);
//битовая маска горизонтального меню на нижнем экране необходима первоначальная инициализация

icon_t cur_menu[48];
u32 cur_menu_active;
u8 cur_menu_size;

u8 cur_active_items_id = TIG;

u8 curent_selected_param_img_id = 0;

u16 controllsp[EIID_SIZE]; //массив sp идентификаторов линий, картинок, чисел

idata u8 cur_par_id = 28; //номер текущего параметра к отображению
code const u16 ICON_RECT_SZ = 128;

u16 icon_sp_bottom_menu[16]; //sp картинок нижнего горизонтального меню // для их изменения

void (*cur_menu_fanc)(u8 item_pos);
void change_mode_control(u8 item_pos);
void make_menu_bar(u8 is_vertical, u16 start_x, u16 start_y, u32 id_bm, void (*cur_menu_fanc)(u8 item_pos));












void do_nothing(u8 item_pos)
{
    static u8 sw = 0;

    if(sw & 1){
        //change_image_id(curent_selected_param_img_id, 28);
        //change_number_color(dgus_variables_display_sp_cylcogramm_numbers[0], PINK);
    } else {
        //change_image_id(curent_selected_param_img_id, 29);
        //change_number_color(dgus_variables_display_sp_cylcogramm_numbers[0], GREEN);
    }
        

    sw++;
    return;
}




void make_menu_bar(u8 is_vertical, u16 start_x, u16 start_y, u32 id_bm, void (*fanc)(u8 item_pos))
{
    u8 i; 
    s16 shift_x = 0;
    s16 shift_y = 0;

    if(is_vertical)
        shift_y = -ICON_RECT_SZ;
    else
        shift_x = ICON_RECT_SZ;

    i = 0;

  
    while (id_bm != 0) {
        if((u32)id_bm & (u32)1) {
            cur_menu[cur_menu_size].ico = i;
            cur_menu[cur_menu_size].r.x0 = start_x;
            cur_menu[cur_menu_size].r.y0 = start_y;
            cur_menu[cur_menu_size].r.x1 = start_x + ICON_RECT_SZ;
            cur_menu[cur_menu_size].r.y1 = start_y + ICON_RECT_SZ;
            start_x += shift_x;
            start_y += shift_y;
            cur_menu_size++;        
        }
        id_bm >>= 1;
        i++;
    }

    cur_menu_fanc = fanc;
    
    for(i = 0; i < cur_menu_size ; i++) {
        draw_image(cur_menu[i].r.x0, cur_menu[i].r.y0, cur_menu[i].ico);  
    }
}

void bottom_level_controls(u8 item_pos)
{ //вызовется после выбора элемента в горизонтальном меню
    u32 bm;
    u8 i;
    //получить индекс типа меню 
    for(i = 0; i < ARR_SIZE(menu_item_bm); i++) {
        if((u32)menu_item_bm[i] & (u32)((u32)1 << cur_menu[item_pos].ico)) { //поиск меню к которму относится данный элемент
            bm = active_items[cur_active_items_id] & menu_item_bm[i];//убрать из отображения эелементы которые не доступны в данном режиме 
            bm &= ~((u32)1 << cur_menu[item_pos].ico); //убрать из отборажения элемент который уже высвечен в нижнем меню
            if(bm != 0 )//если нечего выводить не рисовать горизонтальное меню
            {
                
                
                //make_menu_bar(1, ICON_RECT_SZ * item_pos, 800 - (128 * 2), bm, change_mode_control);   //10 картинок зарезервиновано до этого      
            }
                
            break;
        }
    }
}


void change_mode_control(u8 item_pos) 
{//вызовется после выбора эелемента в вертикальном меню
    u8 i;
    u32 bm;
    for(i = 0; i < ARR_SIZE(menu_item_bm); i++) {
        if(menu_item_bm[i] & (u32)1 << cur_menu[item_pos].ico) {
            main_menu_bm &= (u32)~menu_item_bm[i]; //очисть область связанную с этим подменю в гавном меню
            switch (cur_menu[item_pos].ico)
            {
                case EIID_TIG:
                    cur_active_items_id = TIG;
                    break;
                
                case EIID_TIG_SPOT:
                    cur_active_items_id = EIID_TIG_SPOT;
                    break;

                case EIID_MMA:
                    cur_active_items_id = MMA;
                    break;
            }
            main_menu_bm |= ((u32)1 << cur_menu[item_pos].ico); //вставить текущий выбранный элемент в главное меню
            
            bm = active_items[cur_active_items_id];
            bm &= (u32)main_menu_bm;
            
            
            //после каждой зачистки картиног нужно отобразить некторые из них завново
           
            //make_menu_bar(0, 0, 800 - ICON_RECT_SZ, bm, VAR_ICON_START_VP, bottom_level_controls);
            
            break;
        }
    }
}



void draw_bottom_menu(void)
{   
    
}



void add_touch_place(u16 x0, u16 y0, u16 x1, u16 y1, u8 touch_id)
{
    if(cur_menu_size > ARR_SIZE(cur_menu)) return;

    cur_menu[cur_menu_size].ico = touch_id;
    cur_menu[cur_menu_size].r.x0 = x0;
    cur_menu[cur_menu_size].r.y0 = y0;
    cur_menu[cur_menu_size].r.x1 = x1;
    cur_menu[cur_menu_size].r.y1 = y1; // размер кнопки
    cur_menu_size++;        
}







// LINE_HIGHT   = 13;//px
// LINE_WIDTH   = 150;//px
// LEVEL_HEIGHT = 120;//px
// TOUCH_HEIGHT = 100;//px

void draw_number_centered_on_line(point_t line_start, u16 n, u8 decimal_places, u8* units, u16 font_size, u16 color, u8 touch_id)
{
    u16 integer_digits;

    if      (n < 10)    integer_digits = 1;
    else if (n < 100)   integer_digits = 2;
    else if (n < 1000)  integer_digits = 3;
    else if (n < 10000) integer_digits = 4;

    integer_digits += strlen(units) + decimal_places;

    Draw_Number(line_start.x + ((LINE_WIDTH - (integer_digits * font_size)) / 2L), 
                line_start.y - (font_size * 2) - LINE_HIGHT, 
                n, 
                decimal_places, 
                units, 
                font_size, 
                color);

    add_touch_place(line_start.x, line_start.y - (font_size * 2), line_start.x + LINE_WIDTH, line_start.y, touch_id);
}

   
point_t dummy_line_up(point_t p)
{
    point_t p2; 
    p2 = make_point(p.x, p.y - LEVEL_HEIGHT);
    draw_line(p.x, p.y, p2.x, p2.y, LINE_HIGHT, GREEN);
    return p2;
}

point_t dummy_line_down(point_t p)
{
    point_t p2; 
    p2 = make_point(p.x, p.y + LEVEL_HEIGHT);
    draw_line(p.x, p.y, p2.x, p2.y, LINE_HIGHT, GREEN);
    return p2;
}

point_t add_t_preflow(point_t p) //возвращает конечную точку после отрисовки данного участка
{
    point_t p2;
    p2 = make_point(p.x + LINE_WIDTH, p.y);

    draw_line(p.x, p.y, p2.x, p2.y, LINE_HIGHT, GREEN);

    //add_touch_place(p.x, p.y, p2.x, TIME_Y_LEVEL + TOUCH_HEIGHT, EIID_PRE_FLOW_T1);//создание области касания для настройки времени продувки
    draw_number_centered_on_line(make_point(p.x, TIME_Y_LEVEL), 10 , 1, "s", 30, CYAN, EIID_PRE_FLOW_T1); //время продувки
    return p2;    
}



point_t add_t_start_i(point_t p) 
{
    point_t p2; 
    p2 = make_point(p.x + LINE_WIDTH, p.y);
    draw_line(p.x, p.y, p2.x, p2.y,  LINE_HIGHT, GREEN);
    
    draw_number_centered_on_line(p, 99, 0, "A", FONT_SIZE, RED, EIID_START_I1);//начальный ток
    draw_number_centered_on_line(make_point(p.x, TIME_Y_LEVEL), 99, 1, "s", 30, CYAN, EIID_START_T2);//время начального тока

    return p2;
}


point_t add_t_up(point_t p) 
{
    point_t p2;
    p2 = make_point(p.x + LINE_WIDTH, p.y - LEVEL_HEIGHT);
    draw_line(p.x, p.y, p2.x, p2.y,  LINE_HIGHT, GREEN);
    
    draw_number_centered_on_line(make_point(p.x, TIME_Y_LEVEL), 99, 1, "s", 30, CYAN, EIID_UP_T3); //время наростания

    return p2;
}

point_t add_base_i1_i2_t(point_t p) 
{
    point_t p2;
    u16 half_height = LINE_HIGHT / 2;
    p2 = make_point(p.x + LINE_WIDTH, p.y);

    draw_line(p.x, p.y, p2.x, p2.y,  LINE_HIGHT, GREEN);

    draw_number_centered_on_line(make_point(p.x, TIME_Y_LEVEL), 99, 1, "s", 30, CYAN, EIID_BASE_T4); //время базы
    draw_number_centered_on_line(p, 99, 0, "A", FONT_SIZE, RED, EIID_BASE_I2); //ток базы
    draw_number_centered_on_line(make_point(p.x, p.y + (FONT_SIZE * 2) + (LINE_HIGHT * 2)), 99, 0, "A", FONT_SIZE, RED, EIID_BASE2_I2X); //второй ток базы
    return p2;
}

point_t add_i_t_impulse(point_t p) 
{
    point_t p2;
    p2 = make_point(p.x + LINE_WIDTH, p.y);

    draw_line(p.x, p.y, p2.x, p2.y,  LINE_HIGHT, GREEN);

    draw_number_centered_on_line(make_point(p.x, TIME_Y_LEVEL), 99, 1, "s", 30, CYAN, EIID_IMPULSE_T5);// время импульса
    draw_number_centered_on_line(p, 99, 0, "A", FONT_SIZE, RED, EIID_IMPULSE_I3);  //ток импульса

    return p2;
}

point_t add_t_down(point_t p)
{
    point_t p2;
    p2 = make_point(p.x + LINE_WIDTH, p.y + LEVEL_HEIGHT);

    draw_line(p.x, p.y, p2.x, p2.y,  LINE_HIGHT, GREEN);

    draw_number_centered_on_line(make_point(p.x, TIME_Y_LEVEL), 99, 1, "s", 30, CYAN, EIID_DOWN_T6); //время спада
    return p2;
}

point_t add_end_i_t(point_t p)
{
    point_t p2;
    p2 = make_point(p.x + LINE_WIDTH, p.y);

    draw_line(p.x, p.y, p2.x, p2.y,  LINE_HIGHT, GREEN);
    
    draw_number_centered_on_line(make_point(p.x, TIME_Y_LEVEL), 99, 1, "s", 30, CYAN, EIID_END_T7); //время конечного тока
    draw_number_centered_on_line(p, 99, 0, "A", FONT_SIZE, RED, EIID_END_I4); //конечный ток
    return p2;
}

point_t add_t_postflow(point_t p)
{
    point_t p2;
    p2 = make_point(p.x + LINE_WIDTH, p.y);

    draw_line(p.x, p.y, p2.x, p2.y,  LINE_HIGHT, GREEN);
    
    draw_number_centered_on_line(make_point(p.x, TIME_Y_LEVEL), 99, 1, "s", 30, CYAN, EIID_POST_FLOW_T8);//время конечной продувки
    return p2;
}


point_t add_i_kz(point_t p)
{
    point_t p2;
    p2 = make_point(p.x + LINE_WIDTH, p.y);

    return p2;   
}





void make_bottom_menu(void)
{
    cur_active_items_id = TIG;
    cur_menu_size = 0;

    {
        u8 i;
        u8 id; 
        u16 start_x = 0;
        u16 start_y = 800 - ICON_RECT_SZ;
        u32 id_bm = main_menu_bm;
        id = 0;
        i = 0;
        while (id_bm != 0) {
            if((u32)id_bm & (u32)1) {
                add_touch_place(start_x, start_y, start_x + ICON_RECT_SZ, start_y + ICON_RECT_SZ, i);
                image_change_id(icon_sp_bottom_menu[i], id);
                image_change_pos(icon_sp_bottom_menu[i], make_point(start_x, start_y));
                start_x += ICON_RECT_SZ;
                i++;
            }
            id_bm >>= 1;
            id++;
        }
    }
 
}




void place_numbers_on_cyclogramm(void) 
{
    u16 x = 40;
    u16 y = 550;
    
    //Drawing_Clear_Screen();
    {
        u8 i;
        for(i = 0; i < ARR_SIZE(icon_sp_bottom_menu); i++) {
            icon_sp_bottom_menu[i] = draw_image(0, 0, 0xffff);
        }
    }
    make_bottom_menu();
    
    // {
    //     u8 i;
    //     u16 new_id = 0;

    //     while (1)
    //     {
    //         for (i = 0; i < 8; i++) {
    //             image_change_id(icon_sp_bottom_menu[i], new_id);
    //         }
    //         new_id++;
    //         if(new_id == 10) new_id = 0;
    //         delay_ms(300);
    //     }
    // }




    // {//tig pulse ac
    //     point_t p;
    //     LINE_WIDTH = 150;
    //     FONT_SIZE = 37; 
    //     p = add_t_preflow(make_point(x, y));
    //     p = dummy_line_up(p);
    //     p = add_t_start_i(p);
    //     p = add_t_up(p);
    //     p = add_base_i1_i2_t(p);
    //     p = dummy_line_down(p);
    //     p = add_i_t_impulse(p);
    //     p = dummy_line_up(p);
    //     p = add_t_down(p);
    //     p = add_end_i_t(p);
    //     p = dummy_line_down(p);
    //     p = add_t_postflow(p);
    // }
    
   
    // {//mma
    //     point_t p;
    //     LINE_WIDTH = 300;
    //     FONT_SIZE = 64;
    //     p = dummy_line_up(make_point(40, 450));
    //     p = add_t_start_i(p);
    //     p = dummy_line_down(p);
    //     p = add_base_i1_i2_t(p);
    //     p = dummy_line_up(p);
    //     p = add_i_t_impulse(p);
    //     p = dummy_line_up(p);
    //     p = add_i_kz(p);
    //     p = dummy_line_down(p);
    //     p = dummy_line_down(p);
    // }
    
    // Draw_Number(0,0, 999, 0, "A", 64, PINK);
        
    
     
   // cur_menu_fanc = do_nothing;  
   

    
        
     
    //draw_bottom_menu();
}



