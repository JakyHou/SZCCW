/*******************************************************************************
** Copyright (c) 2011, Development Department Of Qingdao Everbright Instrument Co., Ltd
** All rights reserved.

** File name:  ET-PSA_2011_06_05
** Summary	: ET-PSA人体静电释放报警器的控制程序

** Current 
   Edition	:Version 1.0
** Mender	:Major
** Mend Date	:2011.06.05
** Description	:
/		 MSP430G2x13/G2x53
//            -----------------
//       /|\ |              XIN|-
//        |  |                 |
//        ---|RST          XOUT|-
//           |                 |
//           |P1.0/CA0     P1.5|-->PWM --> 蜂鸣器
//           |P1.1/CA1         |
//           |                 |
//           |                 |			
//           |Vss
//
*******************************************************************************/

/**************************************************************************
使用到的端口说明：
P1.0-    TOUCH PAD1
P1.1-    TOUCH PAD1
P1.5-    PWM输出，接蜂鸣器
**************************************************************************/

/******************************************************************************/
#include  <msp430g2553.h>
#include "TK.H"
#include "GS.H"
#include "config.h"


/******************************************************************************/

#define SPK        BIT0


uchar S_DATA,B_DATA;
unsigned int uiAlarmCounter = 0;
uchar Red_flag=0,Green_flag=0,Yellow_flag = 0,k = 0,End_flag = 0;
void delay_1us(void)
{
	asm("nop");
}
void delay_nus(unsigned int n)
{
	unsigned int i;
	for(i=0;i<n;i++)
		delay_1us();
}

void delay_1ms(void)
{
	unsigned int i;
	for(i=0;i<1140;i++)
		;
}
void delay_nms(unsigned int n)
{
	unsigned int i;
	for(i=0;i<n;i++)
		delay_1ms();	
}

/******************************************************************************/
void vGreenLed(uchar ucgreenflag)
{  
    if(ucgreenflag)
    {
    P3DIR |= BIT6;
	P3OUT |= BIT6;

    }
    
    else
    {
    P3DIR |= BIT6;
	P3OUT &= ~BIT6;
    }

}


/******************************************************************************/
void vYellowLed(uchar ucyellowflag)
{
    if(ucyellowflag)
    {
	P3DIR |= BIT6;
	P3OUT |= BIT6;
    
    P3DIR |= BIT7;
	P3OUT |= BIT7;        
    }
    
    else
    {
	P3DIR |= BIT6;
	P3OUT &= ~BIT6;
    
    P3DIR |= BIT7;
	P3OUT &= ~BIT7;       
    }

}


/******************************************************************************/
void vRedLed(uchar ucyellowflag)
{
    if(ucyellowflag)
    {
	P3DIR |= BIT7;
	P3OUT |= BIT7;    
	k++;
    }
    else
    {
	P3DIR |= BIT7;
	P3OUT &= ~BIT7;       
    }

}



/******************************************************************
** Function name	：void sleep( void )
** Description		:  休眠程序
******************************************************************/

void vSleep( void )
{  
	
         P1DIR |= 0x6f;
	     P1OUT &= 0x90;
	
         //P1DIR = 0xff;
	     //P1OUT = 0x00;	
         P2DIR = 0xff;
	     P2OUT = 0x01;
         P3DIR = 0xff;
	     P3OUT = 0x02;
      
          BCSCTL1 |= DIVA_3; 
		 //BCSCTL1 = (BCSCTL1 & 0x0CF) + DIVA_3;
	      WDTCTL = WDT_ADLY_16;             // Set Watchdog Timer interval to 16ms
	      IE1 |= WDTIE;  
	      //_EINT();                         // Enable WDT interrupt
	     _BIS_SR(LPM3_bits+ GIE);             // Enter LPM3 w/ interrupt
               
          BCSCTL1 |= DIVA_3 ; 
		 //BCSCTL1 = (BCSCTL1 & 0x0CF) + DIVA_3;
	     WDTCTL = WDT_ADLY_16;             // Set Watchdog Timer interval to 16ms
	     IE1 |= WDTIE;  
	      //_EINT();                         // Enable WDT interrupt
	     _BIS_SR(LPM3_bits+ GIE);             // Enter LPM3 w/ interrupt
         
        BCSCTL1 |= DIVA_2 ; 
		 //BCSCTL1 = (BCSCTL1 & 0x0CF) + DIVA_2;
	     WDTCTL = WDT_ADLY_16;             // Set Watchdog Timer interval to 16ms
	     IE1 |= WDTIE;  
	      //_EINT();                         // Enable WDT interrupt
	     _BIS_SR(LPM3_bits+ GIE);             // Enter LPM3 w/ interrupt
	      //_NOP();  
          // _DINT(); 
    
}

