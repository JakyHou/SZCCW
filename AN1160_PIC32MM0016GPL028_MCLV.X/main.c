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
#include "defs.h"		//defines, function headers, pi, etc
#include <xc.h>
#include <sys/attribs.h>

int32_t main(void)
{
    Init_Peripheral();	// Clock , Port Configuration
    Init_MCCP();    	// MCCP initialization
    Init_ADC();         // Analog-Digital Converter initialization
    Init_Timers();  	// Timer1, SCCP2, SCCP3 initialization
    
    //defaults: CLKW rotation, motor stopped
    Flags.RunMotor = 0;
    Flags.Startup = 0;
    Flags.CLKW = 0;
    Flags.newCLKW = 0;
    Flags.DMCI_Control_SW = 0;		//default potentiometer read
    DesiredRPM = STARTUP_RPM;
    
    Flags.current_state = STATE_STOPPED;
    while(1)
    {
#if defined MCLV
        if(S2)
        { //CLKW/CCLKW switch
            while(S2) //debounce
                DelayNmSec(DEBOUNCE_DELAY);
            Flags.newCLKW = !Flags.CLKW;
        }
        if(S3)
        { //start/stop switch
            while(S3) //debounce
                DelayNmSec(DEBOUNCE_DELAY);

            if(Flags.current_state == STATE_STOPPED)
                Flags.current_state = STATE_STARTING;
            else
                Flags.current_state = STATE_STOPPING;
        }
#elif defined MCHV
        if(S1)
		{ //start/stop switch
            while(S1) //debounce
                DelayNmSec(DEBOUNCE_DELAY);
            
            if(Flags.current_state == STATE_STOPPED)
                Flags.current_state = STATE_STARTING;
            else
                Flags.current_state = STATE_STOPPING;
        }
#endif
        
        switch(Flags.current_state)
        {
            case STATE_STOPPING:
                Stop_Motor();
                Flags.current_state = STATE_STOPPED;
                break;

            case STATE_STARTING:
                if(Flags.Startup == 0)
                    Init_Motor();
                else
                    Start_Motor();
                break;

            case STATE_FAULT:
                Stop_Motor();
                DelayNmSec(1000);   //delay for motor to actually stop
                Flags.current_state = STATE_STARTING;
                break;

            case STATE_STARTED:
                if(Flags.newCLKW != Flags.CLKW)
                {
                    Stop_Motor();
                    Flags.CLKW = Flags.newCLKW;
                    DelayNmSec(1000);	//delay for motor to actually stop
                    Flags.current_state = STATE_STARTING;
                }
                break;

            case STATE_STOPPED:
                if(Flags.newCLKW != Flags.CLKW)
                    Flags.CLKW = Flags.newCLKW;
                break;
        }
    }
    return 0;
}

