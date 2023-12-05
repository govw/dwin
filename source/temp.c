#include "temp.h"
#include "sys.h"
#include "string.h"
#include "stdio.h"
#include "drawing.h"
#include "encoder.h"

code const u8  LINE_HIGHT   = 13;//px
idata      u16 LINE_WIDTH   = 150;//px
idata      u16 FONT_SIZE    = 24;
code const u8  LEVEL_HEIGHT = 120;//px
code const u8  TOUCH_HEIGHT = 100;//px
idata      u16 TIME_Y_LEVEL = 800 - 150;
code const u16 ICON_RECT_SZ = 128;





//tig_spot только 2t
code const u32 active_items[] = {
    ((u32)1 << EIID_TIG)       | ((u32)1 << EIID_MMA)      |
    ((u32)1 << EIID_2T)        | ((u32)1 << EIID_4T)       | ((u32)1 << EIID_4T_PLUS)  | ((u32)1 << EIID_TIG_SPOT) |
    ((u32)1 << EIID_AC)        | ((u32)1 << EIID_AC_MIX)   | ((u32)1 << EIID_DC_MINUS) |
    ((u32)1 << EIID_HF)        | ((u32)1 << EIID_LIFT)     |
    ((u32)1 << EIID_PULSE_OFF) | ((u32)1 << EIID_PULSE_ON) |
    ((u32)1 << EIID_SIN)       | ((u32)1 << EIID_TRIG)     | ((u32)1 << EIID_RECT)     | ((u32)1 << EIID_TRAP) |
    ((u32)1 << EIID_D_10)      | ((u32)1 << EIID_D_16)     | ((u32)1 << EIID_D_20)     | ((u32)1 << EIID_D_24) | ((u32)1 << EIID_D_32) | ((u32)1 << EIID_D_40) |
    ((u32)1 << EIID_H2O_OFF)   | ((u32)1 << EIID_H2O_ON),    //tig;

    ((u32)1 << EIID_TIG)       | ((u32)1 << EIID_MMA)     |
    ((u32)1 << EIID_AC)        | ((u32)1 << EIID_DC_MINUS) | ((u32)1 << EIID_DC_PLUS) |
    ((u32)1 << EIID_PULSE_OFF) | ((u32)1 << EIID_PULSE_ON),     //mma
};

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

//те элементы которые будут отображены в горизонтальном меню и то какая сейчас настройка
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

u8 idata cur_active_items_id = TIG;


u16 text_sp[EIID_SIZE]; //массив sp идентификаторов линий, картинок, чисел
s16 par[EIID_SIZE];
xdata u16 par_tek[36];//параметры для силовой части
u16 cur_par_id; //номер текущего параметра к отображению

typedef struct {
    s16 min;
    s16 max;
}lim_t;
lim_t par_lim[EIID_SIZE];
s16   par_step[EIID_SIZE];





u16 icon_sp_bottom_menu[16]; //sp картинок нижнего горизонтального меню // для их изменения

void (*cur_menu_fanc)(u8 item_pos);
void cur_par_value_change(u16 cur_par_id, s8 shift);
void display_par(u16 par_id);