/*
void vSleep( void )
{           
         P1DIR = 0xff;
	     P1OUT = 0x00;
         P2DIR = 0xff;
	     P2OUT = 0x00;
         P3DIR = 0xff;
	     P3OUT = 0x00;
         
        //BCSCTL1 = CALBC1_8MHZ; //校准DCO为8MHZ
        //DCOCTL = CALDCO_8MHZ;  
         BCSCTL3 |= XCAP_3  ;   //ACLK 选择内部晶振      
         BCSCTL1 |= DIVA_0;
         
         TA0CTL  = TASSEL_1 + ID_2; //ACLK, upmode+ ID_3
         TA0CCR0 = 600;  //      采用外部晶振32.768KHZ时 CCR0 = 1300  采用内部12K,20 
         TA0CTL  |= MC0 + TACLR;
         TA0CCTL0  |= CCIE;  //  比较中断使能                                   
        _EINT(); // Enable WDT interrupt   
        WDTCTL = WDT_ARST_1000;          // Set Watchdog Timer interval to 250ms  
	    _BIS_SR(LPM3_bits+GIE);             // Enter LPM3 
        // _BIS_SR( CPUOFF + SCG0 + SCG1 );       
	    _NOP(); 
        TA0CTL &= ~MC0;  
        TA0CCTL0  &= ~CCIE;
        BCSCTL3 &= ~XCAP_3  ;  
        BCSCTL1 &= ~DIVA_0;
        _DINT(); 
      //  BCSCTL1 = CALBC1_8MHZ; //校准DCO为8MHZ
       // DCOCTL = CALDCO_8MHZ;  
              
}
*/
/******************************************************************
** Function name	：void vAlarmOn ( void )
** Description		: 蜂鸣器打开,
******************************************************************/
void vAlarm ( uchar  SB_DATA)
{    
	uchar j;
	P3DIR |= BIT1;
	P3OUT |= BIT1;//时钟
	
	P2DIR |= BIT0;
	P2OUT |= BIT0;//数据
	
	S_DATA = SB_DATA;
	P3DIR |= BIT1;
	P3OUT &= ~BIT1;//时钟
	
	//T16B0_delay_ms(5);
	delay_nms(5);
	B_DATA = S_DATA&0X01;
	for(j=0;j<8;j++)
	{
		
		P3DIR |= BIT1;
		P3OUT &= ~BIT1;//时钟
		P2DIR |= BIT0;
		P2OUT = (B_DATA<<0);//数据
		
		//T16B0_delay_us(400);
		delay_nus(400);
		P3DIR |= BIT1;
		P3OUT |= BIT1;//时钟
		//T16B0_delay_us(400);
		delay_nus(400);
		S_DATA = S_DATA>>1;
		B_DATA = S_DATA&0X01;
		
	}
	P3DIR |= BIT1;
	P3OUT |= BIT1;//时钟
	
	P2DIR |= BIT0;
	P2OUT |= BIT0;//数据
           
}

/******************************************************************
** Function name	：TIMERA0_VECTOR(void)
** Description		:  timer_a0中断服务程序
******************************************************************/
#pragma vector = TIMER1_A0_VECTOR

__interrupt void TIMERA1ISR( void )
{       
    
        WDTCTL = WDT_ARST_1000; 
        _BIC_SR_IRQ(LPM3_bits);
        _NOP(); 
 
}

/******************************************************************
** Function name	：TIMERA0_VECTOR(void)
** Description		:  timer_a0中断服务程序
******************************************************************/
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMERA0ISR( void )
{  
        WDTCTL = WDT_ARST_1000; 
        _BIC_SR_IRQ(LPM3_bits);
        _NOP(); 
}

/******************************************************************
** Function name	：vWdtIsr(void)
** Description		:  WDT看门狗定时器中断服务程序
******************************************************************/
#pragma vector = WDT_VECTOR
__interrupt void vWdtIsr(void)
{   
   _BIC_SR_IRQ(LPM3_bits);
}
/*****************************************************************
** Function name	：main( void )
** Description		: 控制主函数
******************************************************************/

