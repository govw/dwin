#include <string.h>
#include "drawing.h"

#define FILLED_RECT_VP 0x1000
#define LINE_VP        0x3000

#define VAR_ICON_START_VP 0xEFF1 //начальный адрес vp для картинок +1 для каждой новой, всего 16 доступно

static code u16 SCREEN_WIDTH = 1279;
static code u16 SCREEN_HEIGHT = 799;
//max 16 элементов
static u8 all_filled_rect_cnt    = 0;
static u8 all_img_cnt            = 0;
static u8 all_line_1px_cnt       = 0;
static u8 all_data_variabl_cnt   = 0;

point_t make_point(u16 x, u16 y);
void drawing_init(void);
u8 chack_screen_bounds(u16 x, u16 y);
u16 draw_line_1px(u16 x0, u16 y0, u16 x1, u16 y1, u16 color);
void clear_line_1px(void);
u16 draw_filled_rect(u16 x0, u16 y0, u16 x1, u16 y1, u16 color);
u16 draw_line(u16 x0, u16 y0, u16 x1, u16 y1, u8 width, u16 color);
void clear_lines(void);
u16 draw_image(u16 x, u16 y, u16 image_id); 
u16 Draw_Number(u16 x, u16 y, u16 n, u8 *units, u8 font_size, u16 color);
void change_number_color(u16 sp, u16 new_color); 





point_t make_point(u16 x, u16 y) 
{
    point_t p;
    p.x = x;
    p.y = y;
    return p;
}

void drawing_init(void)
{
    all_filled_rect_cnt    = 0;
    all_img_cnt            = 0;
    all_line_1px_cnt       = 0;
    all_data_variabl_cnt   = 0;

}


u8 chack_screen_bounds(u16 x, u16 y)
{
    if(x < SCREEN_WIDTH && y < SCREEN_HEIGHT)
        return 1; // correct points
    else
        return 0; 
}

//__________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________
u16 draw_line_1px(u16 x0, u16 y0, u16 x1, u16 y1, u16 color)
{
    u16 cmd[8];
    
    if(!(chack_screen_bounds(x0, y0) && chack_screen_bounds(x1, y1)))  return;
     
    cmd[2] = color;
    cmd[3] = x0;
    cmd[4] = y0;
    cmd[5] = x1;
    cmd[6] = y1;
    cmd[7] = 0xFF00;
    all_line_1px_cnt++;
    cmd[0] = 0x000A; //draw line segment instruction
    cmd[1] = all_line_1px_cnt; //количество элементов для вывода   
        
    write_dgus_vp(LINE_VP ,(u8*) &cmd[0], 2); // запись количества элементов и команды
    {
        u16 cur_object_vp = LINE_VP + 2 + (5 * (u16)all_line_1px_cnt); 
        write_dgus_vp (cur_object_vp ,(u8*) &cmd[2], 6); // сам элемент
        return cur_object_vp;
    }
}

void clear_line_1px(void)
{
    u16 dummy = 0x0000;
    all_line_1px_cnt = 0;
    write_dgus_vp(LINE_VP ,(u8*) &dummy, 1);
}
//__________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________
u16 draw_filled_rect(u16 x0, u16 y0, u16 x1, u16 y1, u16 color)
{
    u16 cmd[8];
    if(!(chack_screen_bounds(x0, y0) && chack_screen_bounds(x1, y1))) return 0;
    
    cmd[2] = x0;
    cmd[3] = y0;
    cmd[4] = x1;
    cmd[5] = y1;
    cmd[6] = color;
    cmd[7] = 0xFF00;
    all_filled_rect_cnt++;
    cmd[0] = 0x0004;       //draw line filled rect
    cmd[1] = all_filled_rect_cnt; //количество элементов для вывода
    
    write_dgus_vp(FILLED_RECT_VP ,(u8*) &cmd[0], 2); // запись количества элементов и команды
    {
        u16 cur_object_vp = FILLED_RECT_VP + 2 + (5 * (u16)all_filled_rect_cnt);
        write_dgus_vp (cur_object_vp, (u8*) &cmd[2], 6); // сам элемент//5 - количество элементов в структуре
        return cur_object_vp;
    }
}