void make_lim(lim_t *lim, s16 min, s16 max) {
    lim->min = min;
    lim->max = max;
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






void draw_number_wtih_touch_centered(point_t line_start, u16 font_size, u16 color, u8 touch_id)
{
    text_sp[touch_id] = Draw_text(line_start.x - 50, line_start.y - (font_size * 2), line_start.x + LINE_WIDTH + 50, line_start.y, font_size, color);
    display_par(touch_id);
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
    draw_number_wtih_touch_centered(make_point(p.x, TIME_Y_LEVEL), 36, CYAN, EIID_PRE_FLOW_T1); //время продувки
    return p2;    
}



point_t add_t_start_i(point_t p) 
{
    point_t p2; 
    p2 = make_point(p.x + LINE_WIDTH, p.y);
    draw_line(p.x, p.y, p2.x, p2.y,  LINE_HIGHT, GREEN);
    
    draw_number_wtih_touch_centered(p, FONT_SIZE, YELLOW, EIID_START_I1);//начальный ток
    draw_number_wtih_touch_centered(make_point(p.x, TIME_Y_LEVEL), 36, CYAN, EIID_START_T2);//время начального тока

    return p2;
}


point_t add_t_up(point_t p) 
{
    point_t p2;
    p2 = make_point(p.x + LINE_WIDTH, p.y - LEVEL_HEIGHT);
    draw_line(p.x, p.y, p2.x, p2.y,  LINE_HIGHT, GREEN);
    
    draw_number_wtih_touch_centered(make_point(p.x, TIME_Y_LEVEL), 36, CYAN, EIID_UP_T3); //время наростания

    return p2;
}

point_t add_base_i1_i2_t(point_t p) 
{
    point_t p2;
    u16 half_height = LINE_HIGHT / 2;
    p2 = make_point(p.x + LINE_WIDTH, p.y);

    draw_line(p.x, p.y, p2.x, p2.y,  LINE_HIGHT, GREEN);

    draw_number_wtih_touch_centered(make_point(p.x, TIME_Y_LEVEL), 30, CYAN, EIID_BASE_T4); //время базы
    draw_number_wtih_touch_centered(p, FONT_SIZE, PINK, EIID_BASE_I2); //ток базы
    if(main_menu_bm & (u32)1 << EIID_4T_PLUS) 
        draw_number_wtih_touch_centered(make_point(p.x, p.y + (FONT_SIZE * 2) + (LINE_HIGHT * 2)), FONT_SIZE, YELLOW, EIID_BASE2_I2X); //второй ток базы
    return p2;
}

point_t add_i_t_impulse(point_t p) 
{
    point_t p2;
    p2 = make_point(p.x + LINE_WIDTH, p.y);

    draw_line(p.x, p.y, p2.x, p2.y,  LINE_HIGHT, GREEN);

    draw_number_wtih_touch_centered(make_point(p.x, TIME_Y_LEVEL), 30, CYAN, EIID_IMPULSE_T5);// время импульса
    draw_number_wtih_touch_centered(p, FONT_SIZE, YELLOW, EIID_IMPULSE_I3);  //ток импульса

    return p2;
}

point_t add_t_down(point_t p)
{
    point_t p2;
    p2 = make_point(p.x + LINE_WIDTH, p.y + LEVEL_HEIGHT);

    draw_line(p.x, p.y, p2.x, p2.y,  LINE_HIGHT, GREEN);

    draw_number_wtih_touch_centered(make_point(p.x, TIME_Y_LEVEL), 36, CYAN, EIID_DOWN_T6); //время спада
    return p2;
}

point_t add_end_i_t(point_t p)
{
    point_t p2;
    p2 = make_point(p.x + LINE_WIDTH, p.y);

    draw_line(p.x, p.y, p2.x, p2.y,  LINE_HIGHT, GREEN);
    
    draw_number_wtih_touch_centered(make_point(p.x, TIME_Y_LEVEL), par[EIID_END_T7], "s", 36, CYAN, EIID_END_T7); //время конечного тока
    draw_number_wtih_touch_centered(p, FONT_SIZE, YELLOW, EIID_END_I4); //конечный ток
    return p2;
}

point_t add_t_postflow(point_t p)
{
    point_t p2;
    p2 = make_point(p.x + LINE_WIDTH, p.y);

    draw_line(p.x, p.y, p2.x, p2.y,  LINE_HIGHT, GREEN);
    
    draw_number_wtih_touch_centered(make_point(p.x, TIME_Y_LEVEL), par[EIID_POST_FLOW_T8], "s", 36, CYAN, EIID_POST_FLOW_T8);//время конечной продувки
    return p2;
}


point_t add_i_kz(point_t p)
{
    point_t p2;
    p2 = make_point(p.x + LINE_WIDTH, p.y - LEVEL_HEIGHT);
    draw_line(p.x, p.y, p2.x, p2.y, LINE_HIGHT, GREEN);

    draw_number_wtih_touch_centered(make_point(p.x, p2.y), FONT_SIZE, YELLOW, EIDD_KZ_I5);//время конечной продувки
    return p2;   
}




void bottom_level_controls(u8 item_pos);

void make_bottom_menu(void)
{
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
                add_touch_place(start_x, start_y, start_x + ICON_RECT_SZ, start_y + ICON_RECT_SZ, id);
                image_change_id(icon_sp_bottom_menu[i], id);
                image_change_pos(icon_sp_bottom_menu[i], make_point(start_x, start_y));
                start_x += ICON_RECT_SZ;
                i++;
            }
            id_bm >>= 1;
            id++;
        }

        add_touch_place(start_x, start_y, start_x + ICON_RECT_SZ, start_y + ICON_RECT_SZ, i);//резерв

        start_x += ICON_RECT_SZ;
        add_touch_place(start_x, start_y, start_x + ICON_RECT_SZ, start_y + ICON_RECT_SZ, i);
        image_change_id(icon_sp_bottom_menu[i], 0);
        image_change_pos(icon_sp_bottom_menu[i], make_point(start_x, start_y));

        for(i = 0; i < 6; i++) {
            add_touch_place(0, 0, 0, 0, 0); //резерв для вертикального меню + 6
        }

        cur_menu_fanc = bottom_level_controls;
    }
}



