#ifndef __TIMER_H__
#define __TIMER_H__
#include "sys.h"
   
#define IR_TIME    (65536-206438400UL/12/76/1000)

extern u8 data SysTick;
extern	u16 data SysTick_RTC;

extern u8 data encoder_ticks;

void T0_Init(void);
void T1_Init(void);
void T2_Init(void);
void StartTimer(u8 ID,u16 nTime);
void KillTimer(u8 ID);
u8 GetTimeOutFlag(u8 ID);
void StopTimer(u8 ID);
void ResumeTimer(u8 ID);

void StartPWM(u8 PWMID, u16 VPWMTotal,u16 VPWMLow,u8 Port,u8 Pin );

void StopAllPWM();
void StopPWM(u8 PWMID);
void Clock(void);





#endif