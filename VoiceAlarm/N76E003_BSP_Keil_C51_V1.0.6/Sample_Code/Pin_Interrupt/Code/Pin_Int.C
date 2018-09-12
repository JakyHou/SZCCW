/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2017 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

//***********************************************************************************************************
//  Nuvoton Technoledge Corp. 
//  Website: http://www.nuvoton.com
//  E-Mail : MicroC-8bit@nuvoton.com
//  Date   : Apr/21/2017
//***********************************************************************************************************

//***********************************************************************************************************
//  File Function: N76E003 I/O Interrupt demo code
//***********************************************************************************************************

#include "N76E003.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "Common.h"
#include "Delay.h"

static int GreenFlag = 1;
int* const GreenP = &GreenFlag;

/******************************************************************************
 * FUNCTION_PURPOSE: I/O Pin interrupt Service Routine
 ******************************************************************************/

void PinInterrupt_ISR (void) interrupt 7 // Pin interrupt server. level 7
{
	if(PIF == 0x80)
	{
			PIF = 0x00;        	        // clear interrupt flag
			P15 = 1;
			P15 = 0;					// Set RED LED
			* GreenP = 1;				// set green flag
  
		
	}
}

/******************************************************************************
The main C function.  Program execution starts
here after stack initialization.
******************************************************************************/
void main (void) 
{

	P10_PushPull_Mode;
	P10 = 0;
	P15_PushPull_Mode;
	P15 = 0;

	 
    P17_Input_Mode; // Set p1.7 input mode
	set_P1S_7;			// set p1.7 voltage level mode
	Enable_INT_Port1; 		// Set interrupt IO port 
	Enable_BIT7_FallEdge_Trig; // Set pin7 Triger mode

    set_EPI;							// Enable pin interrupt
    set_EA;								// global interrupt enable bit
	
    while (1)
		{
			set_PD;
			while(* GreenP)
			{
				P10 = 1;
				Timer0_Delay1ms(3000);
				P10 = 0;
			}
		}

}


