#ifndef TEMP_H
#define TEMP_H
#include "sys.h"

typedef enum {
    EIID_TIG,
    EIID_MMA,
    EIID_MIX,
    EIID_2T,
    EIID_4T,
    EIID_4T_PLUS,
    EIID_DC_MINUS,
    EIID_DC_PLUS,
    EIID_AC,
    EIID_HF,
    EIID_LIFT,
    EIID_PULSE_OFF,
    EIID_PULSE_ON,
    EIID_D_10,
    EIID_D_16,
    EIID_D_20,
    EIID_D_40,
    EIID_RECT,
    EIID_SIN,
    EIID_TRAP,
    EIID_TRIG,
    EIID_H2O_OFF,
    EIID_H2O_ON,
}ICON_ID;

typedef struct {
	u16 x;
	u16 y;
	u16 id;
}ICON_T;


typedef struct {
    ICON_T icon;
    
}MENU_ITEM_T;

typedef struct {
    u8 size;
    MENU_ITEM_T *p_items;
    void (code *func) (void);
}MENU_T;

void draw_bottom_menu(void);

void draw_menu(u16 vp, MENU_T menu[]);
void level_back(void);


extern MENU_T *p_cur_menu;
extern u8 last_selected_menu_item;

#endif //TEMP_H