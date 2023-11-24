#ifndef DRAWING_H
#define DRAWING_H
#include "sys.h"


#define RED    0xF800
#define PINK   0xF81F
#define YELLOW 0xFFE0
#define CYAN  0x07FF
#define WHITE 0xFFFF
#define GREEN 0x07E0


static code u16 SCREEN_WIDTH = 1279;
static code u16 SCREEN_HEIGHT = 799;

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
        u8 string_unit[5]; //max 11
} dgus_data_variable_display_t;

typedef struct {
    u16 vp;                   //Variable pointer, integer.
    point_t upper_left_point; //Upper- left coordinates of the icons to display
    u16 v_min;     //Min value. Values smaller than " V_ Min" will show no Icons.
    u16 v_max;     //Max value. Values greater than " V_ Max" will show no Icons.
    u16 icon_min;  //Icon associated to the V_Min.
    u16 icon_max;  //Icon associated to the V_Max.
    u8 icon_lib;   //Index in the FLASH memory of the icon library to use.
    u8 mode;       //Display mode: 0x00 = transparency mode Others = background display
    u8 layer_mode; //0x00: overlay 0x01: overlay mode 1 0x02: overlay mode 2
    u8 icon_gamma; //Icon brightness in overlay mode 2, range 0x00-0xFF, unit 1/256.
    u8 pic_gamma;  //Background brightness in overlay mode 2, range 0 x00 - 0xFF, unit 1/256.
    u8 filter_set; //The intensity of the background color filtering in transparency mode, range 0x01-0x3F.
} dgus_variables_icon_t;



extern point_t make_point          (u16 x, u16 y);
extern void    drawing_init        (void);
extern void    Drawing_Clear_Screen (void);
extern u8      check_screen_bounds (u16 x, u16 y);
extern u16     draw_line_1px       (u16 x0, u16 y0, u16 x1, u16 y1, u16 color);
extern void    clear_line_1px      (void);
extern u16     draw_filled_rect    (u16 x0, u16 y0, u16 x1, u16 y1, u16 color);
extern u16     draw_line           (u16 x0, u16 y0, u16 x1, u16 y1, u8 width, u16 color);
extern void    clear_lines         (void);
extern u16     draw_image          (u16 x, u16 y, u16 image_id); 
extern void    image_change_id     (u16 sp, u16 new_image_id);
extern void    image_change_pos    (u16 sp, point_t p);  
extern u16     Draw_Number         (u16 x, u16 y, u16 n, u8 decimal_places, u8 *units, u8 font_size, u16 color);
extern void    change_number_color (u16 sp, u16 new_color);
extern void    change_number_pos   (u16 sp, point_t p);
extern u16     read_number_color   (u16 sp); 
extern void    change_number_value (u16 sp, u16 new_value);



#endif //DRAWING_H