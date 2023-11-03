#ifndef TEMP_H
#define TEMP_H
#include "sys.h"

typedef enum {
    EIID_TIG,
    EIID_TIG_SPOT,
    EIID_MMA,

    EIID_AC,
    EIID_AC_MIX,
    EIID_DC_MINUS,
    EIID_DC_PLUS,

    EIID_2T,
    EIID_4T,
    EIID_4T_PLUS,
    
    EIID_HF,
    EIID_LIFT,
    
    EIID_PULSE_OFF,
    EIID_PULSE_ON,

    EIID_D_10,
    EIID_D_16,
    EIID_D_20,
    EIID_D_24,
    EIID_D_32,
    EIID_D_40,

    EIID_SIN,
    EIID_TRIG,
    EIID_RECT,
    EIID_TRAP,
    
    EIID_H2O_ON,
    EIID_H2O_OFF,

    EIID_NO_ICON,

}icon_id;





typedef struct {
    u16 x0;
    u16 y0;
    u16 x1;
    u16 y1;
}trect_t;

typedef struct {
    u8 ico;
    trect_t r;
}menu_item_t; 

typedef struct {
    u8 size;    
    menu_item_t *items;
    void (code *func) (u8 select); 
}menu_t;

void draw_bottom_menu(void);

void draw_menu(u16 vp, menu_t menu[]);
void level_back(void);


extern menu_t *cur_menu;
extern u8 last_selected_menu_item;
extern u8 prev_menu_item;
extern idata u32 active_items_bm; //all items active at sturt
extern code menu_t arr_of_menus[];
#endif //TEMP_H