//__________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________
u16 draw_line(u16 x0, u16 y0, u16 x1, u16 y1, u8 width, u16 color)
{
    u8 half_widht = width / 2;
    if(!(chack_screen_bounds(x0, y0) && chack_screen_bounds(x1, y1))) return 0;
    
    if(x0 == x1 || y0 == y1)
    {
        {
            u16 tmp;
            if(x0 > x1) {
                tmp = x0;
                x0 = x1;
                x1 = tmp;
            }
            if(y0 > y1) {
                tmp = y0;
                y0 = y1;
                y1 = tmp;
            }
        }
        
        x0 -= half_widht;
        y0 -= half_widht;
        x1 += half_widht;
        y1 += half_widht;
        return draw_filled_rect(x0, y0, x1, y1, color);

    } else {
        u8 i;

        x0 += half_widht; 
        y0 -= half_widht + 2;
        x1 -= half_widht;
        y1 -= half_widht + 2;

        for(i = 0; i <= width; i++) {
            draw_line_1px(x0, y0, x1,y1, color);
            y0++;
            y1++;
        }
        return 0;
    }

    return 0;//если небыло других
}

void clear_lines(void)
{
    u16 dummy = 0x0000;
    all_filled_rect_cnt = 0;
    write_dgus_vp(FILLED_RECT_VP ,(u8*) &dummy, 1); 
}
//__________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________
//возвращает sp картинки , при необходимиости смены координат уже существующей картинки и ид самой картинки передать ее sp, не vp

u16 draw_image(u16 x, u16 y, u16 image_id) 
{             
    code u16 var_icon_sp[] = {
        0xF210, 0xF1F0, 0xF1D0, 0xF1B0, 
        0xF190, 0xF170, 0xF150, 0xF130,
        0xF110, 0xF0F0, 0xF0D0, 0xF0B0,
        0xF090, 0xF070, 0xF050, 0xF030
    };
    dgus_variables_icon_t d;
    u8 i;               

    if(all_img_cnt > ARR_SIZE(var_icon_sp)) return 0;

    d.upper_left_point = make_point(x,y); 
    d.v_min            = 0;     
    d.v_max            = 41;     
    d.icon_min         = 0;  
    d.icon_max         = 41;  
    d.icon_lib         = 36;   //36.icl  
    d.mode             = 0x01;      
    d.layer_mode       = 0x01;
    d.icon_gamma       = 0xff;
    d.pic_gamma        = 0x00; 
    d.filter_set       = 0x01;
   
    d.vp = var_icon_sp[all_img_cnt] + sizeof(dgus_variables_icon_t); //адрес переменной id картинки
    write_dgus_vp(d.vp, (u8*)&image_id, 1); //запись id картинки
    
    {
        u16 cur_sp = var_icon_sp[all_img_cnt];
        write_dgus_vp(cur_sp, (u8*)&d, sizeof(d) / 2);
        all_img_cnt++;
        return cur_sp;
    }
}


//__________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________



//__________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________
u16 Draw_Number(u16 x, u16 y, u16 n, u8 *units, u8 font_size, u16 color)
{
    code u16 data_variables_sp[] = {
        0xF500, 0xF4D0, 0xF4A0, 0xF470,
        0xF440, 0xF410, 0xF3E0, 0xF3B0,
        0xF380, 0xF350, 0xF320, 0xF2F0,
        0xF2C0, 0xF290, 0xF260, 0xF230,
    };
    
    dgus_data_variable_display_t temp; //одна структура 30 слов //выделено с запасом
    
    if(all_data_variabl_cnt > ARR_SIZE(data_variables_sp)) return 0;
    
    temp.upper_left_point   = make_point(x, y);
    temp.color              = color; 
    temp.lib_id             = 0x00;
    temp.font_size          = font_size;
    temp.alignment          = 0x02; 
    temp.integer_digits     = 0x02;
    temp.decimal_places     = 0x01;
    temp.variable_data_type = 0x00; //int
    temp.len_unit           = strlen(units);

    strncpy(temp.string_unit, units, sizeof(temp.string_unit));
    
    {
        u16 cur_sp = data_variables_sp[all_data_variabl_cnt];
        all_data_variabl_cnt++;
        temp.vp = data_variables_sp + 1;
        
        write_dgus_vp(temp.vp, (u8*) &n, 1);
        write_dgus_vp(data_variables_sp, (u8*) &temp, sizeof(temp) / 2);
        
        return cur_sp;
    }
}

void change_number_color(u16 sp, u16 new_color) 
{
    write_dgus_vp(sp + 3, (u8*) &new_color, 1);  //смещение к адресу цвета  в структуре dgus_data_variable_display_t
}
//__________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________