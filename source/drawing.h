#ifndef INCLUDED_DRAWING_H
#define INCLUDED_DRAWING_H
#include "sys.h"
#include <stdarg.h>

#define RED        0xF800
#define PINK       0xF81F
#define YELLOW     0xFFE0
#define CYAN       0x07FF
#define WHITE      0xFFFF
#define BLACK      0x0000
#define GREEN      0x07E0
#define DARK_GREEN 0x4DE5
#define GRAY       0x6B4D 

#define TEXT_INTERVAL_0                0x80 // без интервала 
#define TEXT_INTERVAL_1                0x00 // с интервалом 

#define TEXT_ALIGNMENT_LEFT            0X00
#define TEXT_ALIGNMENT_RIGHT           0X20
#define TEXT_ALIGNMENT_CENTER          0X40
#define TEXT_ALIGNMENT_L_TO_R          0X60

#define TEXT_ALIGNMENT_VERTICAL_UP     0X00
#define TEXT_ALIGNMENT_VERTICAL_CENTER 0X10

#define TEXT_ENC_ASCII                 0X00  
#define TEXT_ENC_GB2312                0X01
#define TEXT_ENC_GBK                   0X02
#define TEXT_ENC_BIG5                  0X03
#define TEXT_ENC_SJIS                  0X04
#define TEXT_ENC_UNICODE               0X05


static code u16 SCREEN_WIDTH = 1280;
static code u16 SCREEN_HEIGHT = 800;

typedef struct {
    u16 x;
    u16 y;
} point_t;

typedef struct {
    u16 x0;
    u16 y0;
    u16 x1;
    u16 y1;
} rect_t;


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
    u16 v_min;                //Min value. Values smaller than " V_ Min" will show no Icons.
    u16 v_max;                //Max value. Values greater than " V_ Max" will show no Icons.
    u16 icon_min;             //Icon associated to the V_Min.
    u16 icon_max;             //Icon associated to the V_Max.
    u8 icon_lib;              //Index in the FLASH memory of the icon library to use.
    u8 mode;                  //Display mode: 0x00 = transparency mode Others = background display
    u8 layer_mode;            //0x00: overlay 0x01: overlay mode 1 0x02: overlay mode 2
    u8 icon_gamma;            //Icon brightness in overlay mode 2, range 0x00-0xFF, unit 1/256.
    u8 pic_gamma;             //Background brightness in overlay mode 2, range 0 x00 - 0xFF, unit 1/256.
    u8 filter_set;            //The intensity of the background color filtering in transparency mode, range 0x01-0x3F.
} dgus_variables_icon_t;

typedef struct {
    u16     vp;            //Text pointer.
    point_t start_pos; //The starting display position, which displays the coordinates of the upper left corner of the string.
    u16     color;         //Display text color
    point_t lh;        //text box 
    point_t rl;        //text box 
    u16     text_len;      //Display the number of bytes, when it encounters 0xFFFF, 0x0000 or displays to the end of the text box, it will no longer be displayed. 
    u8      font0_id;
    u8      font1_id;
    u8      font_x_dots;
    u8      font_y_dots;
    u8      encode_mode;
    u8      hor_dis;
    u8      ver_dis;
    u8      undef;          //Write 0x00. 
} dgus_text_display_t;



extern point_t make_point                  (u16 x, u16 y);
extern void    drawing_init                (void);
extern void    Draw_clear_screen           (void);
extern u8      check_screen_bounds         (u16 x, u16 y);
extern u16     draw_line_1px               (u16 x0, u16 y0, u16 x1, u16 y1, u16 color);
extern void    clear_line_1px              (void);
extern u16     draw_filled_rect            (u16 x0, u16 y0, u16 x1, u16 y1, u16 color);
extern void    Draw_filled_rect_redraw     (u16 vp, u16 x0, u16 y0, u16 x1, u16 y1, u16 color);
extern u16     draw_line                   (u16 x0, u16 y0, u16 x1, u16 y1, u8 width, u16 color);
extern void    clear_lines                 (void);
extern u16     draw_image                  (u16 x, u16 y, u16 image_id); 
extern void    image_change_id             (u16 sp, u16 new_image_id);
extern void    image_change_pos            (u16 sp, point_t p);  
extern u16     Draw_Number                 (u16 x, u16 y, u16 n, u8 decimal_places, u8 *units, u8 font_size, u16 color);
extern void    change_number_color         (u16 sp, u16 new_color);
extern void    change_number_pos           (u16 sp, point_t p);
extern u16     read_number_color           (u16 sp); 
extern void    change_number_value         (u16 sp, u16 new_value);
extern u16     Draw_text                   (u16 x0, u16 y0, u16 x1, u16 y1, u8 font0_id, u8 font1_id, u8 font_x_dots, u8 font_y_dots, u16 encode_mode, u16 color);
extern void    Draw_text_change_color      (u16 sp, u16 new_color);
extern u16     Draw_text_get_color         (u16 sp);
extern void    Draw_text_get_pos           (u16 sp, rect_t *r);
extern void    Draw_text_num_to_text       (u16 sp, u16 n, u8* units);
extern void    Draw_text_point_num_to_text (u16 sp, u16 n, u8 point_pos, u8* units);
extern void    Draw_text_change_text       (u8* format, u16 sp, ...);

#endif //INCLUDED_DRAWING_H