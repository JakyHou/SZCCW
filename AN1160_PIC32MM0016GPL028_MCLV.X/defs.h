/* *********************************************************************
 * (c) 2017 Microchip Technology Inc. and its subsidiaries. You may use
 * this software and any derivatives exclusively with Microchip products.
 *
 * This software and any accompanying information is for suggestion only.
 * It does not modify Microchip's standard warranty for its products.
 * You agree that you are solely responsible for testing the software and
 * determining its suitability.  Microchip has no obligation to modify,
 * test, certify, or support the software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE, OR ITS INTERACTION WITH
 * MICROCHIP PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY
 * APPLICATION.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL,
 * PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF
 * ANY KIND WHATSOEVER RELATED TO THE USE OF THIS SOFTWARE, THE
 * motorBench(TM) DEVELOPMENT SUITE TOOL, PARAMETERS AND GENERATED CODE,
 * HOWEVER CAUSED, BY END USERS, WHETHER MICROCHIP'S CUSTOMERS OR
 * CUSTOMER'S CUSTOMERS, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES OR THE DAMAGES ARE FORESEEABLE. TO THE
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
 * CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT
 * OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS
 * SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF
 * THESE TERMS.
 *
* *****************************************************************************/
#ifndef DEFS_H
#define DEFS_H

#include "pi.h"
#include "periph.h"
#include <xc.h>
#include <stdint.h>

/* MCLV2 or MCHV2 development board can be selected based on the requirement.
 * At any time, only one should be defined.
 * Based on the selection the recommended configurations will be enabled */

#define MCLV
//#define MCHV

#define FCY 24000000    //Internal 24MHz clock SPLLCONbits.PLLMULT = 1; //PLL X3 8M*3=24M
#define FPWM 20000		//20,000 Hz PWM

//##################### SPEED CONTROLLER. Choose only one of the two ############################

//#define OPEN_LOOP_CONTROL			//Openloop Control
#define PI_CLOSED_LOOP_CONTROL		//Closedloop Control

#define PHASE_ADVANCE_DEGREES	0		//degrees for phase advancing

//########### Motor Control Definitions #################
#if defined MCLV    // MCLV - Hurst Motor (DMB0224C10002)
    #define STARTUP_DUTY            800     //sets the starting motor speed in forced commutation mode; increase the value for a lower speed
                                            //CCP1RBbits.CMPB_____L______---H--_____L_____--H---_______ L: 800 H:((FCY/FPWM)-1)-(CCP1RBbits.CMPB)
    #define STARTUP_RPM             1000	//final RPM after startup. this becomes the minimum RPM#define MIN_RPM 
    #define MIN_RPM                 1000 // 750     // motor RPM at MIN_MOTOR_SPEED_REF
    #define MAX_RPM                 3100    // motor RPM at MAX_MOTOR_SPEED_REF
    #define POLEPAIRS               3// 5//  Number of pole pairs of the motor

    #define RAMPDELAY_START         80      //in ms; the starting sector comutation period
    #define RAMPDELAY_MIN           4       //in ms; minimum period for startup ramp; when reaching this value, it will start looking for BEMF

    #define BLANKING_COUNT          40       // Blanking count expressed in PWM periods used to avoid false zero-crossing detection after commutating motor
    #define BEMF_STALL_LIMIT        5000     // If no BEMF signal is detected for (BEMF_STALL_LIMIT*BLANKING_COUNT * 50us => 5000*40*50/10e6=10s) then it is assumed the rotor is stalled

    #define MAX_MOTOR_SPEED_REF     2000    //1199    // corresponds to MAX_RPM
    #define MIN_MOTOR_SPEED_REF     700//500     // decrease or increase this value to set the minimum motor speed
                                            // The minimum motor speed in closed loop is MAX_RPM*MIN_MOTOR_SPEED_REF/MAX_MOTOR_SPEED_REF

    #define RPM_PWM_FACTOR (uint16_t)(32768 * ((float)MAX_MOTOR_SPEED_REF / (float)MAX_RPM))	//PWM Duty cycle = RPM_PWM_FACTOR * Speed_in_RPM

    #define BEMF_VDDMAX             520     //465
    /*	on MCLV:
        R10/(R10+R14) * DC Voltage / 3.3 V * 1024
        2K/32K * 24 / 3.3 * 1024 = 465 
     * 
     * now we use 12DC so
     * 7.5k/(47+7.5)k * 12v / 3.3v * 1024 = 512
    */
