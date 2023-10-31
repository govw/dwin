#include "temp.h"
#include "sys.h"
#include "string.h"


#define ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))


void draw_bottom_menu(void);
void level_back(void);
void do_nothing(void);

<<<<<<< HEAD
menu_t *p_cur_menu;
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
}


//code icon_id tig_icons[]       = {EIID_TIG, EIID_DC_PLUS, EIID_TRIG, EIID_PULSE_OFF, EIID_D_10, EIID_2T, EIID_H2O_OFF}; //иконки раежимов на главной панели
idata icon_id bottom[]           {EIID_TIG, EIID_TIG, EIID_TIG, EIID_TIG, EIID_TIG, EIID_TIG, EIID_TIG,}
code icon_id welding_mode[] =    {EIID_TIG, EIID_MMA, EIID_MIX};
code icon_id tig_button_mod[] =  {EIID_2T, EIID_4T, EIID_4T_PLUS};
code icon_id polarity[] =        {EIID_DC_MINUS, EIID_DC_PLUS, EIID_AC};
code icon_id ignition_type[] =   {EIID_HF, EIID_LIFT};
code icon_id pulse_state[] =     {EIID_PULSE_OFF, EIID_PULSE_ON};
code icon_id electrode_d[] =     {EIID_D_10, EIID_D_16, EIID_D_20, EIID_D_40};
code icon_id flow_sensor[] =     {EIID_H2O_OFF, EIID_H2O_ON};

idata menu_t arr_of_menus[] = {
    { ARR_SIZE(bottom),         0xff, welding_mode,  do_nothing}, 
    { ARR_SIZE(welding_mode),   0xff, welding_mode,  do_nothing}, 
    { ARR_SIZE(tig_button_mod), 0xff, welding_mode,  do_nothing};
    { ARR_SIZE(polarity),       0xff, polarity,      do_nothing};
    { ARR_SIZE(ignition_type),  0xff, ignition_type, do_nothing};
    { ARR_SIZE(pulse_state),    0xff, pulse_state,   do_nothing};
    { ARR_SIZE(electrode_d),    0xff, electrode_d,   do_nothing};
    { ARR_SIZE(flow_sensor),    0xff, flow_sensor,   do_nothing};
}

=======
menu_item_t cur_menu_items[40];
menu_t cur_menu;


u8 last_selected_menu_item;
u8 prev_menu_item;

code ICON_ID tig_icons[]       = {EIID_TIG, EIID_DC_PLUS, EIID_TRIG, EIID_PULSE_OFF, EIID_D_10, EIID_2T, EIID_H2O_OFF}; //иконки раежимов на главной панели
code ICON_ID welding_mod_sel[] = {EIID_TIG, EIID_MMA, EIID_MIX};
code ICON_ID weld_button_mod[] = {EIID_2T, EIID_4T, EIID_4T_PLUS};
code ICON_ID polarity[] =        {EIID_DC_MINUS, EIID_DC_PLUS, EIID_AC};
code ICON_ID ignition_type[] =   {EIID_HF, EIID_LIFT};
code ICON_ID pulse_state[] =     {EIID_PULSE_OFF, EIID_PULSE_ON};
code ICON_ID electrode_d[] =     {EIID_D_10, EIID_D_16, EIID_D_20, EIID_D_40};
code ICON_ID flow_sensor[] =     {EIID_H2O_OFF, EIID_H2O_ON};


idata menu_t bottom_menu = {0xff, }









>>>>>>> 19b7549690ea1a51661db2aa29284ce58c68ec8b
void draw_icons(u16 vp, menu_t *p_menu)
{
    u8 i, j;
    u16 cmd[100];
    cmd[0] = 0x2407; //icon_write_instruction      
    cmd[1] = p_menu->size; //display_icon_cnt
    j = 2;
    for(i = 0; i < p_menu->size; i++) {
<<<<<<< HEAD
        if(p_menu->items[i].ico == EIID_NO_ICON) { //если нет картинки недобавлять ее к отображению
            continue;
        }
        cmd[j] = p_menu->items[i].r.x0;
        j++;
        cmd[j] = p_menu->items[i].r.y0;
        j++;
        cmd[j] = p_menu->items[i].ico;
=======
        if(p_menu->p_items[i].icon == EIID_NO_ICON) { //если нет картинки недобавлять ее к отображению
            continue;
        }
        cmd[j] = p_menu->p_items[i].t_rect.x0;
        j++;
        cmd[j] = p_menu->p_items[i].t_rect.y0;
        j++;
        cmd[j] = p_menu->p_items[i].icon;
>>>>>>> 19b7549690ea1a51661db2aa29284ce58c68ec8b
        j++;
    }
    cmd[j] = 0xFF00;
    write_dgus_vp(vp, (u8*)&cmd, j);
}


