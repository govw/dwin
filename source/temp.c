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
















void draw_cyclogramm(void) 
{
    u16 start_x = 0;
    u16 start_y = 300;
    const u8 LINE_HEIGHT = 10;//px
    const u8 LINE_WIDTH = 150;//px
    struct {
        u16 x0;
        u16 y0;
        u16 x1;
        u16 y1;
        u16 color;
    }rect[10];

    rect[0].x0 = 0;
    rect[0].y0 = 400;
    rect[0].x1 = 300; 
    rect[0].y1 = 400;
    rect[0].color = 0xF800;

    rect[1].x0 = 150;
    rect[1].y0 = 250;
    rect[1].x1 = 300; 
    rect[1].y1 = 400;
    rect[1].color = 0xF800;

    {
        u8 i, j;
        u16 cmd[100];
        u8 size = 2;
        cmd[0] = 0x0004; //
        cmd[1] = size; //количество элементов для вывода
        j = 2;
        for(i = 0; i < size; i++) {
            cmd[j] = rect[i].x0;
            j++;
            cmd[j] = rect[i].y0;
            j++;
            cmd[j] = rect[i].x1;
            j++;
            cmd[j] = rect[i].y1;
            j++;
            cmd[j] = rect[i].color;
            j++;
        }
        cmd[j] = 0xFF00;
        write_dgus_vp(0x7000, (u8*)&cmd, j);
    }



    //u16 cmd_len = 0;
    // cmd[0] = 0x0004;
    // cmd[1] = 1; //количество элементов для вывода
    // cmd[2] = 0; //upper left coordinate
    // cmd[3] = 0;
    // cmd[4] = 1280; //lower right coordinate
    // cmd[5] = 800;
    // cmd[6] = 0xF800; //color
    // cmd[7] = 0xFF00; //end cmd




      
}








