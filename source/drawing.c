#include "drawing.h"

#define FILLED_RECT_VP 0x1000
#define IMAGE_VP       0x2000
#define LINE_VP        0x3000

code u16 data_variables_sp[] = {
    0xF500, 0xF4D0, 0xF4A0, 0xF470,
    0xF440, 0xF410, 0xF3E0, 0xF3B0,
    0xF380, 0xF350, 0xF320, 0xF2F0,
    0xF2C0, 0xF290, 0xF260, 0xF230,
};

code u16 var_icon_sp[] = {
    0xF210, 0xF1F0, 0xF1D0, 0xF1B0, 
    0xF190, 0xF170, 0xF150, 0xF130,
    0xF110, 0xF0F0, 0xF0D0, 0xF0B0,
    0xF090, 0xF070, 0xF050, 0xF030
};

// code u16 var_icon_data_vp[] = {
//     0xF000, 0xEFFF, 0xEFFE, 0xEFFD,
//     0xEFFC, 0xEFFB, 0xEFFA, 0xEFF9, 
//     0xEFF8, 0xEFF7, 0xEFF6, 0xEFF5,
//     0xEFF4, 0xEFF3, 0xEFF2, 0xEFF1,
// };



static code u16 SCREEN_WIDTH = 1280;
static code u16 SCREEN_HEIGHT = 800;


static idata u8 all_line_cnt = 0;
static idata u16 cur_filled_rect_addr = (u16)(FILLED_RECT_VP + (u16)2);

static idata u8 all_img_cnt = 0;
static idata u16 cur_img_addr = (u16)(IMAGE_VP + (u16)2);

static idata u8 all_line_1px_cnt = 0;
static idata u16 cur_line_1px_addr = (u16)(LINE_VP + (u16)2);

static idata u8 all_data_variabl_cnt = 0;



point_t make_point(u16 x, u16 y);




point_t make_point(u16 x, u16 y) 
{
    point_t p;
    p.x = x;
    p.y = y;
    return p;
}



void drawing_init(void)
{
    u8 i;
    dgus_data_variable_display_t temp; //одна структура 30 слов //выделено с запасом
 
    temp.lib_id = 0x00;
    temp.font_size = 0x00;
  
    //temp.integer_digits = 0x00;
    //temp.decimal_places = 0x00;
    for(i = 0; i < ARR_SIZE(data_variables_sp); i++) {
        write_dgus_vp(data_variables_sp[i], (u8*) &temp, sizeof(temp) / 2);
    }

    //init image variables
    {
        dgus_variables_icon_t d;
        u8 i;                
        d.upper_left_point = make_point(0,0); 
        d.v_min = 0;     
        d.v_max = 41;     
        d.icon_min = 0;  
        d.icon_max = 41;  
        d.icon_lib = 36;//36.icl  
        d.mode = 0x01;      
        d.layer_mode = 0x01;
        d.icon_gamma = 0xff;
        d.pic_gamma = 0x00; 
        d.filter_set = 0x01;


        clear_images();

        for(i = 0; i < ARR_SIZE(var_icon_sp); i++) {
            write_dgus_vp(var_icon_sp[i], (u8*) &d, sizeof(d) / 2);  
        }
    }
    

}


void clear_screen(void)
{
    u16 dummy = 0x0000;
    all_line_cnt = 0;
    cur_filled_rect_addr = (u16)(FILLED_RECT_VP + (u16)2);
    write_dgus_vp(FILLED_RECT_VP ,(u8*) &dummy, 1); 

    all_line_1px_cnt = 0;
    cur_line_1px_addr = (u16)(LINE_VP + (u16)2);
    write_dgus_vp(LINE_VP ,(u8*) &dummy, 1);


    all_data_variabl_cnt = 0; //стереть счетчик элементов data_variable_cnt на экране
}

u8 chack_screen_bounds(u16 x, u16 y)
{
    if(x < SCREEN_WIDTH && y < SCREEN_HEIGHT)
        return 1; // correct points
    else
        return 0; 
}



void draw_line_1px(u16 x0, u16 y0, u16 x1, u16 y1, u16 color)
{
    u16 cmd[8];
    
    if(!(chack_screen_bounds(x0, y0) && chack_screen_bounds(x1, y1))) {
        return;
    }
    all_line_1px_cnt++;
    cmd[0] = 0x000A; //draw line segment instruction
    cmd[1] = all_line_1px_cnt; //количество элементов для вывода
    
    cmd[2] = color;
    cmd[3] = x0;
    cmd[4] = y0;
    cmd[5] = x1;
    cmd[6] = y1;
    cmd[7] = 0xFF00;


    write_dgus_vp(LINE_VP ,(u8*) &cmd[0], 2); // запись количества элементов и команды
    write_dgus_vp (cur_line_1px_addr ,(u8*) &cmd[2], 6); // сам элемент
    cur_line_1px_addr += 5;
}



