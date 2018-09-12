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

#define ALARM      BIT5
#define CVCC       BIT1
#define CCA0       BIT0
#define CCA3       BIT3
/*定义变量*********/
/*
 unsigned long int  data=0; //AD装换值
 unsigned long int temp=0;
 char flag=0; //负电压标志位
 int sum=0;
 long int ww,gw,sw,bw,qw,ww;            //
 unsigned long int adConvertResult=0;   //AD转换值

*/
/******************************************************************
** Function name	：delay()
** Description		: 延时控制函数
******************************************************************/
void Delay(void)
{
   unsigned  int i = 30000;                                // Delay
  while(i>0) 
  {
    i--;
  }
}
void Delay_ns(void)
{
	int i;
    // P1DIR = 0xFF;
    //P1OUT &= 0xc0+ ZLG7289B_CS   ;  
   for(i=0;i<2;i++)
   {
	WDTCTL = WDT_ADLY_1000;           // Set Watchdog Timer interval to 1000ms
	IE1 |= WDTIE;  
	_EINT();                           // Enable WDT interrupt
	_BIS_SR(LPM3_bits);             // Enter LPM3 w/ interrupt
	_NOP();  
   }
}

void delay1(unsigned int i)
{
    int j;
    for (j=i;j>= 0 ; j --)
    {}

}
/******************************************************************
** Function name	：void compare_open( void )
** Description		:打开比较器
******************************************************************/
void compare_open( void )
{ 
  P1DIR |= CVCC;   //比较器电压设置为输出
  P1DIR &= ~(CCA0+CCA3);  //比较器
  P2SEL |= CCA0+CCA3;   //使用第二功能 比较器
  P1OUT |= CVCC;   //比较器+提供电压
  delay1(3);
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
  P1DIR |= ALARM; //设为输出                           // P1.1 output
  P1SEL |= ALARM; //设为输出                           // P1.1 option select
  CCR0 = 200;                               //   PWM CCR0
  CCTL0 = OUTMOD_4;                         //  模式4toggle/set
  TACTL = TASSEL_2 + MC_1;                  // SMCLK, up-down mode
}

/******************************************************************
** Function name	：void vBuzzer_Off( void )
** Description		: 蜂鸣器打开
******************************************************************/
void vBuzzer_Off( void )
{
 TACTL = TASSEL_2 + MC_0; 
 P1OUT &= 0x00;
 P1DIR |= 0xFF;  //设置为输出，低电平
}
/**********************************************************
** 函数名:  alarm
** 输　入: 
** 数据类型　 
** 数据类型　 
** 数据类型　 
** 输　出: 
** 功能描述:  报警程序，即蜂鸣器驱动程序
** 全局变量: 
** 调用模块: 
** 作　者: MAJOR
** 日　期: 2008.09.08
** 修　改:
** 日　期:
** 版本  initiate
***********************************************************/
void Alarm(void )
{
	unsigned int i;
	TACTL = TASSEL_2;//+ID_3 ;// + TACLR;//时钟选择SMCLK
	P1DIR |= ALARM; //设为输出
	CCR0 = 148;
	TACTL |=MC_1 ;
	CCTL0 |= CCIE;
	_EINT();
	//WDTCTL = WDTPW + WDTHOLD;  
  for( i = 0; i < 1100; i++ )
  {
	P1OUT  ^= ALARM; 
        TACTL |= TACLR + MC0;
	WDTCTL = WDT_ARST_1000;
        _BIS_SR( CPUOFF );//
  }
  
  for( i = 0; i < 1000; i++ )  
  {
        _NOP();
        WDTCTL = WDT_ARST_1000;//清看门狗1000MS
        _BIS_SR( CPUOFF );
  } 
	TACTL &= ~MC0; 
	CCTL0 &= ~CCIE;
	P1DIR |= 0xFF;
	//P1OUT = RED_LIGHT | 0x00; 

} 




/******************************************************************
** Function name	：void sleep( void )
** Description		:  休眠程序
******************************************************************/
void sleep( void )
{  
 	P1DIR = 0xFF;
	P1OUT = 0x00  ; 
	WDTCTL = WDT_ADLY_16;             // Set Watchdog Timer interval to 16ms
	IE1 |= WDTIE;  
	_EINT();                         // Enable WDT interrupt
	_BIS_SR(LPM3_bits);             // Enter LPM3 w/ interrupt
	_NOP();  
}

/******************************************************************
** Function name	：TIMERA0ISR( void )
** Description		:  定时器中断服务程序
******************************************************************/
#pragma vector = TIMERA0_VECTOR 
__interrupt void TIMERA0ISR( void )
{ 
  _BIC_SR_IRQ( CPUOFF + SCG0 + SCG1 );
}
// Watchdog Timer interrupt service routine
/******************************************************************
** Function name	：watchdog_timer(void)
** Description		:  看门狗中断服务程序
******************************************************************/
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
	_BIC_SR_IRQ(LPM3_bits) ;
	_NOP(); 
        WDTCTL = WDTPW + WDTHOLD;
}



/*****************************************************************
** Function name	：main( void )
** Description		: 控制主函数
******************************************************************/
void main(void)
{  
    unsigned char state = 0;
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT   
    BCSCTL3 = LFXT1S_2 ;
    /*
  //WDTCTL = WDT_ARST_1000; 
  P1DIR |= ALARM; //设为输出                           // P1.1 output
  P1SEL |= ALARM; //设为输出                           // P1.1 option select
  CCR0 = 200;                               // PWM Period/2
  CCTL0 = OUTMOD_4;                         // CCR0 toggle/set
  TACTL = TASSEL_2 + MC_0;                  // SMCLK, up-down mode
*/
//  _BIS_SR(LPM0_bits);                       // Enter LPM0
  while(1)
  { 
    compare_open();             /* 开比较器，准备采样比较起输出值 */
    state = CACTL2&CAOUT ;       /* 采样比较器输出值 */
    compare_close();
    if( state )                /* 比较器输出为0，表明夹子张开或者接地不好*/
    {
    vBuzzer_On();
    Delay();
    }
     
    vBuzzer_Off();
    Delay();
     
     
     WDTCTL = WDT_ARST_1000;  

  } 
 
}
