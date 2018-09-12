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
#define ALARM	    10	//断线滤波次数
#define ALARMCOUNTER	    180   //断线报警时间
//#define ALARMCOUNTER	    30   //断线报警时间 调试用
/*定义变量*********/
 
 //************************************************************************ 
#define CPU_F ((double)1000000) 
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0)) 
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0)) 
//************************************************************************

void delay1(unsigned int i)
{
    int j;
    for (j=i;j>= 0 ; j --);
    
}
/******************************************************************
** Function name	：void compare_open( void)
** Description		:打开比较器
******************************************************************/
//用于内部0.5*VCC参考电压测试
void compare_open( void )
{
  	P1OUT = 0x00;
	P1DIR |= CVCC;   //比较器电压设置为输出
	P1DIR &= ~(CCA0);  //比较器+CCA3
	P2SEL |= CCA0;   //使用第二功能 比较器+CCA3
	P1OUT |= CVCC;   //比较器+提供电压

	CACTL1 |= CARSEL; // CAEX = 0 CARSEL =1
	CACTL1 |= CAREF_2;//内部参考电压 0.5VCC  //采用内部参考电压，预留代码
	CACTL2 |= P2CA0+CAF; //    CA0 接 + terminal; CA3 -terminal
	CACTL1 |= CAON;   //开比较器
	delay1(10);     //延时 解决夹子电容问题	

	CAPD |= CAPD0;  //内部缓冲不使能+CAPD3
}

//检测端接比较器正输入端
void compareP_open( void )
{  
  	P1OUT = 0x00;
  	P1DIR |= CVCC;   		//比较器电压设置为输出
  	P1DIR &= ~(CCA0+CCA3);  //比较器
  	P2SEL |= CCA0+CCA3;   	//使用第二功能 比较器
  	P1OUT |= CVCC;   		//比较器+提供电压
	CACTL2 |= P2CA0+P2CA2+P2CA1+CAF ; // CA0 接 + terminal; CA3 -terminal
	CACTL1 |= CAON;   //开比较器
	delay1(100);
	CAPD |= CAPD0+CAPD3 ;  //内部缓冲不使能 
}
//检测端接比较器负输入端
void compareN_open( void )
{  
  P1OUT = 0x00;
  P1DIR |= CVCC;   //比较器电压设置为输出
  P1DIR &= ~(CCA0+CCA3);  //比较器
  P2SEL |= CCA0+CCA3;   //使用第二功能 比较器
  P1OUT |= CVCC;   //比较器+提供电压
  CACTL1 |= CAEX ; //  = 0 CARSEL =1
  CACTL2 |= P2CA0+P2CA2+P2CA1+CAF ;// CA0 接 + terminal; CA3 -terminal
  CACTL1 |= CAON;   //开比较器
  delay1(100);     //延时 解决夹子电容问题
  CAPD |= CAPD0+CAPD3 ;  //内部缓冲不使能
}

/******************************************************************
** Function name	：void compare_close( void )
** Description		:打开比较器
******************************************************************/
void compare_close( void )
{ 
 
  P1OUT = 0x00;
  P1DIR = 0xFF;  //设置为输出，低电平
  P1SEL = 0x00;
  CACTL1 &= ~CAON;
  CACTL2 &= ~(P2CA0+P2CA2+P2CA1);
  CAPD  &= ~(CAPD0+CAPD3); 
  
}

