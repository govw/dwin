#include "encoder.h"
#include "sys.h"
#include "timer.h"





void Encoder_Init(void);

void Encoder_Init(void)
{
    //  //int0
    // EA = 0; 
    // IP0 &= 0xFE;//Clear bit0
	// IP1 &= 0xFE;//Clear bit0

	// IE0 = 0;//Clear the interrupt flag bit of external interrupt 0
	
	// IT0 = 1;//1 is set as falling edge trigger, 0 is set as low level trigger
	// EX0 = 1;//Enable external interrupt 0, the corresponding pin is P3.0
    
   
    // //int1
    // IE1 = 0;
    
    // IT1 = 1;
    // EX1 = 1;
    // EA = 1;
    
//  
//    //Interrupt Allow Register IE
//    //EA = 1;     //Open the total interrupt
//      EX0 = 1;     //Open the external interrupt of the 0
//    //Control register TCON
//      IT0 = 1;     //Set external interrupt trigger mode 
//                   //0 - low level trigger
//                   //1 - Negative jump trigger
//       
////    P0 |= (1 << 6);
////    P0 |= (1 << 7);
//    SetPinIn(0,6);    
//    SetPinIn(0,7);
//    EA = 1;	
}