#elif defined MCHV      //MCHV - High Voltage PMSM Motor (Beijing Eletechnic 80-252140-220)
    #define STARTUP_DUTY            700     //sets the starting motor speed in forced commutation mode; increase the value for a lower speed
    #define STARTUP_RPM             1000
    #define MIN_RPM                 750     // motor RPM at MIN_MOTOR_SPEED_REF
    #define MAX_RPM                 3600    // motor RPM at MAX_MOTOR_SPEED_REF
    #define POLEPAIRS               2       // Number of pole pairs of the motor

    #define RAMPDELAY_START			40	   //in ms;	the starting sector comutation period
    #define RAMPDELAY_MIN			4	   //in ms;	minimum period for startup ramp; when reaching this value, it will start looking for BEMF

    #define BLANKING_COUNT          40       // Blanking count expressed in PWM periods used to avoid false zero-crossing detection after commutating motor
    #define BEMF_STALL_LIMIT        5000     // If no BEMF signal is detected for (BEMF_STALL_LIMIT*BLANKING_COUNT * 50us) then it is assumed the rotor is stalled

    #define MAX_MOTOR_SPEED_REF     3600    // corresponds to MAX_RPM
    #define MIN_MOTOR_SPEED_REF     700     // decrease or increase this value to set the minimum motor speed
                                        // The minimum motor speed in closed loop is MAX_RPM*MIN_MOTOR_SPEED_REF/MAX_MOTOR_SPEED_REF

    #define RPM_PWM_FACTOR (uint16_t)(32768 * ((float)MAX_MOTOR_SPEED_REF / (float)MAX_RPM))	//PWM Duty cycle = RPM_PWM_FACTOR * Speed_in_RPM

    #define BEMF_VDDMAX             700
    /*	on MCHV:
    R96/(R80+R88+R96) * DC Voltage / 3.3V * 1024
    2.2K/302K * 310 / 3.3V * 1024 = 700
    */
#endif

/* Switch configuration based on the board selected*/
#if defined MCLV
    #define S3	!PORTAbits.RA4	//S3 button
    #define S2	!PORTCbits.RC9	//S2 button
#elif defined MCHV
    #define S1	!PORTBbits.RB4	//S1 button
#endif

/* ####################  PI loop constants ############################## */
#define PI_P_TERM	2000
#define PI_I_TERM	200
#define PI_ANTI_WINDUP  0x7FFF

#define PWM_100us_FACTOR            (FPWM/10000) //2

/*******************  Derived Definitions  - Do not change*******************/
#define PI_TICKS        80                          // Speed Controller frequency ->  80 ADC periods
#define MAX_DUTY_CYCLE  (int32_t)((FCY/FPWM)-1)   // 100% duty cycle

/* SCCP3 Timer measures the motor speed by measuring the time the rotor takes 
 * to make a 60 degree electrical rotation angle.
 * SCCP3 Timer minimum value is: 1/(MAX_RPM/60)/POLEPAIRS*FCY/SCCP3Prescaler/(360/60) */

#define SCCP3_MIN       (int64_t)60/MAX_RPM/POLEPAIRS*FCY/64/6
#define SCCP3_MAX       (uint16_t)(SPEEDMULT/MIN_MOTOR_SPEED_REF)

// CONVERSION SPEED FACTOR - SPEEDMULT
#define SPEEDMULT       (int32_t)(MAX_MOTOR_SPEED_REF * SCCP3_MIN)  //Factor used to scale the Desired speed to the actual motor speed

//###################### Flags, State Machine, etc #############################

