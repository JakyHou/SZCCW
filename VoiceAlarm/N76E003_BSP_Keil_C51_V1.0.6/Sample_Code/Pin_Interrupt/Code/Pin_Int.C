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
#define INTP	P07
static int GFlag = 0;
static int RFlag = 0;
static int repet = 1;

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
		Timer0_Delay100us(4);
		P_SCL = 1;
		Timer0_Delay100us(4);
		cmdData >>= 1;
	}
	P_SCL = 1;
	P_SDA = 1;
}

void PinInterrupt_ISR (void) interrupt 7 // Pin interrupt server. level 7
{
	
		PIF = 0x00;        	        // clear interrupt flag
		repet = 1;
		GFlag = 6;
		RFlag = 30;
		// Timer1_Delay10ms(10);

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
	 
    P07_Input_Mode; // Set p1.7 input mode
	set_P0S_7;			// set p1.7 voltage level mode
	Enable_INT_Port0; 		// Set interrupt IO port 
	Enable_BIT7_RasingEdge_Trig; // Set pin7 Triger mode

    set_EPI;							// Enable pin interrupt
    set_EA;								// global interrupt enable bit
	clr_BODEN;
//		set_PD;
	Timer0_Delay1ms(200);
    while (1)
	{
		RFlag = 30;
		GFlag = 6;

		// PowerDown mode
		while(GFlag > 0)
		{
			if(INTP != 0)
			{
				RFlag = 30;
				break;
			}
			GLED = 1;
			Timer0_Delay1ms(200);
			GLED = 0;
			Timer0_Delay1ms(300);
			GFlag--;
		}
		if(GFlag == 0) repet = 0;
		while(repet>0)
		{
			if(INTP != 0)
			{
				Timer0_Delay1ms(1);
				if(INTP != 0)
				{
					// Timer0_Delay1ms(20);
					SendCMD(0x4);
					Timer0_Delay1ms(5);
					SendCMD(0xF2);
					while(RFlag > 0)
					{
						if(INTP == 0)
						{
							SendCMD(0xFE);
							GFlag = 6;
							break;
						}
						RLED = 1;
						Timer0_Delay1ms(500);
						RLED = 0;
						Timer0_Delay1ms(500);
						RFlag--;
					}
					SendCMD(0xFE);
					if(RFlag != 0)
					while(GFlag > 0)
					{
						if(INTP != 0) 
						{repet = 1; break;}
						GLED = 1;
						Timer0_Delay1ms(200);
						GLED = 0;
						Timer0_Delay1ms(300);
						GFlag--;
					}
					else repet = 0;
					// break;
				}
			}
		}
		set_PD;
	}
}