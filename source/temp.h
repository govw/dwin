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
    EIID_PROG_ID,
    EIID_SIZE,
};

enum PAR_ID{
    EPID_MODES_SUBMODES,//0    режимы/подрежимы
    EPID_PRE_FLOW_T1,   //1    Т1  время предв. продувки, с
    EPID_START_I1,      //2    I1  начальный ток, А
    EPID_START_T2,      //3    T2  время начального тока, c
    EPID_UP_T3,         //4    Т3  время нарастания, с
    EPID_BASE_I2,       //5    I2  сварочный ток (ток базы), А
    EPID_BASE_T4,       //6    Т4  время тока базы, с
    EPID_IMPULSE_I3,    //7    I3  импульсный ток, А
    EPID_IMPULSE_T5,    //8    Т5  время тока импульса, с
    EPID_FREQ_F1,       //9    F1  частота, Гц
    EPID_BALANCE_D1,    //10   D1  баланс *
    EPID_DOWN_T6,       //11   Т6  время спада, с
    EPID_END_I4,        //12   I4  ток окончания сварки, А
    EPID_END_T7,        //13   T7  время конечного тока, с
    EPID_POST_FLOW_T8,  //14   Т8  время продувки в конце, с
    EPID_KZ_I5,         //15   I5  ток короткого замыкания, %
    EPID_BSN,           //16   БСН - вкл/выкл
    EPID_2T_4T,         //17   2t/4t
    EPID_IGNITION_TYPE, //18   Вид поджига - контактный/бесконтактный
    EPID_FLOW_SENSOR,   //19   Опрос датчика протока - вкл/выкл
    EPID_POLARITY,      //20   Полярность DC+/DC-
    EPID_ELECTRODE_D,   //21   Диаметр электрода, мм
    EPID_WAVE_FORM,     //22   Тип огибающей
    EPID_SERVICE_PAR,   //23   Служебные параметры (битовые) 
    EPID_PLUS_PULSE,    //24   + Pulse Between 
    EPID_MINUS_PULSE,   //25   - Pulse Between
    EPDI_I_MIN_MINUS,   //26   Imin - 
    EPID_I_MIN_PLUS,    //27   Imin + 
    EPID_OSCIL_TYPE,    //28   Тип осциллятора Китай/Завод    
    EPID_KC_RATIO,      //29   Коэффициент Кс 
    EPID_I_IGN_PULSE,   //30   I0  ток импульса поджига, А
    EPID_I_CLEAN,       //31   Ток очистки, А 
    EPID_T_CLEAN,       //32   Время очистки, мс
    EPID_IMPULSE_CNT,   //33   Импульсы подпитки, шт
    EPID_BASE2_I2X,     //I2х второй ток базы (только в 4Т+), А
    EPID_IS_PLUS_4T,    //35   плюс/не плюс (для 4Т)
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
    u8 id;
    rect_t r;
}icon_t; 






void draw_bottom_menu(void);


extern icon_t cur_menu[];
extern u8  touch_rect_cnt;
extern data u32 main_menu_bm;
extern void (*cur_menu_fanc)(u8 item_pos);

extern xdata s16 par[];
extern xdata s16 par_tek[];

extern u16 Amp_text_sp; //поле для вывода тока напряжения сварки
extern u16 Volt_text_sp;//поле для вывода напраяжения во время сварки

extern data u8  Welding_state;

extern void init_par_udgu(void);
extern void draw_cyclogramm(void);
extern void make_scene(void);
void Encoder_process_code(u8 state); 
#endif //TEMP_H