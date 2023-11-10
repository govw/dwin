#include "temp.h"
#include "sys.h"
#include "string.h"

#define ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))





typedef enum {
    EIID_TIG,       EIID_TIG_SPOT, EIID_MMA,
    EIID_AC,        EIID_AC_MIX,   EIID_DC_MINUS, EIID_DC_PLUS,
    EIID_2T,        EIID_4T,       EIID_4T_PLUS,
    EIID_HF,        EIID_LIFT,
    EIID_PULSE_OFF, EIID_PULSE_ON,
    EIID_D_10,      EIID_D_16,     EIID_D_20,     EIID_D_24, EIID_D_32, EIID_D_40,
    EIID_SIN,       EIID_TRIG,     EIID_RECT,     EIID_TRAP,
    EIID_H2O_ON,    EIID_H2O_OFF,
    EIID_NO_ICON,
}icon_id;

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
    TIG_SPOT,
    MMA,
};

code const u16 ICON_RECT_SZ = 128;

code const u32 active_items[] = {
    ((u32)1 << EIID_TIG)       | ((u32)1 << EIID_TIG_SPOT) | ((u32)1 << EIID_MMA)      |
    ((u32)1 << EIID_2T)        | ((u32)1 << EIID_4T)       | ((u32)1 << EIID_4T_PLUS)  |
    ((u32)1 << EIID_AC)        | ((u32)1 << EIID_AC_MIX)   | ((u32)1 << EIID_DC_MINUS) |
    ((u32)1 << EIID_HF)        | ((u32)1 << EIID_LIFT)     |
    ((u32)1 << EIID_PULSE_OFF) | ((u32)1 << EIID_PULSE_ON) |
    ((u32)1 << EIID_SIN)       | ((u32)1 << EIID_TRIG)     | ((u32)1 << EIID_RECT)     | ((u32)1 << EIID_TRAP) |
    ((u32)1 << EIID_D_10)      | ((u32)1 << EIID_D_16)     | ((u32)1 << EIID_D_20)     | ((u32)1 << EIID_D_24) | ((u32)1 << EIID_D_32) | ((u32)1 << EIID_D_40) |
    ((u32)1 << EIID_H2O_OFF)   | ((u32)1 << EIID_H2O_ON),    //tig;

    ((u32)1 << EIID_TIG)       | ((u32)1 << EIID_TIG_SPOT) | ((u32)1 << EIID_MMA)      |
    ((u32)1 << EIID_2T)        | 
    ((u32)1 << EIID_AC)        | ((u32)1 << EIID_AC_MIX)   | ((u32)1 << EIID_DC_MINUS) |
    ((u32)1 << EIID_HF)        | ((u32)1 << EIID_LIFT)     |
    ((u32)1 << EIID_PULSE_OFF) | ((u32)1 << EIID_PULSE_ON) |
    ((u32)1 << EIID_SIN)       | ((u32)1 << EIID_TRIG)     | ((u32)1 << EIID_RECT)     | ((u32)1 << EIID_TRAP) |
    ((u32)1 << EIID_D_10)      | ((u32)1 << EIID_D_16)     | ((u32)1 << EIID_D_20)     | ((u32)1 << EIID_D_24) | ((u32)1 << EIID_D_32) | ((u32)1 << EIID_D_40) |
    ((u32)1 << EIID_H2O_OFF)   | ((u32)1 << EIID_H2O_ON),     //tig /spot;

    ((u32)1 << EIID_TIG)       | ((u32)1 << EIID_TIG_SPOT) | ((u32)1 << EIID_MMA)     |
    ((u32)1 << EIID_AC)        | ((u32)1 << EIID_DC_MINUS) | ((u32)1 << EIID_DC_PLUS) |
    ((u32)1 << EIID_PULSE_OFF) | ((u32)1 << EIID_PULSE_ON),     //mma
};
u8 cur_active_items_id = TIG;