/******************************************************************
** Function name	：void vBuzzer_Off( void )
** Description		: 蜂鸣器打开
******************************************************************/
void vAlarm ( void )
{
	unsigned int uiAlarmCounter = 0;
	//P1OUT &= 0x00;
	P1DIR |= SPK ; 	//设为输出  
        P1OUT |= SPK;
        P1SEL = 0x00;
	//P1SEL |= SPK ; 	//设为输出   
       // CCTL0 |= OUTMOD_0;
        //TACTL |= MC_0;
	CCR0 =100 ; 		//1M时，蜂鸣器用的定时器165	//   PWM CCR0
	//CCR0 =1320 ;		//8M时，蜂鸣器用的定时器
	//CCR0 =2640 ;		//16M时，蜂鸣器用的定时器
	CCTL0 |= OUTMOD_4+CCIE;                         //  模式4toggle/set 比较中断使能
	TACTL = TASSEL_2 + MC_1+TACLR;                  // SMCLK, upmode
	_EINT();
        
	for(uiAlarmCounter = 0;uiAlarmCounter <= 800;uiAlarmCounter++)	//蜂鸣器间歇时间控制
	{
		 WDTCTL = WDT_ARST_1000; 
		_BIS_SR(LPM0_bits); //休眠延时

	}  
        
	//CCTL0 |= OUTMOD_0;
	//P1SEL &=  ~SPK;
	P1OUT &=  ~SPK;
    
	for(uiAlarmCounter = 0;uiAlarmCounter <=10000;uiAlarmCounter++)	//蜂鸣器间歇时间控制
	{ 
		WDTCTL = WDT_ARST_1000; 
		_BIS_SR(LPM0_bits); //休眠延时

	}
        
	_DINT();  
	CCTL0 &= ~CCIE;
	TACTL |= MC_0; //关闭定时器
         
	P1SEL = 0x00;
	P1OUT = 0x00;
	P1DIR = 0xFF;  //设置为输出，低电平
  
        
        
         
  
}

/******************************************************************
** Function name	:void sleep( void )
** Description		:休眠程序
******************************************************************/
void vSleep( void )
{           
	P1DIR = 0xFF;
	P1OUT = 0x00;
	BCSCTL1 |= DIVA_3;
	TACTL = TASSEL_1 + ID_3; //ACLK, upmode+ ID_3
	CCR0 = 500;  //      采用外部晶振32.768KHZ时 CCR0 = 1300  采用内部12K,20
	TACTL |= MC0 + TACLR;
	CCTL0 |= CCIE;  //  比较中断使能                                   
	_EINT(); // Enable WDT interrupt   
	WDTCTL = WDT_ARST_1000;          // Set Watchdog Timer interval to 250ms  
	//_BIS_SR(LPM3_bits);             // Enter LPM3 w
	_BIS_SR( CPUOFF + SCG0 + SCG1 );       
	_NOP();
	TACTL &= ~MC0;
	CCTL0 &= ~CCIE;
	BCSCTL1 &= ~DIVA_3;
	_DINT();
              
} 
/******************************************************************
** Function name	：TIMERA0_VECTOR(void)
** Description		:  timer_a0中断服务程序
******************************************************************/
#pragma vector = TIMERA0_VECTOR 
__interrupt void TIMERA0ISR( void )
{  
	WDTCTL = WDT_ARST_1000; 
	_BIC_SR_IRQ(LPM3_bits);
	_NOP(); 
 
}

/*****************************************************************
** Function name	：main( void )
** Description		: 控制主函数
******************************************************************/
void main(void)
{  
	unsigned char ucClampState = 0;
	unsigned char ucClampState1 = 0;
	unsigned char ucClampState2 = 0;
	
	unsigned char ucNormalTimer = 0;
	unsigned int  uiAlarmTimer = 0;
	unsigned char ucSpkFlag = 0;

	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	BCSCTL1 = CALBC1_1MHZ; //校准DCO为1MHZ
	DCOCTL = CALDCO_1MHZ;  

	while(1)
  	{
          
         // vAlarm (); 
        
		compareP_open();             //开比较器，准备采样比较起输出值 
		ucClampState = CACTL2&CAOUT ;       // 采样比较器输出值 
		compare_close();

		if(ucClampState)                // 比较器输出为1，表明夹子张开或者接地不好
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
     
		if(ucNormalTimer >= NORMAL)   //
		{
			ucNormalTimer = NORMAL;
			ucSpkFlag = 0;
			vSleep();   
		}
          
		WDTCTL = WDT_ARST_1000;  
	} 
}