void main (void)
{
	uint i = 0;
	uint uiNormalTimer = 0;
	uchar ucAlarmTimer = 0;
	uint ucGndAlarmTimer = 0;
	uchar ucAlarmFlag = 0;
	uchar ucGndAlarmFlag = 0;
	uchar ucNormalFlag = 0;
	
	uchar ucAlarmCounterFlag = 0;       
	uchar ucNormalCounterFlag = 0; 
	uchar ucGndAlarmCounterFlag = 0; 
	uchar TKResetCnt = 0;	

	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	DCOCTL = CALDCO_8MHZ;
	BCSCTL1 = CALBC1_8MHZ;
	vTkInit(); 
   //vAlarm (0x03);    
  	while (1)                                 // Mainloop
  	{  

		//vAlarm ( 0x03);
		//delay_nms(6000);
		if(vCheckStatic())   //检测接地线
		{  
			DCOCTL = CALDCO_8MHZ;
			BCSCTL1 = CALBC1_8MHZ;  
			BCSCTL1 |= DIVA_2 ; 
			WDTCTL = WDT_ADLY_16;//WDT_ADLY_;16////;          
			IE1 |= WDTIE; 
			_BIS_SR(LPM0_bits+GIE);    // Wait for WDT interrup

			ucGndAlarmTimer++;

			if(ucGndAlarmFlag == 0)
			{
				ucGndAlarmCounterFlag ^= 0x01 ;  
				  
				vYellowLed(ucGndAlarmCounterFlag);
				//if(Yellow_flag == 0)
					//{
				i++;
				if(i>80)
				{
						vAlarm (0x04);
						Red_flag=0;
						Green_flag=0;
						//End_flag=0;
						i = 0;
						k=0;
				}
						//Yellow_flag = 1;
					//}
			}
			
			if(GNDALARM-1 == ucGndAlarmTimer)
			{
				vCheckTouchPad();
				vTkBaseReset(); //改变基值
			}
			if(ucGndAlarmTimer >= GNDALARM)
			{
				Red_flag=0;
				Green_flag=0;
				Yellow_flag = 0;
				if(vCheckTouchPad())
				{
					if (TKResetCnt)
					{
						ucGndAlarmFlag = 0;
						ucGndAlarmTimer = 0;
						ucGndAlarmCounterFlag = 0;
						TKResetCnt = 0;
					}
					else
					{
						vTkBaseReset(); //改变基值
						//vTkInit();
						//vCheckTouchPad();
						TKResetCnt = 1;
					}					
				}
				else
				{
					TKResetCnt = 0;
					
					ucGndAlarmFlag = 1;
					ucGndAlarmTimer = GNDALARM;

					vRedLed(0);  
					vGreenLed(0); 
					//vAlarm(0x00);
					vSleep();        
				}                 
			}      

		}
     
		else 
    	{  
			TKResetCnt = 0;
			if(vCheckTouchPad())//
			{
				 BCSCTL1 |= DIVA_3; 
				 WDTCTL = WDT_ADLY_16;      //WDT_ADLY_;16////;          
				 IE1 |= WDTIE; 
				 _BIS_SR(LPM0_bits+GIE);    // Wait for WDT interrup
		   		
				 ucAlarmTimer++;
				 
				 if(ucAlarmFlag == 0)    
				 {
					
					ucAlarmCounterFlag ^= 0x01 ;  
					vRedLed( ucAlarmCounterFlag  );
					
					if(Red_flag==0)
					{	
						vAlarm (0x01);
						Red_flag=1;
						Green_flag=0;
						Yellow_flag = 0;
						End_flag=0;
					}
					
					if(k>9)
					{	
						k=0;						
						Green_flag=0;
						Yellow_flag = 0;

						//if(End_flag==0)
						//{
							vAlarm (0x02);	
						//}	
					}
					vGreenLed(0);
				 }
				 if(ucAlarmTimer >= ALARM	)
				 {                 
					ucAlarmFlag = 1;                
					vRedLed(0);  
					//vAlarm (0);
					uiNormalTimer++;
					ucAlarmTimer = ALARM ;

					if(ucNormalFlag == 0)
					{	
						
						ucNormalCounterFlag  ^= 0x01 ;   
						vGreenLed( ucNormalCounterFlag); 
						

						if(Green_flag==0)
						{
						vAlarm ( 0x03);
						
						//for(k=0;k<15;)
						//vAlarm ( 0x03);
						Red_flag=0;
						Green_flag=1;
						Yellow_flag = 0;
						//End_flag=0;
						k=0;
						}

						
					}
					
					if(uiNormalTimer >= NORMAL)
					{
						uiNormalTimer = NORMAL;
						ucNormalFlag = 1;
						vGreenLed(0);
					  
					}
				 }
					   
			}
     
			else
			{   
			
				uiNormalTimer = 0;
				ucAlarmTimer = 0;
				ucGndAlarmTimer = 0;
				
				ucAlarmCounterFlag = 0;
				ucNormalCounterFlag = 0;
				ucAlarmCounterFlag = 0;
				
				ucAlarmFlag = 0;
				ucNormalFlag = 0;    
				ucGndAlarmFlag = 0;
				
				vRedLed(0);  
				vGreenLed(0); 
				//vAlarm(0x00);
				Red_flag=0;
				Green_flag=0;
				Yellow_flag = 0;
				End_flag=1;
				k=0;
				vSleep();
			 
			} 
		}
		WDTCTL = WDT_ARST_1000;
	}
}


