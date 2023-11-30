#ifndef TEMP_H
#define TEMP_H
#include "sys.h"
#include "drawing.h"

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
extern void (*cur_menu_fanc)(u8 item_pos);

extern xdata s16 par[];


extern void draw_cyclogramm(void);
extern void make_scene(void);
void Process_Encoder(u8 state); 
#endif //TEMP_H