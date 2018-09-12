/******************************************************************
** Copyright (c) 2010, Development Department Of Qingdao Everbright Instrument Co., Ltd
** All rights reserved.

** File name    :ET- SGA-M
** Summary	:静电接地报警器，检测电阻50欧姆，8分钟休眠

** Current 
   Edition	:Version 1.0
** Mender	:Major
** Mend Date	:2010.05.20
** Description	:
**************************************************************************/
/**************************************************************************
使用到的端口说明：
P1.1-    比较器电压
P1.1-    比较器输入1
P1.3-    比较器输入2
P1.5-    蜂鸣器
**************************************************************************/

#include  <msp430x20x1.h>
#include  <in430.h>

#define SPK        BIT5
#define CVCC       BIT1
#define CCA0       BIT0
#define CCA3       BIT3


#define	NORMAL	    10
#define ALARM	    10
#define ALARMCOUNTER	    1700
/*定义变量*********/
 


void delay1(unsigned int i)
{
    int j;
    for (j=i;j>= 0 ; j --);
    
}
/******************************************************************
** Function name	：void compare_open( void )
** Description		:打开比较器
******************************************************************/
void compare_open( void )
{  
  P1OUT = 0x00;
  P1DIR |= CVCC;   //比较器电压设置为输出
  P1DIR &= ~(CCA0+CCA3);  //比较器
  P2SEL |= CCA0+CCA3;   //使用第二功能 比较器

  P1OUT |= CVCC;   //比较器+提供电压
  delay1(3);     //延时 解决夹子电容问题
  // CACTL1 |= CARSEL; // CAEX = 0 CARSEL =1
  //CACTL1 |= CAREF_2;//内部参考电压 0.5VCC  //采用内部参考电压，预留代码
  CACTL2 |= P2CA0+P2CA2+P2CA1 ; //CA0 接 + terminal; CA3 -terminal
  CACTL1 |= CAON;   //开比较器
  CAPD |= CCA0;  //内部缓冲不使能
}
/******************************************************************
** Function name	：void compare_close( void )
** Description		:打开比较器
******************************************************************/
void compare_close( void )
{
  CACTL1 &= ~CAREF_2;
  CACTL1 &= ~CAON;
  CACTL2 &= ~(P2CA0+P2CA2+P2CA1);
  CAPD  &= ~CCA0; 
  P1OUT &= 0x00;
  P1DIR |= 0xFF;  //设置为输出，低电平
}

/******************************************************************
** Function name	：void vBuzzer_On( void )
** Description		: 蜂鸣器打开
******************************************************************/
void vBuzzer_On( void )
{ 
  P1OUT &= 0x00;
  P1DIR |= SPK ; //设为输出                      
  P1SEL |= SPK ; //设为第二功能                          
  CCR0 = 165;                               //   PWM CCR0
  CCTL0 |= OUTMOD_4;                         //  模式4toggle/set
  TACTL |= TASSEL_2 + MC_1;                  // SMCLK, upmode
}

/******************************************************************
** Function name	：void vBuzzer_Off( void )
** Description		: 蜂鸣器打开
******************************************************************/
void vBuzzer_Off( void )
{
 TACTL |= MC_0; //关闭定时器
 P1SEL &= 0x00;
 P1OUT &= 0x00;
 P1DIR |= 0xFF;  //设置为输出，低电平
}

/******************************************************************
** Function name	：void vBuzzer_Off( void )
** Description		: 蜂鸣器打开
******************************************************************/
void vAlarm ( void )
{    
     unsigned int uiAlarmCounter = 0;
     P1OUT &= 0x00;
     P1DIR |= SPK ; //设为输出                      
     P1SEL |= SPK ; //设为输出   
     CCR0 = 165;                               //   PWM CCR0
     CCTL0 |= OUTMOD_4+CCIE;                         //  模式4toggle/set 比较中断使能
     TACTL = TASSEL_2 + MC_1+TACLR;                  // SMCLK, upmode
     _EINT();
     for(uiAlarmCounter = 0;uiAlarmCounter <= 1000;uiAlarmCounter++)
      {
        _BIS_SR(LPM0_bits); //休眠延时
        WDTCTL = WDT_ARST_1000; 
      }    
      CCTL0 |= OUTMOD_0;
      P1SEL &=  ~SPK;
      P1OUT &=  ~SPK;
      for(uiAlarmCounter = 0;uiAlarmCounter <= 1000;uiAlarmCounter++)
      {
        _BIS_SR(LPM0_bits); //休眠延时
        WDTCTL = WDT_ARST_1000; 
      }
     _DINT();  
     CCTL0 &= ~CCIE;
     TACTL |= MC_0; //关闭定时器
     P1SEL = 0x00;
     P1OUT &= 0x00;
     P1DIR |= 0xFF;  //设置为输出，低电平
  
}

