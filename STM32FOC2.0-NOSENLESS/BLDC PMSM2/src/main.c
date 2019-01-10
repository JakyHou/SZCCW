/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : main.c
* Author             : IMS Systems Lab
* Date First Issued  : 21/11/07
* Description        : Main program body.
********************************************************************************
* History:
* 21/11/07 v1.0
* 29/05/08 v2.0
* 27/06/08 v2.0.1
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOURCE CODE IS PROTECTED BY A LICENSE.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#define	  root
#include <Ram.h>

#include "STM103REG.h"
#include "stm32f10x_lib.h"
#include "stm32f10x_MClib.h"
#include "MC_Globals.h"
#include "stm32f10x_dac.h"
#include "lkznui.h"
#include "oled.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

void NVIC_Configuration(void);
void GPIO_Configuration(void);
void RCC_Configuration(void);
void DAC_Cmd(u32 DAC_Channel,FunctionalState NewState);
void LCD_Init(void);

u16 cnt,cnt_pre,possion_l,possion_h;
 u16 time,time1;

void UART3_Init(void);


/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{			//	s16 te;

#ifdef DEBUG
  debug();
#endif
    
  RCC_Configuration();
  GPIO_Configuration(); 



#ifdef THREE_SHUNT  //�����������ȡ��
  SVPWM_3ShuntInit(); //���õ���ȡ����PWM����
#elif defined ICS_SENSORS
  SVPWM_IcsInit();
#elif defined SINGLE_SHUNT
  SVPWM_1ShuntInit();
#endif
  
#ifdef ENCODER
   ENC_Init();
   #ifdef OBSERVER_GAIN_TUNING
      STO_StateObserverInterface_Init();
      STO_Init();
   #endif
#elif defined HALL_SENSORS
   HALL_HallTimerInit();
   #ifdef OBSERVER_GAIN_TUNING
      STO_StateObserverInterface_Init();
      STO_Init();
   #endif
#elif defined NO_SPEED_SENSORS
    STO_StateObserverInterface_Init();
    STO_Init();
   #ifdef VIEW_ENCODER_FEEDBACK
      ENC_Init();
   #elif defined VIEW_HALL_FEEDBACK
      HALL_HallTimerInit();
   #endif
#endif

#ifdef DAC_FUNCTIONALITY   
  MCDAC_Init();
#endif

  TB_Init();//��ʱ����ʼ��
  
  PID_Init(&PID_Torque_InitStructure, &PID_Flux_InitStructure, &PID_Speed_InitStructure);//��ʼ��PIDֵ
  //GPIO_SetBits(GPIOG,GPIO_Pin_2);
#ifdef BRAKE_RESISTOR
    MCL_Brake_Init();
#endif
      
  KEYS_Init();
  
  /* TIM1 Counter Clock stopped when the core is halted */
  //DBGMCU_Config(DBGMCU_TIM1_STOP, ENABLE);
  
  // Init Bus voltage and Temperature average  
  MCL_Init_Arrays();

    
//  Display_Welcome_Message();   
  NVIC_Configuration(); 


	OLED_Init();
	 

  welcoms();
  while(1)
  { 
  

  
		lkznui();
   
  }
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the TIM1 Pins.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{

                  
  /* Configure PF.06, PF.07, PF.08 and PF.09 as Output push-pull for debugging
     purposes */
    GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitType;
   

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO |RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|
                        	RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG|RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_USART3, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	//GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	/*-----------------GPIOA--------------------*/


	GPIO_InitStructure.GPIO_Pin = PGO_M1EN|PGO_M2EN;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
 	 //******************************************************************************
//����1��ʹ�ùܽ�������붨��
//******************************************************************************
//����UART3 TX (PA.02)��Ϊ�����������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;         //IO�ڵĵ�2��
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO���ٶ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //IO�ڸ����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);            //��ʼ������2���IO��

  // ���� USART1 Rx (PA.03)Ϊ�������� 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;           //IO�ڵĵ�3��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//IO����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);   
	
	

	
    
}



/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void)
{

   
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
   {
	
     USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	  
   }
   else if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
   { 
    USART_ClearITPendingBit(USART3, USART_IT_TXE);
   }   

}
/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{ 
  ErrorStatus HSEStartUpStatus;

  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  
  if(HSEStartUpStatus == SUCCESS)
  {
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the Vector Table base address.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
 NVIC_InitTypeDef NVIC_InitStructure;
#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif
    NVIC_InitStructure.NVIC_IRQChannel  = USART3_IRQChannel;         //ADC1,ADC2ȫ���ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;   //��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority   = 2;      //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
    //printf("Wrong parameters value: file %s on line %d\r\n", file, line);
  }
}
#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
