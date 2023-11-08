#ifndef TEMP_H
#define TEMP_H
#include "sys.h"




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

typedef struct {
    u16 x;
    u16 y;
} point_t;

typedef struct {
        u16 vp;
        point_t upper_left_point;       
        u16 color;
        u8 lib_id;
        u8 font_size;
        u8 alignment;
        u8 integer_digits;
        u8 decimal_places;
        u8 variable_data_type;
        u8 len_unit;
        u8 string_unit[11]; //max 11
} dgus_data_variable_display_t;


void draw_bottom_menu(void);


extern icon_t cur_menu[];
extern u8  cur_menu_size;
extern void (*cur_menu_fanc)(u8 item_pos);

extern void draw_cyclogramm(void);
extern void place_numbers_on_cyclogramm(void);
extern point_t make_point(u16 x, u16 y); 
#endif //TEMP_H