/*******************************************************************
Init_Motor()
	Procedure used to initialize all params for the motor and
for the AN1160 algorithm.
	Also rotor alignment is done here.
*******************************************************************/
void Init_Motor()
{
    int32_t i;	//auxiliary counter

    T1CONbits.TON = 0;
    CCP2CON1bits.ON = 0;
    CCP3CON1bits.ON = 0;
    TMR1 = 0;
    CCP2TMRbits.TMRL = 0;
    CCP3TMRbits.TMRL = 0;
    
    //Setting direction CLKW or CCLKW
    if (Flags.CLKW == 1)
    {
        for (i = 0; i < 6; i++)
        {
            PWM_STATE[i] = PWM_STATE_CLKW[5 - i];
            MotorPhaseAState[i] = MotorPhaseAState_CLKW[5 - i];
            MotorPhaseBState[i] = MotorPhaseBState_CLKW[5 - i];
            MotorPhaseCState[i] = MotorPhaseCState_CLKW[5 - i];
            ADC_CHANNEL[i] = ADC_CHANNEL_CLKW[5 - i];
            ADC_MASK[i] = ADC_MASK_CLKW[5 - i];
            ADC_XOR[i] = ADC_XOR_CLKW[5 - i];
        }
        for (i = 0; i < 64; i++)
            ADC_BEMF_FILTER[i] = ADC_BEMF_FILTER_CLKW[63 - i];
    }
    else
    {
        for (i = 0; i < 6; i++)
        {
           PWM_STATE[i] = PWM_STATE_CLKW[i];
           MotorPhaseAState[i] = MotorPhaseAState_CLKW[i];
           MotorPhaseBState[i] = MotorPhaseBState_CLKW[i];
           MotorPhaseCState[i] = MotorPhaseCState_CLKW[i];
           ADC_CHANNEL[i] = ADC_CHANNEL_CLKW[i];
           ADC_MASK[i] = ADC_MASK_CLKW[i];
           ADC_XOR[i] = ADC_XOR_CLKW[5-i];
        }
       for (i = 0; i < 64; i++)
           ADC_BEMF_FILTER[i] = ADC_BEMF_FILTER_CLKW[63-i];
    }

    stallCount = 0;

    InitPI(&PIDStructure,SpeedControl_P,SpeedControl_I,PI_ANTI_WINDUP,MAX_MOTOR_SPEED_REF,MIN_MOTOR_SPEED_REF,MIN_MOTOR_SPEED_REF);

    ADCCommState = 5;	//Always start with sector 6 forced
    RampDelay = RAMPDELAY_START;	//startup initial delay. also the delay used to hold the rotor for the first sector
        
    CCP1CON2 = ((CCP1CON2 & 0XC0FFFFFF) | PWM_STATE[ADCCommState]); //set PWM overdrive according to the PWM channel
    CurrentSpeed = STARTUP_DUTY;   //Initialize PWM duty cycle value to minimum duty allowed
    CCP1RBbits.CMPB = CurrentSpeed;
    DelayNmSec(500);
    
    SCCP3Average = SCCP3Value = CCP3TMRbits.TMRL = SCCP3_MAX;
    SCCP2Value = 0;
    Flags.RunMotor = 1;         // Turn the motor ON
    Flags.Startup = 1;          // Motor initialized, go to starting sequence
}

/**********************************************************************
Start_Motor()
	Procedure for starting the motor according to the implemented
startup ramp. After the ramp, PI loop training will begin.
**********************************************************************/
void Start_Motor()
{
    if(++ADCCommState >5)	// Change The Six-Step Commutation Sector
        ADCCommState = 0;

    AD1CHSbits.CH0SA = ADC_CHANNEL[ADCCommState];	//Change ADC Channel AN Selection
    
    //non-linear RAMP for startup.
    if(RampDelay > 10)
        RampDelay -= 10;
    else
	if(RampDelay > 3)
            RampDelay -= 3;

    if(RampDelay <= RAMPDELAY_MIN)
    {
        //if RampDelay is smaller than RAMPDELAY_MIN then we'll switch on BEMF detection.
        //BEMF Detection is started by starting Timer1, which will trigger ADC
        RampDelay = RAMPDELAY_MIN;
        CurrentSpeed = MIN_MOTOR_SPEED_REF;		//this is mainly for OpenLoopController, to start from a lower value
        if (T1CONbits.TON == 0)
            T1CONbits.TON = 1;
    }
    
    CCP1CON2 = ((CCP1CON2 & 0XC0FFFFFF) | PWM_STATE[ADCCommState]);	//overdrive and output next motor sector
    DelayNmSec(RampDelay);      //the delay for rotor to move
    adcBackEMFFilter = 0;       //clear the BEMF filter
}

