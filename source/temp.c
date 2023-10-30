#include "temp.h"
#include "sys.h"
#include "string.h"


void draw_bottom_menu(void);
void level_back(void);
void do_nothing(void);


MENU_T *p_cur_menu;

u8 last_selected_menu_item;
u8 prev_menu_item;

code MENU_ITEM_T welding_mod_sel[] = {
    {0, 600, EIID_TIG},
    {0, 500, EIID_MMA},
    {0, 400, EIID_MIX},          
};

code MENU_T welding_mod_sel_menu = {sizeof(welding_mod_sel) / sizeof(welding_mod_sel[0]), welding_mod_sel, do_nothing};

code MENU_ITEM_T weld_button_mod[] = {
    {100, 600, EIID_2T},
    {100, 500, EIID_4T},
    {100, 400, EIID_4T_PLUS},          
};
code MENU_T weld_button_mod_menu = {sizeof(weld_button_mod) / sizeof(weld_button_mod[0]), weld_button_mod, do_nothing};


code MENU_ITEM_T polarity[] = {
    {200, 600, EIID_DC_MINUS},
    {200, 500, EIID_DC_PLUS},
    {200, 400, EIID_AC},          
};
code MENU_T polarity_menu = {sizeof(polarity) / sizeof(polarity[0]), polarity, do_nothing};


code MENU_ITEM_T ignition_type[] = {
    {300, 600, EIID_HF},
    {300, 500, EIID_LIFT},     
};
code MENU_T ignition_type_menu = {sizeof(ignition_type) / sizeof(ignition_type[0]), ignition_type, do_nothing};


code MENU_ITEM_T pulse_state[] = {
    {400, 600, EIID_PULSE_OFF},
    {400, 500, EIID_PULSE_ON},     
};
code MENU_T pulse_state_menu = {sizeof(pulse_state) / sizeof(pulse_state[0]), pulse_state, do_nothing};


code MENU_ITEM_T electrode_d[] = {
    {500, 600, EIID_D_10},
    {500, 500, EIID_D_16},
    {500, 400, EIID_D_20}, 
    {500, 300, EIID_D_40},      
};
code MENU_T electrode_d_menu = {sizeof(electrode_d) / sizeof(electrode_d[0]), electrode_d, do_nothing};


code MENU_ITEM_T vawe_form[] = {
    {500, 600, EIID_D_10},
    {500, 500, EIID_D_16},
    {500, 400, EIID_D_20}, 
    {500, 300, EIID_D_40},      
};
code MENU_T vawe_form_menu = {sizeof(vawe_form) / sizeof(vawe_form[0]), vawe_form, do_nothing};


code MENU_ITEM_T flow_sensor[] = {
    {700, 600, EIID_H2O_OFF},
    {700, 500, EIID_H2O_ON}, 
};
code MENU_T flow_sensor_menu = {sizeof(flow_sensor) / sizeof(flow_sensor[0]), flow_sensor, do_nothing};

void draw_sub_menu(void)
{
    MENU_T *array_of_menus[] = {
        &welding_mod_sel_menu,
        &weld_button_mod_menu,
        &polarity_menu,
        &ignition_type_menu,
        &pulse_state_menu,
        &electrode_d_menu,
        &vawe_form_menu
    };
    prev_menu_item = last_selected_menu_item;
    draw_menu(0x9500, array_of_menus[last_selected_menu_item]);  
}

idata MENU_ITEM_T bottom_menu_items[] = { //idata
    {000, 700, EIID_4T  },
    {100, 700, EIID_MMA },
    {200, 700, EIID_MMA },
    {300, 700, EIID_MIX },
    {400, 700, EIID_2T  },
    {500, 700, EIID_4T  },
    {600, 700, EIID_4T_PLUS},
};
idata MENU_T bottom_menu = {sizeof(bottom_menu_items) / sizeof(bottom_menu_items[0]), bottom_menu_items, draw_sub_menu};






void do_nothing(void)
{ 
    u16 tmp = 0x0000;
    write_dgus_vp(0x9500, (u8*)&tmp, 1); //clear vertical menu
    bottom_menu.p_items[prev_menu_item].icon.id = p_cur_menu->p_items[last_selected_menu_item].icon.id;
    draw_bottom_menu();    
}


void level_back(void)
{
    u16 tmp = 0x0000;
    write_dgus_vp(0x9500, (u8*)&tmp, 1); //clear vertical menu
    draw_bottom_menu();
}



void draw_bottom_menu(void)
{   
    draw_menu(0x9000, &bottom_menu);      
}


void draw_menu(u16 vp, MENU_T *p_menu)
{
    u8 i, j;
    u16 cmd[100];
    cmd[0] = 0x2407; //icon_write_instruction      
    cmd[1] = p_menu->size; //display_icon_cnt
    j = 2;
    for(i = 0; i < p_menu->size; i++) {
        cmd[j] = p_menu->p_items[i].icon.x;
        j++;
        cmd[j] = p_menu->p_items[i].icon.y;
        j++;
        cmd[j] = p_menu->p_items[i].icon.id;
        j++;
    }
    cmd[j] = 0xFF00;

    p_cur_menu = p_menu; // инициализация указателя на текущий обработичк кнопок и их областей
    write_dgus_vp(vp, (u8*)&cmd, j);
}









//    struct{
    //         u8 icon_library;
    //         u8 icon_write_instruction;
    //         u16 display_icon_cnt;
    //         ICON_T bottom_icons[11];
    //         u16 terminator;
    //     } temp = {
    //         36,
    //         0x07,
    //         11,//icon cnt
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