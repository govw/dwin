#include "temp.h"
#include "sys.h"
#include "string.h"


#define ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))

enum {
    MEN_BOTTOM_MOD,
    MEN_WELD_MOD,
    MEN_BTN_MOD,
    MEN_POL_MOD,
    MEN_IGN_MOD,
    MEN_PLS_MOD,
    MEN_D_MOD,
    MEN_FLOW_MOD,
};

void draw_bottom_menu(void);
void gui_init(void);
void bottom_level_controls(u8 select);
void welding_mode_controls(u8 select);

menu_t *cur_menu; //сделать в виде переменной ,а не указателем
u8 last_selected_menu_item;
u8 prev_menu_item;
code const u16 icon_rect_sz = 128;

code const icon_id bottom[]=            {EIID_TIG, EIID_2T, EIID_AC ,EIID_HF ,EIID_PULSE_OFF ,EIID_SIN ,EIID_D_10, EIID_H2O_OFF};
code const icon_id welding_mode[] =     {EIID_TIG, EIID_TIG_SPOT, EIID_MMA};
code const icon_id tig_button_mod[] =   {EIID_2T, EIID_4T, EIID_4T_PLUS};
code const icon_id polarity[] =         {EIID_AC, EIID_AC_MIX , EIID_DC_MINUS, EIID_DC_PLUS};
code const icon_id ignition_type[] =    {EIID_HF, EIID_LIFT};
code const icon_id pulse_state[] =      {EIID_PULSE_OFF, EIID_PULSE_ON};
code const icon_id wave_shape[]  =      {EIID_SIN, EIID_TRIG, EIID_RECT, EIID_TRAP}; 
code const icon_id electrode_d[] =      {EIID_D_10, EIID_D_16, EIID_D_20, EIID_D_24, EIID_D_32, EIID_D_40};
code const icon_id flow_sensor[] =      {EIID_H2O_OFF, EIID_H2O_ON};

menu_item_t bottom_items         [ARR_SIZE(bottom)];
menu_item_t welding_mode_items   [ARR_SIZE(welding_mode)];
menu_item_t tig_button_mod_items [ARR_SIZE(tig_button_mod)];
menu_item_t polarity_items       [ARR_SIZE(polarity)];
menu_item_t ignition_type_items  [ARR_SIZE(ignition_type)];
menu_item_t pulse_state_items    [ARR_SIZE(pulse_state)];
menu_item_t wave_shape_items     [ARR_SIZE(wave_shape)];
menu_item_t electrode_d_items    [ARR_SIZE(electrode_d)];
menu_item_t flow_sensor_items    [ARR_SIZE(flow_sensor)];

code menu_t arr_of_menus[] = {
    { ARR_SIZE(bottom_items),          bottom_items,            bottom_level_controls},
    { ARR_SIZE(welding_mode_items),    welding_mode_items,      welding_mode_controls}, 
    { ARR_SIZE(tig_button_mod_items),  tig_button_mod_items,    welding_mode_controls},
    { ARR_SIZE(polarity_items),        polarity_items,          welding_mode_controls},
    { ARR_SIZE(ignition_type_items),   ignition_type_items,     welding_mode_controls},
    { ARR_SIZE(pulse_state_items),     pulse_state_items,       welding_mode_controls},
    { ARR_SIZE(wave_shape_items),      wave_shape_items,        welding_mode_controls},
    { ARR_SIZE(electrode_d_items),     electrode_d_items,       welding_mode_controls},
    { ARR_SIZE(flow_sensor_items),     flow_sensor_items,       welding_mode_controls},
};



code const u32 active_items_bm_tig = 0xffffffff;
code const u32 active_items_bm_tig_spot = 0xffffffff;
code const u32 active_items_bm_mma = (((u32)1 << EIID_TIG) |
                                ((u32)1 << EIID_TIG_SPOT) |
                                ((u32)1 << EIID_MMA) |
                                ((u32)1 << EIID_AC) |
                                ((u32)1 << EIID_DC_MINUS) |
                                ((u32)1 << EIID_DC_PLUS) |
                                ((u32)1 << EIID_PULSE_OFF) |
                                ((u32)1 << EIID_PULSE_ON));
                                        
idata u32 active_items_bm = 0xffffffff; //all items active at sturt