void par_select(u8 pair_id);

void bottom_level_controls(u8 item_pos)
{ //вызовется после выбора элемента в горизонтальном меню
    u32 bm = 0;
    u8 i;
    u8 id;     
    u16 start_x;
    u16 start_y;


    for(i = 10; i < 16; i++) { //зачистка прошлого меню
        image_change_id(icon_sp_bottom_menu[i], 0xffff);
        cur_menu[i].r.x0 = 0;
        cur_menu[i].r.y0 = 0;  
        cur_menu[i].r.x1 = 0;  
        cur_menu[i].r.y1 = 0;      
    }
    //получить индекс типа меню
    if(item_pos < 8) { //отризовать вертикальное меню
        for(i = 0; i < ARR_SIZE(menu_item_bm); i++) {
            if((u32)menu_item_bm[i] & (u32)((u32)1 << cur_menu[item_pos].ico)) { //поиск меню к которму относится данный элемент
                bm = active_items[cur_active_items_id] & menu_item_bm[i];//убрать из отображения эелементы которые не доступны в данном режиме 
                bm &= ~((u32)1 << cur_menu[item_pos].ico); //убрать из отборажения элемент который уже высвечен в нижнем меню
                if(bm != 0 ) { //если нечего выводить не рисовать горизонтальное меню
                  
                    
                    id = 0;
                    i = 10;
                    
                    start_x = ICON_RECT_SZ * item_pos;
                    start_y = 800 - (ICON_RECT_SZ * 2);
                    while (bm != 0) {
                        if((u32)bm & (u32)1) {
                            image_change_pos(icon_sp_bottom_menu[i], make_point(start_x, start_y));
                            image_change_id(icon_sp_bottom_menu[i], id);
                            
                            cur_menu[i].r.x0 = start_x;
                            cur_menu[i].r.y0 = start_y;
                            cur_menu[i].r.x1 = start_x + ICON_RECT_SZ;
                            cur_menu[i].r.y1 = start_y + ICON_RECT_SZ;
                            cur_menu[i].ico  = id;
                            start_y -= ICON_RECT_SZ; 
                            i++;
                        }
                        bm >>= 1;
                       
                        id++;
                    } 
                }
                break;
            }
        }
    } else { //еслы выбор из вертикального меню //елси позция болше 10
        
        if(item_pos > 15)  { //вбор из настройки параметров
           par_select(cur_menu[item_pos].ico);
           return;
        }


        for(i = 0; i < ARR_SIZE(menu_item_bm); i++) {
            if(menu_item_bm[i] & (u32)1 << cur_menu[item_pos].ico) {
                main_menu_bm &= (u32)~menu_item_bm[i]; //очисть область связанную с этим подменю в гавном меню
                
                main_menu_bm |= ((u32)1 << cur_menu[item_pos].ico); //вставить текущий выбранный элемент в главное меню
                
                
                cur_par_value_change(cur_menu[item_pos].ico, 0); //изменение параметра для силовой части

                switch (cur_menu[item_pos].ico)
                {
                    case EIID_TIG: {
                        cur_active_items_id = TIG;
                        make_scene(); 
                    } break;

                    case EIID_TIG_SPOT: { //в режиме tig_spot есть только 2_t
                        cur_active_items_id = TIG;
                        bm &= ~(menu_item_bm[MEN_BTN_MOD]);
                        bm |= ((u32)1 << EIID_2T);
                        make_scene(); 
                    } break;

                    case EIID_MMA: {
                        cur_active_items_id = MMA;
                        make_scene(); 
                    } break;

                    case EIID_2T:
                    case EIID_4T:
                    case EIID_4T_PLUS: {
                        make_scene();         
                    } break;

                    case EIID_PULSE_ON :
                    case EIID_PULSE_OFF: {
                        make_scene();         
                    } break;
                }

                bm = main_menu_bm & active_items[cur_active_items_id];
                
                

                for(i = 0; i < 8; i++) { //зачистка прошлого меню
                    image_change_id(icon_sp_bottom_menu[i], 0xffff);    
                }
               
                id = 0;
                i = 0;
                start_x = 0;
                start_y = 800 - (ICON_RECT_SZ);

                while (bm != 0) { //отрисовка нижнего меню
                    if((u32)bm & (u32)1) {
                        image_change_pos(icon_sp_bottom_menu[i], make_point(start_x, start_y));
                        image_change_id(icon_sp_bottom_menu[i], id);
                        
                        cur_menu[i].r.x0 = start_x;
                        cur_menu[i].r.y0 = start_y;
                        cur_menu[i].r.x1 = start_x + ICON_RECT_SZ;
                        cur_menu[i].r.y1 = start_y + ICON_RECT_SZ;
                        cur_menu[i].ico  = id;
                        start_x += ICON_RECT_SZ; 
                        i++;
                    }
                    bm >>= 1;
                    id++;
                }
                for(; i < 10; i++) { //зачистка областей касания
                    cur_menu[i].r.x0 = 0;
                    cur_menu[i].r.y0 = 0;
                    cur_menu[i].r.x1 = 0;
                    cur_menu[i].r.y1 = 0;
                } 
               
                break;
            }
        }
    }
    
}




