#ifndef __SSD2828_H__
#define __SSD2828_H__

//#include "si_efm8bb1_defs.h"

#define bist 0	//ssd2828 bist mode

/*同事版本*/
//#define VFP 16
//#define VBP 12
//#define VS  4

//#define HFP 16
//#define HBP 12
//#define HS 	4

/*CFG版本*/
//#define VFP 56
//#define VBP 40
//#define VS  4

//#define HFP 53
//#define HBP 30
//#define HS 	4

/*手册版本1*/
//#define VFP 15
//#define VBP 12
//#define VS  4

//#define HFP 118
//#define HBP 118
//#define HS 	16//(未给出)

/*手册版本2*/
#define VFP 20
#define VBP 20
#define VS  4

#define HFP 40
#define HBP 20
#define HS 	20

#define VACT 1280
#define HACT 800

////瞎测版本
//#define VFP 20
//#define VBP 30
//#define VS  8

//#define HFP 100
//#define HBP 100
//#define HS 33

//#define VACT 1280
//#define HACT 800

/*
typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned long   u32;
typedef char            s8;
typedef short           s16;
typedef long            s32;
*/

sbit csx = P1^0;		//p1.0
sbit sdo = P1^1;		//p1.1
sbit sdi = P1^2;		//p1.2
sbit sck = P1^3;		//p1.3
sbit sdc = P1^4;		//p1.4

extern bit ssd2828_cfg_ok;

void delay_us(u8 t);
//u8 ssd2828_init(void);
void ssd2828_init(void);
u16 ssd2828_read_mipi(u16 mipi_reg);
u16 ssd2828_readReg(u8 addr_reg);

#endif