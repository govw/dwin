#include "temp.h"
#include "sys.h"
#include "string.h"

#define ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))




typedef enum {
    EIID_TIG,       EIID_TIG_SPOT, EIID_MMA,
    EIID_AC,        EIID_AC_MIX,   EIID_DC_MINUS, EIID_DC_PLUS,
    EIID_2T,        EIID_4T,       EIID_4T_PLUS,
    EIID_HF,        EIID_LIFT,
    EIID_PULSE_OFF, EIID_PULSE_ON,
    EIID_D_10,      EIID_D_16,     EIID_D_20,     EIID_D_24, EIID_D_32, EIID_D_40,
    EIID_SIN,       EIID_TRIG,     EIID_RECT,     EIID_TRAP,
    EIID_H2O_ON,    EIID_H2O_OFF,
    EIID_NO_ICON,
}icon_id;

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

code const u16 ICON_RECT_SZ = 128;

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
u8 cur_active_items_id = TIG;

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

struct Flags {
    u16 mode : 3;
    u16 tact : 3;
    u16 pol : 4;
    u16 ignition: 2;
    u16 pulse : 2;
    u16 vawe : 4;
    u16 elctrode_d: 5;
    u16 flow :2;
}bit_field_startup;


icon_t cur_menu[32];
u32 cur_menu_active;
u8 cur_menu_size;


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


void (*cur_menu_fanc)(u8 item_pos);

void change_mode_control(u8 item_pos);
void dgus_draw_icons(u16 vp, icon_t *p_icon, u8 size);
void make_menu_bar(u16 vp, u8 is_vertical, u16 start_x, u16 start_y, u32 id_bm, void (*cur_menu_fanc)(u8 item_pos));







void dgus_draw_icons(u16 vp, icon_t *p_icon, u8 size)
{//classic now change 
    u8 i, j;
    u16 cmd[100];
    cmd[0] = 0x2407; //icon_write_instruction      
    cmd[1] = size; //display_icon_cnt
    j = 2;
    for(i = 0; i < size; i++) {
        cmd[j] = p_icon[i].r.x0;
        j++;
        cmd[j] = p_icon[i].r.y0;
        j++;
        cmd[j] = p_icon[i].ico;
        j++;
    }
    cmd[j] = 0xFF00;
    write_dgus_vp(vp, (u8*)&cmd, j);
}


void make_menu_bar(u16 vp, u8 is_vertical, u16 start_x, u16 start_y, u32 id_bm, void (*fanc)(u8 item_pos))
{
    u8 i; 
    s16 shift_x = 0;
    s16 shift_y = 0;

    if(is_vertical)
        shift_y = -ICON_RECT_SZ;
    else
        shift_x = ICON_RECT_SZ;

    i = 0;
    cur_menu_size = 0;
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
    dgus_draw_icons(vp, cur_menu, cur_menu_size);
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
                make_menu_bar(0x9500, 1, ICON_RECT_SZ * item_pos, 800 - 128 * 2, bm, change_mode_control);         
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
            make_menu_bar(0x9000, 0, 0, 800 - ICON_RECT_SZ, bm, bottom_level_controls); 
            break;
        }
    }
    
    {
        u16 tmp = 0;
        write_dgus_vp(0x9500, (u8*)&tmp, 1);
    }
   

   
}



void draw_bottom_menu(void)
{   
    // bit_field_startup.mode = 1;
    // bit_field_startup.tact = 1;
    // bit_field_startup.pol = 1;
    // bit_field_startup.ignition = 1;
    // bit_field_startup.pulse = 1;
    // bit_field_startup.vawe = 1;
    // bit_field_startup.elctrode_d = 1;
    // bit_field_startup.flow = 1;
    
    // //main_menu_bm = *(u32*)&bit_field_startup;

    cur_active_items_id = TIG;
    make_menu_bar(0x9000, 0, 0, 800 - ICON_RECT_SZ, main_menu_bm, bottom_level_controls);   
}


























// dgus_draw_fillded_rect(u16 vp, filled_rect_t *p_rect, u8 size) 
// {
//     u8 i, j;
//     u16 cmd[100];
//     u8 size = 2;
//     cmd[0] = 0x0004; //
//     cmd[1] = size; //количество элементов для вывода
//     j = 2;
//     for(i = 0; i < size; i++) {
//         cmd[j] = p_rect[i].x0;
//         j++;
//         cmd[j] = p_rect[i].y0;
//         j++;
//         cmd[j] = p_rect[i].x1;
//         j++;
//         cmd[j] = p_rect[i].y1;
//         j++;
//         cmd[j] = p_rect[i].color;
//         j++;
//     }
//     cmd[j] = 0xFF00;
//     write_dgus_vp(vp, (u8*)&cmd, j);
// }



point_t make_point(u16 x, u16 y) 
{
    point_t p;
    p.x = x;
    p.y = y;
    return p;
}


typedef struct {
    point_t p0;
    point_t p1;
    u16 color;
} filled_rect_t;

void draw_cyclogramm(void) 
{
    u16 start_x = 0;
    u16 start_y = 300;
    const u8 LINE_HEIGHT = 11;//px
    const u8 LINE_WIDTH = 150;//px
    const u8 LEVEL_HEIGHT = 100;//px

    //циклограмма тиг не пульс в виде параметрической линии с 
    point_t p[10];
    p[0] = make_point(0,300);
    p[1] = make_point(p[0].x + LINE_WIDTH, p[0].y); 
    p[2] = make_point(p[1].x, p[1].y - LEVEL_HEIGHT); 
    p[3] = make_point(p[2].x + LINE_WIDTH, p[2].y);
    p[4] = make_point(p[3].x + LINE_WIDTH, p[3].y - LEVEL_HEIGHT);
    
    p[5] = make_point(p[4].x + LINE_WIDTH * 2, p[4].y);

    p[6] = make_point(p[5].x + LINE_WIDTH, p[5].y + LEVEL_HEIGHT);
    p[7] = make_point(p[6].x + LINE_WIDTH, p[6].y);
    p[8] = make_point(p[7].x, p[7].y + LEVEL_HEIGHT);
    p[9] = make_point(p[7].x + LEVEL_HEIGHT, p[7].y);
    
     
    {
        u8 half_height = LEVEL_HEIGHT / 2;
        filled_rects[0].p0 = make_point(p[0].x, p[0].y - half_height);
        filled_rects[0].p1 = make_point(p[1].x, p[1].y + half_height);
        
        filled_rects[1].p0 = make_point(p[2].x - half_height, p[2].y);
        filled_rects[1].p1 = make_point(p[1].x + half_height, p[1].y);

        filled_rects[2].p0 = make_point(p[2].x, p[2].y - half_height);
        filled_rects[2].p1 = make_point(p[3].x, p[3].y + half_height);

        filled_rects[3].p0 = make_point(p[4].x, p[4].y - half_height);
        filled_rects[3].p1 = make_point(p[5].x, p[5].y + half_height);

        filled_rects[4].p0 = make_point(p[6].x, p[6].y - half_height);
        filled_rects[4].p1 = make_point(p[7].x, p[7].y + half_height);

        filled_rects[5].p0 = make_point(p[7].x - half_height, p[7].y - half_height);
        filled_rects[5].p1 = make_point(p[8].x + half_height, p[8].y + half_height);

        filled_rects[6].p0 = make_point(p[8].x - half_height, p[8].y - half_height);
        filled_rects[6].p1 = make_point(p[9].x + half_height, p[9].y + half_height);

    }
    
    
   
    
    //dgusbb_draw_fillded_rect(0x7000, rect, ARR_SIZE(rect));


    
}