code const u32 menu_item_bm[] = { 
    ((u32)1 << EIID_TIG)       | ((u32)1 << EIID_TIG_SPOT) | ((u32)1 << EIID_MMA),      
    ((u32)1 << EIID_2T)        | ((u32)1 << EIID_4T)       | ((u32)1 << EIID_4T_PLUS),    
    ((u32)1 << EIID_AC)        | ((u32)1 << EIID_AC_MIX)   | ((u32)1 << EIID_DC_MINUS) | ((u32)1 << EIID_DC_PLUS),                                             
    ((u32)1 << EIID_HF)        | ((u32)1 << EIID_LIFT),
    ((u32)1 << EIID_PULSE_OFF) | ((u32)1 << EIID_PULSE_ON),
    ((u32)1 << EIID_SIN)       | ((u32)1 << EIID_TRIG)     | ((u32)1 << EIID_RECT)     | ((u32)1 << EIID_TRAP),                                                
    ((u32)1 << EIID_D_10)      | ((u32)1 << EIID_D_16)     | ((u32)1 << EIID_D_20)     | ((u32)1 << EIID_D_24) | ((u32)1 << EIID_D_32) | ((u32)1 << EIID_D_40),
    ((u32)1 << EIID_H2O_OFF)   | ((u32)1 << EIID_H2O_ON),   
}; 

struct Flags {
    u16 mode : 3;
    u16 tact : 3;
    u16 pol : 4;
    u16 ignition: 2;
    u16 pulse : 2;
    u16 vawe : 4;
    u16 elctrode_d: 5;
    u16 flow :2;
}bit_field_startup;


icon_t cur_menu[32];
u32 cur_menu_active;
u8 cur_menu_size;


//те элементы которые будут отображены в горизонтальном меню
idata u32 main_menu_bm = (((u32)1 << EIID_TIG)       |
                         ((u32)1 << EIID_2T)        |
                         ((u32)1 << EIID_AC)        |
                         ((u32)1 << EIID_HF)        |
                         ((u32)1 << EIID_PULSE_OFF) |
                         ((u32)1 << EIID_SIN)       |
                         ((u32)1 << EIID_D_10)      |
                        ((u32)1 << EIID_H2O_OFF)      
                               
);
//битовая маска горизонтального меню на нижнем экране необходима первоначальная инициализация


void (*cur_menu_fanc)(u8 item_pos);

void change_mode_control(u8 item_pos);
void dgus_draw_icons(u16 vp, icon_t *p_icon, u8 size);
void make_menu_bar(u16 vp, u8 is_vertical, u16 start_x, u16 start_y, u32 id_bm, void (*cur_menu_fanc)(u8 item_pos));







void dgus_draw_icons(u16 vp, icon_t *p_icon, u8 size)
{//classic now change 
    u8 i, j;
    u16 cmd[100];
    cmd[0] = 0x2407; //icon_write_instruction      
    cmd[1] = size; //display_icon_cnt
    j = 2;
    for(i = 0; i < size; i++) {
        cmd[j] = p_icon[i].r.x0;
        j++;
        cmd[j] = p_icon[i].r.y0;
        j++;
        cmd[j] = p_icon[i].ico;
        j++;
    }
    cmd[j] = 0xFF00;
    write_dgus_vp(vp, (u8*)&cmd, j);
}


void make_menu_bar(u16 vp, u8 is_vertical, u16 start_x, u16 start_y, u32 id_bm, void (*fanc)(u8 item_pos))
{
    u8 i; 
    s16 shift_x = 0;
    s16 shift_y = 0;

    if(is_vertical)
        shift_y = -ICON_RECT_SZ;
    else
        shift_x = ICON_RECT_SZ;

    i = 0;
    cur_menu_size = 0;
    while (id_bm != 0) {
        if((u32)id_bm & (u32)1) {
            cur_menu[cur_menu_size].ico = i;
            cur_menu[cur_menu_size].r.x0 = start_x;
            cur_menu[cur_menu_size].r.y0 = start_y;
            cur_menu[cur_menu_size].r.x1 = start_x + ICON_RECT_SZ;
            cur_menu[cur_menu_size].r.y1 = start_y + ICON_RECT_SZ;
            start_x += shift_x;
            start_y += shift_y;
            cur_menu_size++;        
        }
        id_bm >>= 1;
        i++;
    }

    cur_menu_fanc = fanc;
    dgus_draw_icons(vp, cur_menu, cur_menu_size);
}