/*********************************************************************
Stop_Motor()
	Procedure for stopping the motor from running and resetting
critical variables.
**********************************************************************/
void Stop_Motor()
{
    CCP1CON2 = (CCP1CON2 & 0XC0FFFFFF) | 0x00000000;         // override PWM pins low
    adcBackEMFFilter = 0;
    PIticks = 0;

    Flags.RunMotor = 0;         		// reset run flag
    Flags.Startup = 0;					// no startup sequence
    Flags.PreCommutationState = 0;		// clear pre-commutation state flag
    Flags.PotRead = 0;					// clear potentiometer read flag

    T1CONbits.TON = 0;      // Stop TIMER1
    CCP3CON1bits.ON = 0;    // Stop SCCP3 Timer
    CCP2CON1bits.ON = 0;	// Stop SCCP2 Timer
    
    TMR1 = 0;
    CCP2TMRbits.TMRL = 0;
    CCP3TMRbits.TMRL = 0;
}

/******************************************************************************
ADC Interrupt Service Routine()
	Actual implementation of the BLDC sensorless BEMF zero-crossing
detection algorithm. In this routine, the ADC will read the corresponding
phase, apply the majority detection filter, and find the zero-crossing point.
	Here we also read the potentiometer and execute the control loop for
the algoritm ( PI or Open loop ).
******************************************************************************/
void __attribute__ ((vector(_ADC_VECTOR), interrupt(IPL7SOFT), micromips)) _ADC1Interrupt(void)
{
    if(Flags.PreCommutationState == 0)
    {
        MotorPhaseA = MotorPhaseAState[ADCCommState];
		MotorPhaseB = MotorPhaseBState[ADCCommState];
		MotorPhaseC = MotorPhaseCState[ADCCommState];
            
        if(MotorPhaseA == 1)
            MotorPhaseA = ADC1BUF0;

        if(MotorPhaseB == 1)
            MotorPhaseB = ADC1BUF0;

        if(MotorPhaseC == 1)
            MotorPhaseC = ADC1BUF0;
         
		MotorNeutralVoltage = (MotorPhaseA + MotorPhaseB + MotorPhaseC) / 3;
         
		/********************* ADC SAMPLING & BMEF signals comparison ****************/
        BlankingCounter++;
		if(BlankingCounter > BLANKING_COUNT) 
        {
			ComparatorOutputs = 0;						// Precondition all comparator bits as zeros
			if(MotorPhaseA > MotorNeutralVoltage)
				ComparatorOutputs += 1;					// Set bit 0 when Phase A is higher than Neutural
			if(MotorPhaseB > MotorNeutralVoltage)
				ComparatorOutputs += 2;					// Set bit 1 when Phase B is higher than Neutural
			if(MotorPhaseC > MotorNeutralVoltage)
				ComparatorOutputs += 4;					// Set bit 2 when Phase C is higher than Neutral

        // Masking the BEMF signals according to the SECTOR in order to determine the ACTIVE BEMF signal
        // XOR operator helps to determine the direction of the upcoming zero-crossing slope

            if((ComparatorOutputs^ADC_XOR[ADCCommState])& ADC_MASK[ADCCommState])
                adcBackEMFFilter|=0x01;

            adcBackEMFFilter = ADC_BEMF_FILTER[adcBackEMFFilter];	//Majority detection filter

            if (adcBackEMFFilter&0b00000001) {

				if(Flags.current_state == STATE_STARTING)
                {	//When a valid BEMF zero crossing event has been detected, disable the motor start-up sequence
					Flags.current_state = STATE_STARTED;
					Flags.Startup = 0;
				}

                stallCount = 0;					//clear the stall counter whenever the BEMF signal is detected
				Flags.PreCommutationState = 1;	//set pre-commutation state flag

				// Calculate the time proportional to the 60 electrical degrees
    			CCP3CON1bits.ON = 0;  // Stop SCCP3 Timer 
    			SCCP3Average = ((SCCP3Average+ SCCP3Value + 2*CCP3TMRbits.TMRL)>>2);
    			SCCP3Value = CCP3TMRbits.TMRL;
    			CCP3TMRbits.TMRL = 0;
    			CCP3CON1bits.ON = 1;  // Start SCCP3 Timer

    			PhaseAdvanceTicks = ((SCCP3Average*Phase_Advance_Degrees)/60);	//Calculate the delay in TIMER1 counts proportional to the Phase Adv angle

    			// Calculate the time proportional to the 30 electrical degrees
    			// Load the SCCP2 with  the SCCP3 counts proportional to 30 deg minus the PHASE ADV angle delay
    			SCCP2Value = (((SCCP3Average)>>1)+PhaseAdvanceTicks);

    			if(SCCP2Value>1)
        			CCP2PRbits.PRL = SCCP2Value;
    			else
                    CCP2PRbits.PRL = SCCP2Value = 1;
            
    			CCP2CON1bits.ON = 1;	// Start SCCP2
            }
            else
                ++stallCount;	//if a BEMF zero crossing was not detected increment the stall counter
        }

        //Call the speed controller at a fixed frequency, which is (PI_TICKS*50us)
        if(++PIticks >= PI_TICKS)
        {
            #if defined OPEN_LOOP_CONTROL
            OpenLoopController();
            #elif defined PI_CLOSED_LOOP_CONTROL
            SpeedPILoopController();
            #endif
            PIticks = 0;
        }
	}
    else
    {
        if(Flags.DMCI_Control_SW == 0)
        {
            if(Flags.PotRead == 0)
            {
				AD1CHSbits.CH0SA = 8;		//Select Potentiometer on AN8
				Flags.PotRead = 1;          //Set potentiometer read flag
			}
            else
            {
                DesiredRPM = (unsigned int)(( (unsigned long)ADC1BUF0 * (MAX_RPM-MIN_RPM) ) >> 10 ) + MIN_RPM;
                DesiredSpeed = (unsigned int)(((unsigned long)DesiredRPM * RPM_PWM_FACTOR) >> 15);
            }
		}
        else
			DesiredSpeed = (unsigned int)(((unsigned long)DesiredRPM * RPM_PWM_FACTOR) >> 15);
    }
	AD1CON1bits.DONE = 0;
	IFS0bits.AD1IF = 0;
}