/******************************************************************
** Function name	：void sleep( void )
** Description		:  休眠程序
******************************************************************/

void vSleep( void )
{       
      
 	P1DIR = 0xFF;
	P1OUT = 0x00  ; 
        P1SEL = 0x00;  
        CCTL0 = CCIE;  //  比较中断使能   
        CCR0 = 200;  //   CCR0  
        TACTL = TASSEL1 + MC_1 +TACLR;                  //ACLK, upmode                                                             
        _EINT(); // Enable WDT interrupt     
	_BIS_SR(LPM3_bits);             // Enter LPM3 w/ interrupt
         WDTCTL = WDT_ARST_250;           // Set Watchdog Timer interval to 250ms
	_NOP();  
       _DINT();  
       CCTL0 &= ~CCIE;
       TACTL |= MC_0; //关闭定时器
}


/******************************************************************
** Function name	：TIMERA0_VECTOR(void)
** Description		:  timer_a0中断服务程序
******************************************************************/
#pragma vector = TIMERA0_VECTOR 
__interrupt void TIMERA0ISR( void )
{   
  _BIC_SR_IRQ(LPM3_bits) ;
  _NOP(); 
  WDTCTL = WDT_ARST_250;
}

/******************************************************************
** Function name	：TIMERA1_VECTOR(void)
** Description		:  timer_a1中断服务程序
******************************************************************/
/*
#pragma vector = TIMERA1_VECTOR 
__interrupt void TIMERA1ISR( void )
{   
  _BIC_SR_IRQ(LPM3_bits) ;
  _NOP(); 
  //WDTCTL = WDT_ARST_1000;
}
*/
/*****************************************************************
** Function name	：main( void )
** Description		: 控制主函数
******************************************************************/
void main(void)
{  
    unsigned char ucClampState = 0;
    unsigned char ucNormalTimer = 0;
    unsigned int  uiAlarmTimer = 0;
    unsigned char ucSpkFlag = 0;   
    
   WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  //for (i = 0; i < 0xfffe; i++);             // Delay for XTAL stabilization
  if (CALBC1_1MHZ ==0xFF || CALDCO_1MHZ == 0xFF)                                     
  {  
    while(1);                               // If calibration constants erased
                                            // do not load, trap CPU!!
  } 
     BCSCTL1 = CALBC1_1MHZ; //校准DCO为1MHZ
     DCOCTL = CALDCO_1MHZ;
     BCSCTL1 &= ~(XTS+XT2OFF) ;
     
    // BCSCTL3 |= XCAP_1 ;   //ACLK 选择内部晶振 VLOCLK

  while(1)
  {  
    
    compare_open();             //开比较器，准备采样比较起输出值 
    ucClampState  = CACTL2&CAOUT ;       // 采样比较器输出值 
    compare_close();
    
   if(  ucClampState  )                // 比较器输出为0，表明夹子张开或者接地不好
   {
   uiAlarmTimer ++;
   ucNormalTimer = 0;
   }
   
   else 
   {
   ucNormalTimer++; 
   uiAlarmTimer = 0;
   }
   
   if(uiAlarmTimer >= ALARM)
   {
     if(ucSpkFlag == 0)
     {
     vAlarm ();     
     }
     else
     {
    uiAlarmTimer= ALARMCOUNTER+1;
     }
   }
   
   if(uiAlarmTimer >= ALARMCOUNTER)   //8分钟休眠
   {
    ucSpkFlag = 1;
    vSleep(); 
   }
     
   if(ucNormalTimer >= NORMAL)
   {
     ucNormalTimer = NORMAL;
     ucSpkFlag = 0;
    vSleep();
     
   }

     WDTCTL = WDT_ARST_1000;  

  } 
 
}