void bottom_level_controls(u8 item_pos)
{ //вызовется после выбора элемента в горизонтальном меню
    u32 bm;
    u8 i;
    //получить индекс типа меню 
    for(i = 0; i < ARR_SIZE(menu_item_bm); i++) {
        if((u32)menu_item_bm[i] & (u32)((u32)1 << cur_menu[item_pos].ico)) { //поиск меню к которму относится данный элемент
            bm = active_items[cur_active_items_id] & menu_item_bm[i];//убрать из отображения эелементы которые не доступны в данном режиме 
            bm &= ~((u32)1 << cur_menu[item_pos].ico); //убрать из отборажения элемент который уже высвечен в нижнем меню
            if(bm != 0 )//если нечего выводить не рисовать горизонтальное меню
                make_menu_bar(0x9500, 1, ICON_RECT_SZ * item_pos, 800 - 128 * 2, bm, change_mode_control);         
            break;
        }
    }
}

void change_mode_control(u8 item_pos) 
{//вызовется после выбора эелемента в вертикальном меню
    u8 i;
    u32 bm;


    for(i = 0; i < ARR_SIZE(menu_item_bm); i++) {
        if(menu_item_bm[i] & (u32)1 << cur_menu[item_pos].ico) {
            main_menu_bm &= (u32)~menu_item_bm[i]; //очисть область связанную с этим подменю в гавном меню
            switch (cur_menu[item_pos].ico)
            {
                case EIID_TIG:
                    cur_active_items_id = TIG;
                    break;
                
                case EIID_TIG_SPOT:
                    cur_active_items_id = EIID_TIG_SPOT;
                    break;

                case EIID_MMA:
                    cur_active_items_id = MMA;
                    break;
            }
            main_menu_bm |= ((u32)1 << cur_menu[item_pos].ico); //вставить текущий выбранный элемент в главное меню
            
            bm = active_items[cur_active_items_id];
            bm &= (u32)main_menu_bm;
            make_menu_bar(0x9000, 0, 0, 800 - ICON_RECT_SZ, bm, bottom_level_controls); 
            break;
        }
    }
    
    {
        u16 tmp = 0;
        write_dgus_vp(0x9500, (u8*)&tmp, 1);
    }
   

   
}



void draw_bottom_menu(void)
{   
    // bit_field_startup.mode = 1;
    // bit_field_startup.tact = 1;
    // bit_field_startup.pol = 1;
    // bit_field_startup.ignition = 1;
    // bit_field_startup.pulse = 1;
    // bit_field_startup.vawe = 1;
    // bit_field_startup.elctrode_d = 1;
    // bit_field_startup.flow = 1;
    
    // //main_menu_bm = *(u32*)&bit_field_startup;

    cur_active_items_id = TIG;
    make_menu_bar(0x9000, 0, 0, 800 - ICON_RECT_SZ, main_menu_bm, bottom_level_controls);   
}






























point_t make_point(u16 x, u16 y) 
{
    point_t p;
    p.x = x;
    p.y = y;
    return p;
}


typedef struct {
    point_t p0;
    point_t p1;
    u16 color;
} filled_rect_t;

typedef struct {
    point_t p0;
    point_t p1;
    u16 color;
} line_t;

dgus_draw_fillded_rect(u16 vp, filled_rect_t *p_rect, u8 size) 
{
    u8 i, j;
    u16 cmd[120];
    cmd[0] = 0x0004; //
    cmd[1] = size; //количество элементов для вывода
    j = 2;
    for(i = 0; i < size; i++) {
        cmd[j] = p_rect[i].p0.x;
        j++;
        cmd[j] = p_rect[i].p0.y;
        j++;
        cmd[j] = p_rect[i].p1.x;
        j++;
        cmd[j] = p_rect[i].p1.y;
        j++;
        cmd[j] = p_rect[i].color;
        j++;
    }
    cmd[j] = 0xFF00;
    write_dgus_vp(vp, (u8*)&cmd, j);
}

