/* 
 * File:   rpm.c
 * Author: ERIKG
 * Created on April 19, 2023, 5:36 PM
 */
#define F_CPU 4000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include<util/delay.h>
#include "rpm.h"

volatile uint16_t p_Duty;   //Positive time of PWM signal
volatile uint16_t n_Duty;   //Negative time of PWM signal
volatile uint16_t period;   //Period of PWM signal
#define shortperiod 8000    //Lowlevel for period in caption
uint16_t dutyCycle = 0; 

void readRPM_init();
void RPMpin(uint8_t pin);
uint32_t readRPM();
float compare_RPM();
void PWM_ut(uint32_t duty);
float returnRPMfromPWM();

/*
 This function set up TCB timer as a counter. It counts the width of a PWM signal.
 */
void readRPM_init(){
    TCB0.INTCTRL = TCB_CAPT_bm; //Interrupt on High
    TCB0.EVCTRL = TCB_CAPTEI_bm;    //Caption mode
    TCB0.CTRLB = TCB_CNTMODE_FRQPW_gc;  //counts the width of pwm signal
    TCB0.CTRLA = TCB_CLKSEL_DIV1_gc | TCB_ENABLE_bm;    //clockspeed with same prescale as mainclock   
}

/*
 readRPM stores TCB values and calculate the speed in RPM. To compensate for 
 noise peaks shortperiod ensure that a period duration is right.
 */
uint32_t readRPM(){
    uint16_t rpm;
    while(1){
        period = TCB0.CNT;  //Value that resets on every period
        p_Duty = TCB0_CCMP; //Store the time positive part of pwm signal.       
        n_Duty = period - p_Duty;   //Store the negative part of pwm signal. 
        float T = (float)period/800000; //calculate the Time period of frequency
        float duty_cycle = (n_Duty/period);

        rpm = (60.0 / T) * (1.0 - duty_cycle);
        
        if (period > shortperiod){  //ensure period duration
            break;
        }
        else{
            continue;
        }
    }   
    return rpm;
}

/*
 RPMpin connect TCB channel to the right port with use of event system.
 To capture readings from different fans, event system can direct TCB counter
 to different ports that are supported by TCB. 
 */
void RPMpin(uint8_t pin){
    // EVSYS(pin -> TCB0 & TCB1 CAPT)
    switch(pin){
        case 0:
        EVSYS.CHANNEL0 = EVSYS_CHANNEL0_PORTA_PIN4_gc;  //PortA pin4  (WOA)
        PORTA.PIN4CTRL = PORT_PULLUPEN_bm;  //Pullup resistor on port
        break;
        
        case 1:
        EVSYS.CHANNEL1 = EVSYS_CHANNEL0_PORTA_PIN5_gc;      //PA5   (WOB)
        PORTA.PIN5CTRL = PORT_PULLUPEN_bm;
        break;
        
        case 2:
        EVSYS.CHANNEL0 = EVSYS_CHANNEL0_PORTA_PIN6_gc;      //PA6   (WOC)
        PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
        break;
        
        case 3:
        EVSYS.CHANNEL0 = EVSYS_CHANNEL0_PORTA_PIN7_gc;      //PA7   (WOD)
        PORTA.PIN7CTRL = PORT_PULLUPEN_bm;
        break;
        
        case 4:
        EVSYS.CHANNEL0 = EVSYS_CHANNEL0_PORTB_PIN4_gc;      //PB4   (WOA)
        PORTB.PIN4CTRL = PORT_PULLUPEN_bm;
        break;
        
        case 5:
        EVSYS.CHANNEL0 = EVSYS_CHANNEL0_PORTB_PIN5_gc;      //PB5   (WOB) 
        PORTB.PIN5CTRL = PORT_PULLUPEN_bm;
        break;
    }
    EVSYS.USERTCB0CAPT = EVSYS_USER_CHANNEL0_gc;  //link TCB with event channel
    EVSYS.USERTCB1CAPT = EVSYS_USER_CHANNEL1_gc;  
}

/*
 compare_RPM use counted PWM and compare with the output PWM signal. 
 If a fan stops or run poorly, this prosent value will indicate that. 
 */
float compare_RPM(){
    float RPMdiff = (float)readRPM()/(float)returnRPMfromPWM(); //(float)n_Duty/(float)period;
    uint16_t dutyPWM = 0x9F/2;
    //uint16_t difference = (float)((Readduty)/dutyPWM);//dutyPWM - Readduty - 79; //79 represent the difference between PWM signal and Taco sensor. 
    return RPMdiff;  
}

void PWM_ut(uint32_t duty){
	//Velger pin out
	PORTD.DIR = PIN2_bm;
	PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTD_gc; //sl?r p? pwm output p? portd
	
	TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc; // Eneblaer output p? pwm pin 2 p? port D setter p? timer i singelslope mode
	
	//Formel: F_CPU/(TCA_SINGLE_CLKSEL_DIVn_gc*(TCA0.SINGLE.PER + 1)) = PWM frekvens
	TCA0.SINGLE.PER = 0x9F; //signal periode (t)
	TCA0.SINGLE.CMP2 = duty; //0x9F/2; //duty cycle
	
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm; //sl?r p? tomer med pre-scaler satt til 1
}