void draw_filled_rect(u16 x0, u16 y0, u16 x1, u16 y1, u16 color)
{
    u16 cmd[8];
    if(!(chack_screen_bounds(x0, y0) && chack_screen_bounds(x1, y1))) {
        return;
    }
    all_line_cnt++;

    cmd[0] = 0x0004;       //draw line filled rect
    cmd[1] = all_line_cnt; //количество элементов для вывода
    
    cmd[2] = x0;
    cmd[3] = y0;
    cmd[4] = x1;
    cmd[5] = y1;
    cmd[6] = color;
    cmd[7] = 0xFF00;

    write_dgus_vp(FILLED_RECT_VP ,(u8*) &cmd[0], 2); // запись количества элементов и команды
    write_dgus_vp (cur_filled_rect_addr ,(u8*) &cmd[2], 6); // сам элемент
    cur_filled_rect_addr += 5;        
}





void draw_line(u16 x0, u16 y0, u16 x1, u16 y1, u8 width, u16 color)
{
    u8 half_widht  = width / 2;
    if(!(chack_screen_bounds(x0, y0) && chack_screen_bounds(x1, y1))) {
        return;
    }
    
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
        
        draw_filled_rect(x0, y0, x1, y1, color);

    } else {
        u8 i;

        x0 += half_widht; 
        y0 -= half_widht + 2;
        x1 -= half_widht;
        y1 -= half_widht + 2;

        for(i = 0; i <= width; i++) {
            y0++;
            y1++;
            draw_line_1px(x0, y0, x1,y1,  color);
        }
    }
    

}



void draw_image(u16 x, u16 y, u16 image_id, u16 vp) //вызывать только 1 раз, после чего писать по vp
{                   
    struct {
        u16 vp;                   
        point_t upper_left_point;
    }img;

    if(all_img_cnt > ARR_SIZE(var_icon_sp)) 
        return;

    img.vp = vp;
    img.upper_left_point = make_point(x, y);
    
    write_dgus_vp(vp, (u8*)&image_id, 1);
    write_dgus_vp(var_icon_sp[all_img_cnt], (u8*)&img, sizeof(img) / 2);
    
    all_img_cnt++;
    
    //old draw image by basic graphics too slow for use
    // u16 cmd[6];

    // struct {
    //     u8 h;
    //     u8 l;
    // }h_ico_l_ins;

    // //иконки должны быть в одной библиотеке
    // h_ico_l_ins.h = 36;//0x24 icon lib code 36.icl
    // h_ico_l_ins.l = 0x07;//icon_write_instruction  

    // all_img_cnt++;
    // cmd[0] = *((u16*)&h_ico_l_ins);            
    // cmd[1] = all_img_cnt;  //display_icon_cnt
    
    // cmd[2] = x;
    // cmd[3] = y;
    // cmd[4] = image_id;
    // cmd[5] = 0xFF00;

    // write_dgus_vp(IMAGE_VP ,(u8*) &cmd[0], 2); // запись количества элементов и команды
    // write_dgus_vp (cur_img_addr ,(u8*) &cmd[2], 4); // сам элемент
    // cur_img_addr += 3;
}




void clear_images(void)
{
    u8 i;
    u16 tmp = 0x0100;
    for(i = 0; i < ARR_SIZE(var_icon_sp); i++) {
        write_dgus_vp(VAR_ICON_START_VP + i, (u8*)tmp, 1);   
    }
    
    all_img_cnt = 0;    
    //old clear
    // u16 dummy = 0x0000;
    // all_img_cnt = 0;
    // cur_img_addr = (u16)(IMAGE_VP + (u16)2);
    // write_dgus_vp(IMAGE_VP ,(u8*) &dummy, 1);             
}



u16 draw_number(u16 x, u16 y, u16 n, u8 font_size, u16 color, u16 vp)
{
    dgus_data_variable_display_t temp; //одна структура 30 слов //выделено с запасом
    u16 ret_sp;

    if(all_data_variabl_cnt > ARR_SIZE(data_variables_sp))
        return 0;
    
    temp.upper_left_point = make_point(x, y);
    temp.vp = vp;
    temp.color = color; 
    temp.lib_id = 0x00;
    temp.font_size = font_size;
    temp.alignment = 0x02; 
    temp.integer_digits = 0x02;
    temp.decimal_places = 0x01;
    temp.variable_data_type = 0x00;
    temp.len_unit = 0x01;
    temp.string_unit[0] = 'S';
    temp.string_unit[1] = 0x00;

    write_dgus_vp(vp, (u8*) &n, 1);
    write_dgus_vp(data_variables_sp[all_data_variabl_cnt], (u8*) &temp, sizeof(temp) / 2);
    
    ret_sp = data_variables_sp[all_data_variabl_cnt];
    all_data_variabl_cnt++;

    return ret_sp;
}

void change_number_color(u16 sp, u16 new_color) 
{
    write_dgus_vp(sp + 3, (u8*) &new_color, 1);  //смещение к адресу цвета  в структуре dgus_data_variable_display_t
}