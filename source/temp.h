#ifndef TEMP_H
#define TEMP_H
#include "sys.h"
#include "drawing.h"



typedef enum { //для id картинок 
    EIID_TIG,       EIID_MMA,
    EIID_AC,        EIID_AC_MIX,   EIID_DC_MINUS, EIID_DC_PLUS, 
    EIID_2T,        EIID_4T,       EIID_4T_PLUS,  EIID_TIG_SPOT,
    EIID_HF,        EIID_LIFT,
    EIID_PULSE_OFF, EIID_PULSE_ON,
    EIID_D_10,      EIID_D_16,     EIID_D_20,     EIID_D_24, EIID_D_32, EIID_D_40,
    EIID_SIN,       EIID_TRIG,     EIID_RECT,     EIID_TRAP,
    EIID_H2O_ON,    EIID_H2O_OFF,
    
    EIID_PRE_FLOW_T1,  //Т1  время предв. продувки, с
    EIID_START_I1,     //I1  начальный ток, А
    EIID_START_T2,     //T2  время начального тока, c
    EIID_UP_T3,        //Т3  время нарастания, с
    EIID_BASE_I2,      //I2  сварочный ток (ток базы), А
    EIID_BASE_T4,      //Т4  время тока базы, с
    EIID_IMPULSE_I3,   //I3  импульсный ток, А
    EIID_IMPULSE_T5,   //Т5  время тока импульса, с
    EIID_FREQ_F1,      //F1  частота, Гц
    EIID_BALANCE_D1,   //D1  баланс *
    EIID_DOWN_T6,      //Т6  время спада, с
    EIID_END_I4,       //I4  ток окончания сварки, А
    EIID_END_T7,       //T7  время конечного тока, с
    EIID_POST_FLOW_T8, //Т8  время продувки в конце, с
    EIDD_KZ_I5,        //I5  ток короткого замыкания, %
    EIID_BASE2_I2X,    //I2х второй ток базы (только в 4Т+), А
    EIID_SIZE,
};

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
    MMA,
};


typedef struct {
    u16 x0;
    u16 y0;
    u16 x1;
    u16 y1;
}rect_t;

typedef struct {
    u8 ico;
    rect_t r;
}icon_t; 






void draw_bottom_menu(void);


extern icon_t cur_menu[];
extern u8  cur_menu_size;
extern idata u32 main_menu_bm;
extern void (*cur_menu_fanc)(u8 item_pos);

extern xdata s16 par[];


extern void draw_cyclogramm(void);
extern void make_scene(void);
void Process_Encoder(u8 state); 
#endif //TEMP_H