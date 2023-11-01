#include "temp.h"
#include "sys.h"
#include "string.h"


#define ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))


void draw_bottom_menu(void);
void level_back(void);
void do_nothing(u8 select);
void gui_init(void);
void bottom_level_controls(u8 select);
void welding_mode_controls(u8 select);

menu_t *cur_menu;
u8 last_selected_menu_item;
u8 prev_menu_item;


enum{
    MEN_MOD,
    MEN_BTN_MOD,
    MEN_POL_MOD,
    MEN_IGN_MOD,
    MEN_PLS_MOD,
    MEN_D_MOD,
    MEN_FLOW_MOD,
};



//code icon_id tig_icons[]       = {EIID_TIG, EIID_DC_PLUS, EIID_TRIG, EIID_PULSE_OFF, EIID_D_10, EIID_2T, EIID_H2O_OFF}; //иконки раежимов на главной панели
code icon_id bottom[]=            {0, 1, 2, 3, 4, 5 ,6, 7, 8, 9, 10};
code icon_id welding_mode[] =     {EIID_TIG, EIID_MMA, EIID_MIX};
code icon_id tig_button_mod[] =   {EIID_2T, EIID_4T, EIID_4T_PLUS};
code icon_id polarity[] =         {EIID_DC_MINUS, EIID_DC_PLUS, EIID_AC};
code icon_id ignition_type[] =    {EIID_HF, EIID_LIFT};
code icon_id pulse_state[] =      {EIID_PULSE_OFF, EIID_PULSE_ON};
code icon_id electrode_d[] =      {EIID_D_10, EIID_D_16, EIID_D_20, EIID_D_40};
code icon_id flow_sensor[] =      {EIID_H2O_OFF, EIID_H2O_ON};









menu_item_t bottom_items         [ARR_SIZE(bottom)];
menu_item_t welding_mode_items   [ARR_SIZE(welding_mode)];
menu_item_t tig_button_mod_items [ARR_SIZE(tig_button_mod)];
menu_item_t polarity_items       [ARR_SIZE(polarity)];
menu_item_t ignition_type_items  [ARR_SIZE(ignition_type)];
menu_item_t pulse_state_items    [ARR_SIZE(pulse_state)];
menu_item_t electrode_d_items    [ARR_SIZE(electrode_d)];
menu_item_t flow_sensor_items    [ARR_SIZE(flow_sensor)];


idata menu_t arr_of_menus[] = {
    { ARR_SIZE(bottom_items),             0xffff,     bottom_items,            bottom_level_controls},
    { ARR_SIZE(welding_mode_items),       0xffff,     welding_mode_items,      welding_mode_controls}, 
    { ARR_SIZE(tig_button_mod_items),     0xffff,     tig_button_mod_items,    welding_mode_controls},
    { ARR_SIZE(polarity_items),           0xffff,     polarity_items,          welding_mode_controls},
    { ARR_SIZE(ignition_type_items),      0xffff,     ignition_type_items,     welding_mode_controls},
    { ARR_SIZE(pulse_state_items),        0xffff,     pulse_state_items,       welding_mode_controls},
    { ARR_SIZE(electrode_d_items),        0xffff,     electrode_d_items,       welding_mode_controls},
    { ARR_SIZE(flow_sensor_items),        0xffff,     flow_sensor_items,       welding_mode_controls},
};

void gui_init(void)
{
    u8 i, j;
    code icon_id *icon_ids[] = {
        bottom, welding_mode, tig_button_mod, polarity, ignition_type, pulse_state, electrode_d,  flow_sensor, 
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
        if(p_menu->items[i].ico == EIID_NO_ICON) { //если нет картинки недобавлять ее к отображению
            continue;
        }
        cmd[j] = p_menu->items[i].r.x0;
        j++;
        cmd[j] = p_menu->items[i].r.y0;
        j++;
        cmd[j] = p_menu->items[i].ico;
        j++;
        size++;
    }
    cmd[1] = size; //display_icon_cnt
    cmd[j] = 0xFF00;
    write_dgus_vp(vp, (u8*)&cmd, j);
}

void toggle_menu(menu_t *p_menu)
{    
  
    
    {
        u8 i;
        const u16 icon_rect_sz = 100;
        u16 start_x = 0;
        u16 start_y = 700;
        for(i = 0; i < p_menu->size; i++) {
            if((p_menu->active & (0x01 << i)) != 0) {
                p_menu->items[i].r.x0 = start_x;
                p_menu->items[i].r.y0 = start_y;
                p_menu->items[i].r.x1 = start_x + icon_rect_sz;
                p_menu->items[i].r.y1 = start_y + icon_rect_sz;
                start_x += 100;
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
    //draw_menu(0x9000, &bottom_menu);
          
}



void bottom_level_controls(u8 select)
{ 
    menu_t *p_menu;
    if(select >= p_menu->size) 
        return;
    
    p_menu = &arr_of_menus[select + 1];
    
    {
        u8 i;
        const u16 icon_rect_sz = 100;
        u16 start_x = 0;
        u16 start_y = 600;
        for(i = 0; i < p_menu->size; i++) {
            if((p_menu->active & (0x01 << i)) != 0) {
                p_menu->items[i].r.x0 = start_x;
                p_menu->items[i].r.y0 = start_y;
                p_menu->items[i].r.x1 = start_x + icon_rect_sz;
                p_menu->items[i].r.y1 = start_y + icon_rect_sz;
                start_y -= 100;
            }
        }
    }   
    cur_menu = p_menu; // make this menu as main
    draw_icons(0x9500, p_menu);        
}

void welding_mode_controls(u8 select)
{
    select = 0;
    {//clear vertical menu
        u16 tmp = 0x0000;
        write_dgus_vp(0x9500, (u8*)&tmp, 1); 
    }
    toggle_menu(&arr_of_menus[0]);
}




void draw_sub_menu(void)
{
    // prev_menu_item = last_selected_menu_item;
    // draw_menu(0x9500, array_of_menus[last_selected_menu_item]);  
}

void do_nothing(u8 select)
{ 
    // u16 tmp = 0x0000;
    // write_dgus_vp(0x9500, (u8*)&tmp, 1); //clear vertical menu
    // bottom_menu.items[prev_menu_item].ico.id = p_cur_menu->items[last_selected_menu_item].ico.id;
    // draw_bottom_menu();    
}


void level_back(void)
{
    // u16 tmp = 0x0000;
    // write_dgus_vp(0x9500, (u8*)&tmp, 1); //clear vertical menu
    // draw_bottom_menu();
}






//mma menu

























//    struct{
    //         u8 icon_library;
    //         u8 icon_write_instruction;
    //         u16 display_icon_cnt;
    //         ICON_T bottom_icons[11];
    //         u16 terminator;
    //     } temp = {
    //         36,
    //         0x07,
    //         11,//ico cnt
    //         {
    //             {0,   700, EIID_TIG},
    //             {100, 700, EIID_MMA},
    //             {200, 700, 2},
    //             {300, 700, EIID_MIX},
    //             {400, 700, EIID_2T},
    //             {500, 700, EIID_4T},
    //             {600, 700, EIID_4T_PLUS},
    //             {700, 700, EIID_TRAP},
    //             {800, 700, EIID_SIN},
    //             {900, 700, EIID_4T_PLUS},
    //             {100, 700, EIID_DC_MINUS},
    //         },   
    //         0xFF00,
    //     };   
        
    //     write_dgus_vp(0x9000, (u8*) &temp, sizeof(temp) / 2); 