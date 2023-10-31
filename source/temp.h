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
    EIID_NO_ICON,
}ICON_ID;


typedef struct {
    u16 x0;
    u16 y0;
    u16 x1;
    u16 y1;
}trect_t;

typedef struct {
    trect_t t_rect;
    ICON_ID icon;
}menu_item_t;

typedef struct {
    u8 size;
    u8 active;//битовое поле
    menu_item_t *p_items;
    void (code *func) (void); 
}menu_t;

void draw_bottom_menu(void);

void draw_menu(u16 vp, menu_t menu[]);
void level_back(void);


extern menu_t cur_menu;
extern u8 last_selected_menu_item;

#endif //TEMP_H