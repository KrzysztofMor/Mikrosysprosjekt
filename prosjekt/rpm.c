/* 
 * File:   rpm.c
 * Author: ERIKG
 * Created on April 19, 2023, 5:36 PM
 */
#define F_CPU 4000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <util/delay.h>
#include "usart.h"


volatile uint16_t p_Duty;
volatile uint16_t n_Duty;
volatile uint16_t period;
volatile uint16_t dutycycle_prosent;
#define shortperiod 8000
uint16_t dutyCycle = 0;

void readRPM_init();
void RPMpin(uint32_t pin);
uint32_t readRPM();

//uint32_t compare_PWM(uint32_t TCA_CMP);
void pwm_ut(uint8_t duty);

//Initialize capturemode on TCB
void readRPM_init(){
    TCB0.INTCTRL = TCB_CAPT_bm;				//Interrupt controll on capture mode
    TCB0.EVCTRL = TCB_CAPTEI_bm | TCB_FILTER_bm;	//Capture on event | reads more cycles and compare them
    TCB0.CTRLB = TCB_CNTMODE_FRQPW_gc;			//capture puls width on pwm signal
    TCB0.CTRLA = TCB_CLKSEL_DIV1_gc | TCB_ENABLE_bm;  	//clockspeed internal clock | 
}

//save values from TCB caption, and calculates RPM.
uint32_t readRPM(){
    uint32_t rpm;	
    //while(1){
        period = TCB0.CNT;				//Value when interrupt occurs. This is the full period widht.
        p_Duty = TCB0_CCMP;        			//Positiv pulswidth - value from positiv flanc to first negativ flank.
        n_Duty = period - p_Duty;			//Negativ pulswidth
        float T = (float)period/1750000;		//Timeperiod
        float duty_cycle = ((float)n_Duty/period); 	//negative dutycycle

        rpm = (60.0 / T) * (1.0 - duty_cycle);		//calculates rpm
     /*   
        if (period > shortperiod){			//If noice affects period time, uncomment these and while loop
            break;				
        }
        else{
            continue;
        }
      */ 
      
    return rpm;
}

//choose which pin should be connected to TCB, through eventsystem channel0 
void RPMpin(uint32_t pin){
    // EVSYS(pin -> TCB0 & TCB1 CAPT)
    switch(pin){
        case 0:
        EVSYS.CHANNEL0 = EVSYS_CHANNEL0_PORTA_PIN4_gc;      //PA4   (WOA)
        PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
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
    EVSYS.USERTCB0CAPT = EVSYS_USER_CHANNEL0_gc; 
}


//compare dutycycles on outsignal with return signal. Commented because it doesn't work 
uint32_t compare_PWM(uint32_t TCA_CMP){
    float read_Dutycycle = 1 - (float)(p_Duty)/(float)(p_Duty+n_Duty);  //((float)p_Duty/(float)period);
    float write_Dutycycle = ((float)TCA_CMP/159.0); //(float)readRPM()/(float)returnRPMfromPWM();//(float)n_Duty/(float)period;
    float diffProsent = (float)(read_Dutycycle/write_Dutycycle)*100 + 3;    //(read_Dutycycle/write_Dutycycle)+0.03;//*100;
    //uint16_t difference = (float)((Readduty)/dutyPWM);//dutyPWM - Readduty - 79; //79 represent the difference between PWM signal and Taco sensor. 
    if(diffProsent < 0){
        abs(diffProsent);
    }
    
    if(diffProsent > 100){
        diffProsent = 100;
    }
    
    return diffProsent;//(diffProsent*100);  
}

//Enables pwm out signal with chosen dutycycle 
void pwm_ut(uint8_t duty){
    PORTD.DIRSET = PIN0_bm | PIN1_bm | PIN2_bm; 			// set PD0, PD1 og PD2 as outputs
    PORTC.DIRSET = PIN6_bm | PIN5_bm | PIN4_bm;				// set PC6, PC5 og PC4 as outputs
    PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTD_gc | PORTMUX_TCA1_PORTC_gc;  // Turns PWM output on Port D and C 
    
    TCA0.SINGLE.PER = 0x9F;						// Max value for dutycycle
    TCA1.SINGLE.PER = 0x9F;
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc; // Enables output on pwm pin 2 port D| set timer in singelslope mode
    TCA1.SINGLE.CTRLB = TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc; 
    
    TCA0.SINGLE.CMP2 = duty; 						// chosen value for dutycycle
    TCA1.SINGLE.CMP2 = duty;
    
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm; //Turns on timer with prescale as 1
    TCA1.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
}
