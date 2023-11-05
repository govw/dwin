#include "temp.h"
#include "sys.h"
#include "string.h"

#define ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))


// struct Flags {
//     u32 flag1 : 1;
//     u32 flag2 : 1;
//     u32 flag3 : 1;
//     u32 flag4 : 1;
// };

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
    0xffffffff, //tig
    0xffffffff, //tig spot
    ((u32)1 << EIID_TIG)       | ((u32)1 << EIID_TIG_SPOT) | ((u32)1 << EIID_MMA)     |
    ((u32)1 << EIID_AC)        | ((u32)1 << EIID_DC_MINUS) | ((u32)1 << EIID_DC_PLUS) |
    ((u32)1 << EIID_PULSE_OFF) | ((u32)1 << EIID_PULSE_ON),    //mma
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




icon_t cur_menu[32];
u32 cur_menu_active;
u8 cur_menu_size;


//те элементы которые будут отображены в горизонтальном меню
idata u32 main_menu_bm; //битовая маска горизонтального меню на нижнем экране необходима первоначальная инициализация


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


void make_menu_bar(u16 vp, u8 is_vertical, u16 start_x, u16 start_y, u32 id_bm, void (*cur_menu_fanc)(u8 item_pos))
{
    u8 i;
    s16 shift_x;
    s16 shift_y;

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

    cur_menu_fanc = cur_menu_fanc;
    dgus_draw_icons(vp, cur_menu, cur_menu_size);
}

void bottom_level_controls(u8 item_pos)
{ //вызовется после выбора элемента в горизонтальном меню
    u32 bm;
    u8 i;
    //получить индекс типа меню 
    for(i = 0; i < ARR_SIZE(menu_item_bm); i++) {
        if(menu_item_bm[i] & (u32)1 << cur_menu[item_pos].ico) {
            bm = active_items[cur_active_items_id] & menu_item_bm[i];
            bm &= ~((u32)1 << cur_menu[item_pos].ico); //убрать из отборажения элемент который уже высвечен в нижнем меню
            make_menu_bar(0x9500, 1, ICON_RECT_SZ * item_pos, 0, bm, change_mode_control);     
            break;
        }
    }
}

void change_mode_control(u8 item_pos) 
{//вызовется после выбора эелемента в вертикальном меню
    u8 i;
    for(i = 0; i < ARR_SIZE(menu_item_bm); i++) {
        if(menu_item_bm[i] & (u32)1 << cur_menu[item_pos].ico) {
            main_menu_bm &= ~menu_item_bm[i]; //очисть область связанную с этим подменю в гавном меню
            main_menu_bm |= ((u32)1 << cur_menu[item_pos].ico); //вставить текущий выбранный элемент в главное меню            
            draw_bottom_menu();
            break;
        }
    }

    
    switch (cur_menu[item_pos].ico)
    {
        case EIID_TIG:
            cur_active_items_id = TIG;
            break;
        
        case EIID_TIG_SPOT:
            cur_active_items_id = TIG_SPOT;
            break;

        case EIID_MMA:
            cur_active_items_id = MMA;
            break;

    }
}



void draw_bottom_menu(void)
{   
    make_menu_bar(0x9000, 0, 0, 800 - ICON_RECT_SZ, main_menu_bm, bottom_level_controls);   
}

