void display_par(u16 par_id) 
{
    switch (par_id)
    {
        case EIID_PRE_FLOW_T1:
        case EIID_START_T2:     
        case EIID_UP_T3:           
        case EIID_DOWN_T6:      
        case EIID_END_T7: {
            Draw_text_change_text("%.1fs", text_sp[par_id], par[par_id] / (float)10.0);
        } break;       

        case EIID_BASE_T4:      
        case EIID_IMPULSE_T5: {
            Draw_text_change_text("%dms", text_sp[par_id], par[par_id]);
        } break;

        case EIID_POST_FLOW_T8: {
            Draw_text_change_text("%ds", text_sp[par_id], par[par_id]); 
        } break;
           
        case EIID_START_I1:     
        case EIID_BASE_I2:      
        case EIID_BASE2_I2X:
        case EIID_IMPULSE_I3:   
        case EIID_END_I4:       
        case EIDD_KZ_I5: {
            Draw_text_change_text("%dA", text_sp[par_id], par[par_id]); 
        } break;       

        case EIID_FREQ_F1:      
        case EIID_BALANCE_D1: {

        } break;   
    }
}

void cur_par_value_change(u16 cur_par_id, s8 shift)
{
    if(shift != 0) {
        par[cur_par_id] += ((s16)par_step[cur_par_id] * shift); 
        if(par[cur_par_id] > par_lim[cur_par_id].max)       
            par[cur_par_id] = par_lim[cur_par_id].max;
        else if(par[cur_par_id] < par_lim[cur_par_id].min) 
            par[cur_par_id] = par_lim[cur_par_id].min;
    } 
   
    switch (cur_par_id)
    {
        case EIID_PRE_FLOW_T1:
        case EIID_START_T2   :
        case EIID_UP_T3      :
        case EIID_DOWN_T6    :
        case EIID_END_T7     : {
            data u16 tmp = par[cur_par_id];
            switch (cur_par_id)
            {
                case EIID_PRE_FLOW_T1: par_tek[EPID_PRE_FLOW_T1] = tmp; break;    
                case EIID_START_T2   : par_tek[EPID_START_T2]    = tmp; break;    
                case EIID_UP_T3      : par_tek[EPID_UP_T3]       = tmp; break;    
                case EIID_DOWN_T6    : par_tek[EPID_DOWN_T6]     = tmp; break;    
                case EIID_END_T7     : par_tek[EPID_END_T7]      = tmp; break;     
            }
            display_par(cur_par_id);
        } break;
        
        case EIID_POST_FLOW_T8: {
            display_par(cur_par_id);    
        } break;

        case EIID_BASE_T4   :
        case EIID_IMPULSE_T5: {
            display_par(cur_par_id);
        } break;
       
        case EIID_START_I1  :     
        case EIID_BASE_I2   :
        case EIID_BASE2_I2X :  
        case EIID_IMPULSE_I3: 
        case EIID_END_I4    :
        case EIDD_KZ_I5     :
        case EIID_FREQ_F1   :
        case EIID_BALANCE_D1: {
            data u16 tmp = par[cur_par_id];
            switch (cur_par_id)
            {
                case EIID_START_I1  : par_tek[EPID_START_I1]   = tmp; break;    
                case EIID_BASE_I2   : par_tek[EPID_BASE_I2]    = tmp; break;
                case EIID_BASE2_I2X : par_tek[EPID_BASE2_I2X]  = tmp; break; 
                case EIID_IMPULSE_I3: par_tek[EPID_IMPULSE_I3] = tmp; break;
                case EIID_END_I4    : par_tek[EPID_END_I4]     = tmp; break;
                case EIDD_KZ_I5     : par_tek[EPID_KZ_I5]      = tmp; break;
                case EIID_FREQ_F1   : par_tek[EPID_FREQ_F1]    = tmp; break;
                case EIID_BALANCE_D1: par_tek[EPID_BALANCE_D1] = tmp; break;   
            }
            display_par(cur_par_id);        
        }break;

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
                u16 pulse : 4; //1-пульс 2-не пульс
                u16 pol   : 4; //1-DC  2-AC  3-MIX
                u16 mod   : 4; //1-TIG 2-MMA 3-TIG Spot
            } tmp;
            *(u16*)&tmp = par_tek[EPID_MODES_SUBMODES]; 
            switch (cur_par_id)
            {
                case EIID_PULSE_OFF: tmp.pulse = 1; break; 
                case EIID_PULSE_ON : tmp.pulse = 0; break;
                case EIID_AC       : tmp.pol = 1;   break;
                case EIID_AC_MIX   : tmp.pol = 2;   break;
                case EIID_DC_MINUS : tmp.pol = 0; par_tek[EPID_POLARITY] = 2; break;
                case EIID_DC_PLUS  : tmp.pol = 0; par_tek[EPID_POLARITY] = 1; break;
                case EIID_TIG      : tmp.mod = 0;   break;
                case EIID_MMA      : tmp.mod = 1;   break;
                case EIID_TIG_SPOT : tmp.mod = 2;   break;
            }
            par_tek[EPID_MODES_SUBMODES] = *(u16*)&tmp;
        } break;
    
        case EIID_2T : par_tek[EPID_2T_4T] = 1; break;
        case EIID_4T : par_tek[EPID_2T_4T] = 2; par_tek[EPID_IS_PLUS_4T] = 2; break;//4t plus mode off
        case EIID_4T_PLUS : {
            par_tek[EPID_IS_PLUS_4T] = 1; //4t plus mode
        } break;

        case EIID_HF  : par_tek[EPID_IGNITION_TYPE] = 2; break;
        case EIID_LIFT: par_tek[EPID_IGNITION_TYPE] = 1; break;

        case EIID_H2O_OFF: par_tek[EPID_FLOW_SENSOR] = 1; break; 
        case EIID_H2O_ON : par_tek[EPID_FLOW_SENSOR] = 2; break;

        case EIID_D_10: par_tek[EPID_ELECTRODE_D] = 1; break;    
        case EIID_D_16: par_tek[EPID_ELECTRODE_D] = 2; break;
        case EIID_D_20: par_tek[EPID_ELECTRODE_D] = 3; break;    
        case EIID_D_24: par_tek[EPID_ELECTRODE_D] = 4; break;
        case EIID_D_32: par_tek[EPID_ELECTRODE_D] = 5; break;
        case EIID_D_40: par_tek[EPID_ELECTRODE_D] = 6; break;

        case EIID_SIN : par_tek[EPID_WAVE_FORM] = 1; break;
        case EIID_RECT: par_tek[EPID_WAVE_FORM] = 2; break;  
        case EIID_TRIG: par_tek[EPID_WAVE_FORM] = 3; break;
        case EIID_TRAP: par_tek[EPID_WAVE_FORM] = 4; break;

    }
   
}