void gui_init(void)
{
    u8 i, j;
    code icon_id *icon_ids[] = {
        bottom, welding_mode, tig_button_mod, polarity, ignition_type, pulse_state, wave_shape, electrode_d,  flow_sensor, 
    };

    for(i = 0; i < ARR_SIZE(icon_ids); i++) {
        for (j = 0; j < arr_of_menus[i].size; j++) {
            arr_of_menus[i].items[j].ico = icon_ids[i][j]; 
        }
        
    }
}

void draw_icons(u16 vp, menu_t *p_menu)
{
    u8 i, j;
    u8 size;
    u16 cmd[100];
    cmd[0] = 0x2407; //icon_write_instruction      
    
    j = 2;
    size = 0;
    for(i = 0; i < p_menu->size; i++) {
        //Подумать как измень данный участок ///////////////////////////
        if(&arr_of_menus[0] != p_menu) {
            if(arr_of_menus[0].items[last_selected_menu_item].ico == p_menu->items[i].ico)
            {
                continue;
            }
        }
        ///////////////////////////////////////////
        if((u32)(active_items_bm & (u32)((u32)0x00000001 << p_menu->items[i].ico)) != 0) {
            cmd[j] = p_menu->items[i].r.x0;
            j++;
            cmd[j] = p_menu->items[i].r.y0;
            j++;
            cmd[j] = p_menu->items[i].ico;
            j++;
            size++;
        }
    }
    cmd[1] = size; //display_icon_cnt
    cmd[j] = 0xFF00;
    write_dgus_vp(vp, (u8*)&cmd, j);
}



void toggle_menu(menu_t *p_menu)
{//отображение горизонтального меню   
    {
        u8 i;
        u16 start_x = 0;
        u16 start_y = 800-128;
        for(i = 0; i < p_menu->size; i++) {
            if(((u32)active_items_bm & (u32)((u32)0x00000001 << p_menu->items[i].ico)) != 0) {
                p_menu->items[i].r.x0 = start_x;
                p_menu->items[i].r.y0 = start_y;
                p_menu->items[i].r.x1 = start_x + icon_rect_sz;
                p_menu->items[i].r.y1 = start_y + icon_rect_sz;
                start_x += 128;
            }
            
        }
    }
    
    cur_menu = p_menu; // make this menu as main
    draw_icons(0x9000, p_menu);        
}


void draw_bottom_menu(void)
{   
    gui_init();
    toggle_menu(&arr_of_menus[0]);  
}

void bottom_level_controls(u8 select)
{ //отображение вертикального меню
    menu_t *p_menu;
    p_menu = &arr_of_menus[select + 1]; //+1 значит пропусть bottom_menu
    {
        u8 i;
        u16 start_x = (u16)last_selected_menu_item * 128;
        u16 start_y = 800-256;
        for(i = 0; i < p_menu->size; i++) {
            if(((u32)active_items_bm & (u32)((u32)0x00000001 << p_menu->items[i].ico)) != 0) {
                 //Подумать как измень данный участок ///////////////////////////
                if(arr_of_menus[0].items[select].ico == p_menu->items[i].ico)
                {
                ////////////////////////////////    
                    continue;   
                }
                p_menu->items[i].r.x0 = start_x;
                p_menu->items[i].r.y0 = start_y;
                p_menu->items[i].r.x1 = start_x + icon_rect_sz;
                p_menu->items[i].r.y1 = start_y + icon_rect_sz;
                start_y -= 128; 
            }
               
            
        } 
    cur_menu = p_menu; // make this menu as main
    draw_icons(0x9500, p_menu);        
    }
}




// реакция на нажатие кнопки в горизонтальном меню
void welding_mode_controls(u8 select) 
{
    {//убрать вертикальное меню
        u16 tmp = 0x0000;
        write_dgus_vp(0x9500, (u8*)&tmp, 1); 
    }

    switch (cur_menu->items[select].ico)
    {
        case EIID_TIG:
        {
            active_items_bm = active_items_bm_tig; 
            break;
        }    
        case EIID_MMA:
        { 
            active_items_bm = active_items_bm_mma;
            break;
        }
        case EIID_TIG_SPOT:
        {
            active_items_bm = active_items_bm_tig_spot;
            break;   
        }
        
        default:
            break;
    }
    
    arr_of_menus[0].items[prev_menu_item].ico = cur_menu->items[select].ico; //поменять иконку нижнего меню на выбранную
    toggle_menu(&arr_of_menus[0]); //отобразить горизонтальное меню
}






