dgus_draw_line_segment(u16 vp, line_t *p_line, u8 size) 
{
    u8 i, j;
    u16 cmd[120];
    cmd[0] = 0x000A; //draw line segment instruction
    cmd[1] = size; //количество элементов для вывода
    j = 2;
    for(i = 0; i < size; i++) {
        cmd[j] = p_line[i].color;
        j++;
        cmd[j] = p_line[i].p0.x;
        j++;
        cmd[j] = p_line[i].p0.y;
        j++;
        cmd[j] = p_line[i].p1.x;
        j++;
        cmd[j] = p_line[i].p1.y;
        j++;
    }
    cmd[j] = 0xFF00;
    write_dgus_vp(vp, (u8*)&cmd, j);
}


void draw_cyclogramm(void) 
{
    u16 start_x = 0;
    u16 start_y = 500;
    const u8 LINE_HIGHT = 11;//px
    const u8 LINE_WIDTH = 150;//px
    const u8 LEVEL_HEIGHT = 120;//px
    const u16 RED = 0xF800;
    filled_rect_t filled_rects[7];
    line_t  l[11 * 2];
    point_t disp_var_upper_left[10];
    dgus_data_variable_display_t temp; //ода структура 30 слов //выделено с запасом

    //циклограмма тиг не пульс в виде параметрической линии с 
    point_t p[10];
    

    p[0] = make_point(40,500);
    p[1] = make_point(p[0].x + LINE_WIDTH, p[0].y); 
    p[2] = make_point(p[1].x, p[1].y - LEVEL_HEIGHT); 
    p[3] = make_point(p[2].x + LINE_WIDTH, p[2].y);
    p[4] = make_point(p[3].x + LINE_WIDTH, p[3].y - LEVEL_HEIGHT);
    
    p[5] = make_point(p[4].x + LINE_WIDTH * 2, p[4].y);

    p[6] = make_point(p[5].x + LINE_WIDTH, p[5].y + LEVEL_HEIGHT);
    p[7] = make_point(p[6].x + LINE_WIDTH, p[6].y);
    p[8] = make_point(p[7].x, p[7].y + LEVEL_HEIGHT);
    p[9] = make_point(p[8].x + LINE_WIDTH, p[8].y);
    

     
    temp.vp = 0x3000;
    temp.color = 0x07FF;
    temp.lib_id = 0x00;
    temp.font_size = 0x20;
    temp.alignment = 0x02; //center alignment
    temp.integer_digits = 0x03;
    temp.decimal_places = 0x00;
    temp.variable_data_type = 0x00;
    temp.len_unit = 0x02;
    temp.string_unit[0] = 'm';
    temp.string_unit[1] = 's';
    temp.string_unit[2] = 0x00;
    
     //вермя продвки
    temp.upper_left_point = make_point(p[0].x, p[1].y);
    write_dgus_vp(0x1000, (u8*)&temp, sizeof(temp) / 2);  
    // //начальный ток
    temp.upper_left_point = make_point(p[2].x, p[3].y - 70);
    write_dgus_vp(0x1030, (u8*)&temp, sizeof(temp) / 2);
    // //вермя начального тока
    temp.upper_left_point = make_point(p[2].x, p[3].y);
    write_dgus_vp(0x1060, (u8*)&temp, sizeof(temp) / 2);
    //время наростания
    temp.upper_left_point = make_point(p[3].x, p[3].y);
    write_dgus_vp(0x1090, (u8*)&temp, sizeof(temp) / 2);
    // //ток базы
    temp.upper_left_point = make_point(p[4].x, p[5].y - 70);
    write_dgus_vp(0x1120, (u8*)&temp, sizeof(temp) / 2);
    // //время спада
    temp.upper_left_point = make_point(p[5].x, p[6].y);
    write_dgus_vp(0x1150, (u8*)&temp, sizeof(temp) / 2);
    // //конечный ток
    temp.upper_left_point = make_point(p[6].x, p[6].y - 70);
    write_dgus_vp(0x1180, (u8*)&temp, sizeof(temp) / 2);
    // //время конечного тока
    temp.upper_left_point = make_point(p[6].x, p[6].y);
    write_dgus_vp(0x1210, (u8*)&temp, sizeof(temp) / 2);
    // //пост продувка
    temp.upper_left_point = make_point(p[8].x, p[9].y);
    write_dgus_vp(0x1240, (u8*)&temp, sizeof(temp) / 2);




    {
        u8 i;
        u8 half_hight = LINE_HIGHT / 2;
        filled_rects[0].p0 = make_point(p[0].x - half_hight, p[0].y - half_hight); // горизонтальная лини //время продувки
        filled_rects[0].p1 = make_point(p[1].x + half_hight, p[1].y + half_hight);
                         
        
        filled_rects[1].p0 = make_point(p[2].x - half_hight, p[2].y - half_hight);
        filled_rects[1].p1 = make_point(p[1].x + half_hight, p[1].y + half_hight);

        filled_rects[2].p0 = make_point(p[2].x - half_hight, p[2].y - half_hight);
        filled_rects[2].p1 = make_point(p[3].x + half_hight, p[3].y + half_hight);


        filled_rects[3].p0 = make_point(p[4].x - half_hight, p[4].y - half_hight);
        filled_rects[3].p1 = make_point(p[5].x + half_hight, p[5].y + half_hight);


        {//line down
            point_t a,b;
            a = make_point(filled_rects[2].p1.x, filled_rects[2].p1.y - LINE_HIGHT + 1);
            b = filled_rects[3].p0;
            for(i = 0; i < ARR_SIZE(l) / 2; i++) { //first half fill
                l[i].p0 = a;
                l[i].p1 = b;
                l[i].color = 0x07FF;
                a.y++;
                b.y++;   
            }
        }

        filled_rects[4].p0 = make_point(p[6].x - half_hight, p[6].y - half_hight);
        filled_rects[4].p1 = make_point(p[7].x + half_hight, p[7].y + half_hight);

        {//line down
            point_t a,b;
            a = make_point(filled_rects[3].p1.x, filled_rects[3].p1.y - LINE_HIGHT + 1);
            b = filled_rects[4].p0;
            for(i = ARR_SIZE(l) / 2; i < ARR_SIZE(l); i++) { //first half fill
                l[i].p0 = a;
                l[i].p1 = b;
                l[i].color = 0x07FF;
                a.y++;
                b.y++;   
            }
        }

        filled_rects[5].p0 = make_point(p[7].x - half_hight, p[7].y - half_hight);
        filled_rects[5].p1 = make_point(p[8].x + half_hight, p[8].y + half_hight);

        filled_rects[6].p0 = make_point(p[8].x - half_hight, p[8].y - half_hight);
        filled_rects[6].p1 = make_point(p[9].x + half_hight, p[9].y + half_hight);


        for(i = 0; i < ARR_SIZE(filled_rects); i++) { //задание цвета всем прямоугольникам
            filled_rects[i].color = 0xffff;       
        }
        filled_rects[3].color = RED;
    }
    



    dgus_draw_fillded_rect(0x7000, filled_rects, ARR_SIZE(filled_rects));
    dgus_draw_line_segment(0x7500, l, ARR_SIZE(l)); 
}






void place_numbers_on_cyclogramm(void) 
{
    //30 00 00 f1 00 9F f8 00 00 20 00 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
    
    
    //memset(temp.string_unit, sizeof(temp.string_unit), 0x00);
    //write_dgus_vp(0x1000, (u8*)&temp, sizeof(temp) / 2);
}



