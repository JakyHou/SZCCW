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

int GFlag = 0;
int RFlag = 0;


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

void PinInit()
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
	Enable_BIT7_RasingEdge_Trig; // Set pin7 Triger mode
}

void PinInterrupt_ISR (void) interrupt 7 // Pin interrupt server. level 7
{
	
	PIF = 0x00;        	        // clear interrupt flag

}


/******************************************************************************
The main C function.  Program execution starts
here after stack initialization.
******************************************************************************/
void main (void) 
{

	PinInit();

	Timer0_Delay1ms(200);
    set_EPI;							// Enable pin interrupt	
    set_EA;								// global interrupt enable bit
	// PIF = 0x00;
	clr_BODEN;							// disable brown out detection
		
	// PIF = 0x80;

    while (1)
	{


		

		if(P17 != 0)
		{
			//		int count = 3;
			Timer0_Delay1ms(5);
			if(P17 != 0)
			{
				// SendCMD(0xFE);				// stop voice alarm
				// Timer0_Delay1ms(10);
				//SendCMD(0x4);				
				// Timer0_Delay1ms(10);
				// SendCMD(0xF2);
				GLED = 0;					// turn off green led
				RLED = 0;
				// GFlag = 0;
				RFlag = 30;
				SendCMD(0x4);	
				Timer0_Delay1ms(10);
				SendCMD(0xF2);
				while(RFlag > 0)
				{
					if(P17 != 0)
					{
						RLED = 1;
						Timer0_Delay1ms(500);
						RLED = 0;
						Timer0_Delay1ms(500);
						RFlag--;
					}
					else
					{
						SendCMD(0xFE);
						GFlag = 6;
						while(GFlag)
						{
							GLED = 1;
							Timer0_Delay1ms(200);
							GLED = 0;
							Timer0_Delay1ms(300);
							GFlag--;
						}
						RFlag = 0;
					}
				}
			}
					
		}
		else
			{
				Timer0_Delay1ms(5);
				if(P17 == 0)
				{
					GFlag = 6;
					while(GFlag)
					{
						GLED = 1;
						Timer0_Delay1ms(200);
						GLED = 0;
						Timer0_Delay1ms(300);
						GFlag--;
					}
				}
			}	
				set_PD;
		}
	

	
}