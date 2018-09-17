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

#define GLED 	P10
#define RLED 	P15
#define P_SDA 	P00
#define P_SCL 	P01

static int GFlag = 0;


/******************************************************************************
 * FUNCTION_PURPOSE: I/O Pin interrupt Service Routine
 ******************************************************************************/

// voice alarm cmd
void SendCMD(int cmd)
{
	int i = 0;
	int cmdData = cmd;
	int eachBit;
	//begin transportation 
	P_SCL = 0;
	Timer0_Delay1ms(5); // delay 5ms
	for(i=0; i<8;i++)   // send one word
	{
		eachBit = cmdData & 0x01;
		P_SCL = 0;
		P_SDA = eachBit;
		Timer0_Delay100us(2);
		P_SCL = 1;
		Timer0_Delay100us(2);
		cmdData >>= 1;
	}
	P_SCL = 1;
	P_SDA = 1;
}

void PinInterrupt_ISR (void) interrupt 7 // Pin interrupt server. level 7
{

	if(PIF == 0x80)
	{
		PIF = 0x00;        	        // clear interrupt flag
		// SendCMD(0xFE);				// stop voice alarm
		GLED = 0;					// turn off green led
		RLED = 1;
		while(1)					// wait for steady
		{
			if(P17 == 1) break;
		}
		RLED = 0;					// turn off red led jump back
		SendCMD(0x0);					// Play voice alarm 0
		SendCMD(0xF2);					// Play voice alarm 0
		GFlag = 30;
	}
}


/******************************************************************************
The main C function.  Program execution starts
here after stack initialization.
******************************************************************************/
void main (void) 
{

	//Green LED output
	P10_PushPull_Mode;
	GLED = 0;
	//RED LED output
	P15_PushPull_Mode;
	RLED = 0;
	//clk-data bus 
	P00_PushPull_Mode; // data 
	P01_PushPull_Mode; // clk
	P_SDA = 1;
	P_SCL = 1;
	 
    P17_Input_Mode; // Set p1.7 input mode
	set_P1S_7;			// set p1.7 voltage level mode
	Enable_INT_Port1; 		// Set interrupt IO port 
	Enable_BIT7_FallEdge_Trig; // Set pin7 Triger mode

    set_EPI;							// Enable pin interrupt
    set_EA;								// global interrupt enable bit

    while (1)
	{
		
		if(GFlag > 0)
		{
				GLED = 1;
				Timer1_Delay10ms(50);
				GLED = 0;
				Timer1_Delay10ms(50);
				GFlag--;
		
		}
		else set_PD;	// PowerDown mode
		
	}
	
	
	
}