<<<<<<< HEAD
//draw_icons(0x9500, &cur_menu);   
//   for(i = 0; i < cur_menu.size; i++) {
//         cur_menu_items[i].ico = p_icons[i];
//         cur_menu_items[i].r.x0 = start_x;
//         cur_menu_items[i].r.y0 = start_y;
//         cur_menu_items[i].r.x1 = start_x + icon_rect_sz;
//         cur_menu_items[i].r.y1 = start_y + icon_rect_sz;
//         start_y -= 100;
//     }

void toggle_menu(menu_t *p_menu)
{    
=======
void make_layer_tig(void);

void tig_layer_response_(void)
{
    
    switch (expression)
    {
    case /* constant-expression */:
        /* code */
        break;
    
    default:
        break;
    }
    
    tig_icons[prev_menu_item] =  cur_menu_items[last_selected_menu_item].icon;
    make_layer_tig(); 
}

void tig_layer_response(void)
{
    code ICON_ID *all_icons[] = {welding_mod_sel, weld_button_mod, polarity, ignition_type, pulse_state, electrode_d, flow_sensor};

    const u16 icon_rect_sz = 100;
    u16 start_x = 0;
    u16 start_y = 600;
    u8 i;
    ICON_ID *p_icons;


    if(last_selected_menu_item == 7) {
        return;
    }

    prev_menu_item = last_selected_menu_item;

    cur_menu.size = all_icons[last_selected_menu_item][0]; 
    p_icons = &all_icons[last_selected_menu_item][1];

    start_x = 100 * last_selected_menu_item;

    cur_menu.func = tig_layer_response_;
    for(i = 0; i < cur_menu.size; i++) {
        cur_menu_items[i].icon = p_icons[i];
        cur_menu_items[i].t_rect.x0 = start_x;
        cur_menu_items[i].t_rect.y0 = start_y;
        cur_menu_items[i].t_rect.x1 = start_x + icon_rect_sz;
        cur_menu_items[i].t_rect.y1 = start_y + icon_rect_sz;
        start_y -= 100;
    }

    draw_icons(0x9500, &cur_menu);   
}

void make_layer_tig(void)
{    
    u8 i;
    
    const u16 icon_rect_sz = 100;
    
    u16 start_x = 0;
    u16 start_y = 700;
    
>>>>>>> 19b7549690ea1a51661db2aa29284ce58c68ec8b
    {//clear vertical menu
        u16 tmp = 0x0000;
        write_dgus_vp(0x9500, (u8*)&tmp, 1); 
    }
<<<<<<< HEAD
    
    {
        u8 i;
        const u16 icon_rect_sz = 100;
        u16 start_x = 0;
        u16 start_y = 700;
        for(i = 0; i < p_menu->size; i++) {
            if((p_menu->active & (0x01 << i)) != 0) {
                p_menu->items[i].r.x0 = start_x
                p_menu->items[i].r.y0 = start_y
                p_menu->items[i].r.x1 = start_x + icon_rect_sz;
                p_menu->items[i].r.y1 = start_y + icon_rect_sz;
            }
            start_x += 100;
        }
    }
    
    p_cur_menu = p_menu; // make this menu as main
=======

    for(i = 0; i < cur_menu.size; i++) {
        if((bit_mask & 0x01) == 0x01) {
                cur_menu_items[i].icon = tig_icons[i];
                cur_menu_items[i].t_rect.x0 = start_x;
                cur_menu_items[i].t_rect.y0 = start_y;
                cur_menu_items[i].t_rect.x1 = start_x + icon_rect_sz;
                cur_menu_items[i].t_rect.y1 = start_y + icon_rect_sz;
        }

   
        start_x += 100;
    }

>>>>>>> 19b7549690ea1a51661db2aa29284ce58c68ec8b
    draw_icons(0x9000, &cur_menu);        
}
void draw_bottom_menu(void)
{   
<<<<<<< HEAD
    toggle_menu(&arr_of_menus[]);
    //draw_menu(0x9000, &bottom_menu);
          
=======
    //draw_menu(0x9000, &bottom_menu);
    make_layer_tig();      
>>>>>>> 19b7549690ea1a51661db2aa29284ce58c68ec8b
}







void draw_sub_menu(void)
{
    // prev_menu_item = last_selected_menu_item;
    // draw_menu(0x9500, array_of_menus[last_selected_menu_item]);  
}

// void do_nothing(void)
// { 
//     u16 tmp = 0x0000;
//     write_dgus_vp(0x9500, (u8*)&tmp, 1); //clear vertical menu
<<<<<<< HEAD
//     bottom_menu.items[prev_menu_item].ico.id = p_cur_menu->items[last_selected_menu_item].ico.id;
=======
//     bottom_menu.p_items[prev_menu_item].icon.id = p_cur_menu->p_items[last_selected_menu_item].icon.id;
>>>>>>> 19b7549690ea1a51661db2aa29284ce58c68ec8b
//     draw_bottom_menu();    
// }


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