/**********************************************************************
MCCP Interrupt Service Routine()
	Occurs every 50 us
	Used to increment delay_counter ( for DelayNmSec )
	detects stalling
**********************************************************************/
void __attribute__ ((vector(_CCP1_VECTOR), interrupt(IPL3SOFT), micromips)) _CCP1Interrupt(void)
{
    delay_counter++;
    //rotor stall detection
    if (stallCount > BEMF_STALL_LIMIT && Flags.current_state == STATE_STARTED)
    {
        Flags.current_state = STATE_FAULT;      //go to FAULT state and restart the motor without pushing the button
        stallCount = 0;     //clear the stall counter
    }
    IFS0bits.CCP1IF = 0;	//clear MCCP Interrupt Flag
}


/**********************************************************************
SCCP2 Interrupt Service Routine()
	- used to switch (commute) the current driving sector of the motor
	- ends PreCommutationState
**********************************************************************/
void __attribute__ ((vector(_CCT2_VECTOR), interrupt(IPL5SOFT), micromips)) _CCT2Interrupt(void)
{
    adcBackEMFFilter = 0;
    BlankingCounter = 0;

    if (++ADCCommState > 5)
        ADCCommState = 0;

    AD1CHSbits.CH0SA = ADC_CHANNEL[ADCCommState];	//switch adc channel
    CCP1CON2 = ((CCP1CON2 & 0XC0FFFFFF) | PWM_STATE[ADCCommState]);  //switch sector
    
    Flags.PreCommutationState = 0;  //clear pre-commutation state flag
    Flags.PotRead = 0;              //clear potentiometer read flag
    IFS1bits.CCT2IF = 0;            //Clear SCCP2 Interrupt Flag
    CCP2CON1bits.ON = 0;            //Stop SCCP2
    CCP2TMRbits.TMRL = 0;
}