void Process_Encoder(u8 state)
{
    code const s8 shifts[] = {-1, -5, +1, +5};
  
    cur_par_value_change(cur_par_id, shifts[state]);     
   
}



void par_select(u8 new_par_id)
{
    static idata u16 old_color = YELLOW;

    if(cur_par_id == new_par_id) 
        return;
    else {
        Draw_text_change_color(text_sp[cur_par_id], old_color);  
        old_color = Draw_text_get_color(text_sp[new_par_id]);//запомнить старый цвет
        Draw_text_change_color(text_sp[new_par_id], PINK);//заменить на цвет "выбранного элемента"
        cur_par_id = new_par_id;
    }      
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


    par_tek[EPID_MODES_SUBMODES] = 0x0100;
    par[EIID_PRE_FLOW_T1]        = par_tek[EPID_PRE_FLOW_T1]    = 53;   // t gaz
    par[EIID_START_I1]           = par_tek[EPID_START_I1]       = 50;   // I begin
    par[EIID_START_T2]           = par_tek[EPID_START_T2]       = 20;   // t begin
    par[EIID_UP_T3]              = par_tek[EPID_UP_T3]          = 33;   // increase
    par[EIID_BASE_I2]            = par_tek[EPID_BASE_I2]        = 100;  // I базы
    par[EIID_BASE_T4]            = par_tek[EPID_BASE_T4]        = 2;    // t базы
    par[EIID_IMPULSE_I3]         = par_tek[EPID_IMPULSE_I3]     = 150;  // I имп
    par[EIID_IMPULSE_T5]         = par_tek[EPID_IMPULSE_T5]     = 3;    // t имп
    par[EIID_FREQ_F1]            = par_tek[EPID_FREQ_F1]        = 100;  // F
    par[EIID_BALANCE_D1]         = par_tek[EPID_BALANCE_D1]     = 6;   // balans
    par[EIID_DOWN_T6]            = par_tek[EPID_DOWN_T6]        = 11;  // Decrease
    par[EIID_END_I4]             = par_tek[EPID_END_I4]         = 60;   // I end
    par[EIID_END_T7]             = par_tek[EPID_END_T7]         = 11;   // t end
    par[EIID_POST_FLOW_T8]       = par_tek[EPID_POST_FLOW_T8]   = 3;   // t gaz
    par[EIDD_KZ_I5]              = par_tek[EPID_KZ_I5]          = 55;  // I kz
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



void make_scene(void) 
{
    u16 x = 40;
    u16 y = 550;
    u8 i;

    drawing_init();
    
    Draw_clear_screen();
    

    for(i = 0; i < ARR_SIZE(icon_sp_bottom_menu); i++) {
        icon_sp_bottom_menu[i] = draw_image(0, 0, 0xffff); // создание пула картинок без отображения картинки
    }
    
    make_bottom_menu();

    cur_par_id = EIID_BASE_I2;


    if(main_menu_bm & (u32)1 << EIID_TIG) {//tig pulse ac
        point_t p;
        LINE_WIDTH = 150;
        FONT_SIZE = 35; 
        p = add_t_preflow(make_point(x, y));
        p = dummy_line_up(p);
        p = add_t_start_i(p);
        p = add_t_up(p);
        p = add_base_i1_i2_t(p);
        p = dummy_line_down(p);
        p = add_i_t_impulse(p);
        p = dummy_line_up(p);
        p = add_t_down(p);
        p = add_end_i_t(p);
        p = dummy_line_down(p);
        p = add_t_postflow(p);


    } else if(main_menu_bm & (u32)1 << EIID_MMA) {//mma
        point_t p;
        if(main_menu_bm & (u32)1 << EIID_PULSE_ON) {
            LINE_WIDTH = 300;
            FONT_SIZE = 48;
        } else {
            LINE_WIDTH = 400;
            FONT_SIZE = 64;
        }
        
        p = dummy_line_up(make_point(40, 450));
        p = add_t_start_i(p);
        p = dummy_line_down(p);
        p = add_base_i1_i2_t(p);
        if(main_menu_bm & (u32)1 << EIID_PULSE_ON) {
            p = dummy_line_up(p);
            p = add_i_t_impulse(p);             
        }
        p = add_i_kz(p);
        p = dummy_line_down(p);
        p = dummy_line_down(p);
    }
   
   //Draw_Number(0,0, 999, 0, "A", 64, PINK);
   
    
    
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

    
        
     
    //draw_bottom_menu();
}



//To do
// -Убрать подменю выбора режима сварки режим tig spot
//  sprt добавить в выбор режима кнопки на горелке