//application flags and state machine
#define STATE_STOPPED 0		//motor is stopped
#define STATE_STOPPING 1	//event to tell the motor to stop
#define STATE_STARTING 2	//event to tell the motor to start, and startup sequence
#define STATE_STARTED 3		//motor is running
#define STATE_FAULT 4		//motor fault

 typedef struct
{
    unsigned RunMotor : 1;
    unsigned Startup :  1;
    unsigned CLKW : 1;
    unsigned newCLKW :  1;
    unsigned PreCommutationState :  1;
    unsigned PotRead :  1;
    unsigned DMCI_Control_SW: 1;
    unsigned current_state:	  3;
    unsigned unused :   6;
} TFlags;
extern TFlags Flags;

//############## Clockwise and Counter-Clockwise rotation constants, majority detection filter, etc ####################

/*override values for each sector*/
extern const uint32_t PWM_STATE_CLKW[6];
extern uint32_t PWM_STATE[6];

/*ADC Channel AN Select*/
extern const uint16_t ADC_CHANNEL_CLKW[6];
extern uint16_t ADC_CHANNEL[6];

/*Motor Phases*/
extern const uint32_t MotorPhaseAState_CLKW[6];
extern const uint32_t MotorPhaseBState_CLKW[6];
extern const uint32_t MotorPhaseCState_CLKW[6];
extern uint32_t MotorPhaseAState[6];
extern uint32_t MotorPhaseBState[6];
extern uint32_t MotorPhaseCState[6];

/*AND & OR operators for masking the active BEMF signal*/
extern const uint16_t ADC_MASK_CLKW[6];
extern uint16_t ADC_MASK[6];

extern const uint16_t ADC_XOR_CLKW[6];
extern uint16_t ADC_XOR[6];

/*BEMF Majority Function Filter values*/
extern const uint8_t ADC_BEMF_FILTER_CLKW[64];
extern uint8_t ADC_BEMF_FILTER[64];

//###################### Miscellaneous Variables and Defines ####################
extern uint8_t ADCCommState;            // state for current motor sector
extern uint8_t adcBackEMFFilter;        // stores value for each ADC filtering
extern uint16_t Phase_Advance_Degrees;	// stores value for Phase Advance degrees. Modify it through DMCI
extern uint16_t PhaseAdvanceTicks;  	// counter for ticks to be extracted out of Timer2 when commuting to add Phase Advance
extern uint8_t BlankingCounter;         // blanking counter, for rejecting some values out of the filter
extern uint32_t PIticks;                // counter for skipping PI calculation
extern uint32_t stallCount;             // counter for stalling
extern uint32_t RampDelay;              // variable used to create the startup ramp delay
extern volatile int32_t delay_counter;	// used for delays. is incremented automaticalLy in MCPWM interrupt, each 50 us
extern uint32_t MotorNeutralVoltage;	// Motor Neutral Voltage calculAtion
extern uint32_t MotorPhaseA;			// Motor Phase A current voltage
extern uint32_t MotorPhaseB;			// Motor Phase B current voltage
extern uint32_t MotorPhaseC;			// Motor Phase C current voltage
extern uint16_t ComparatorOutputs;

extern uint32_t SCCP3Value;             //Used for calculating next commutation occurring
extern uint32_t SCCP3Average;			//Used for calculating next commutation occurring
extern uint32_t SCCP2Value;

extern uint32_t CurrentSpeed, DesiredSpeed, DesiredRPM, CurrentDuty, DesiredDuty; 	//speed definitions

//PI Controller definitions
extern uint32_t SpeedControl_P;         // The P term for the PI speed control loop. Modify in defs.c
extern uint32_t SpeedControl_I;         // The I term for the PI speed control loop. Modify in defs.c
extern tPIParm PIDStructure;            // PID Structure

void SpeedPILoopController(void);		// PI Loop Controller
void OpenLoopController(void);			// Open Loop Controller

//other functions
void DelayNmSec(uint32_t N);	//delays N milLi seconds. MCCP Timer Based blocking delay.

//motor control functions. defined here, implemented in main.c
void Init_Motor(void);		//just initialize motor and PI
void Start_Motor(void);		//sequence to attempt to forcely start the motor
void Stop_Motor(void);		//stops motor

#endif
