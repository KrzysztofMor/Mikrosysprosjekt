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
//volatile bool state = false;
//volatile uint32_t RPM;


void readRPM_init();
void RPMpin(uint32_t pin);
uint32_t readRPM();
//uint32_t compare_PWM(uint32_t TCA_CMP);
//void pwm_ut(uint8_t duty);
//uint32_t readfanspeed(uint32_t pin);

/*
ISR(TCB0_INT_vect){
    RPM = readRPM();
    state = true;
    TCB0.INTFLAGS = TCB_CAPT_bm;
}
*/

void readRPM_init(){
    TCB0.INTCTRL = TCB_CAPT_bm;
    TCB0.EVCTRL = TCB_CAPTEI_bm | TCB_FILTER_bm;
    TCB0.CTRLB = TCB_CNTMODE_FRQPW_gc;
    TCB0.CTRLA = TCB_CLKSEL_DIV1_gc | TCB_ENABLE_bm;  
}
/*
uint32_t readfanspeed(uint32_t pin){
    RPMpin(pin); 
    uint32_t speed;
    if(state == true){
        speed = RPM;
        state = false;
    }
    else{
        speed = 0;
    }
    return speed;
}
*/

uint32_t readRPM(){
    uint32_t rpm;
    //while(1){
        period = TCB0.CNT;
        p_Duty = TCB0_CCMP;        
        n_Duty = period - p_Duty;
        float T = (float)period/1750000;//800000.0; //1550000.0; //800000.0; //690000.0;//690000.0;//700000;//700000.0;//(1 / 20000000.0); 800000.0
        float duty_cycle = ((float)n_Duty/period); //(float)n_Duty/(float)period;

        rpm = (60.0 / T) * (1.0 - duty_cycle);
     /*   
        if (period > shortperiod){
            break;
        }
        else{
            continue;
        }
      */ 
      
    return rpm;
}

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

/*
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
*/
void pwm_ut(uint8_t duty){

	//PORTC.DIR = PIN0_bm;
    PORTD.DIRSET = PIN0_bm | PIN1_bm | PIN2_bm; // Setter PD0, PD1 og PD2 som utganger
    PORTC.DIRSET = PIN6_bm | PIN5_bm | PIN4_bm;// Setter PC6, PC5 og PC4 som utganger
    PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTD_gc | PORTMUX_TCA1_PORTC_gc; // Slår på PWM-output på Port D og C
    
    TCA0.SINGLE.PER = 0x9F;
    TCA1.SINGLE.PER = 0x9F;
	TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc; // Eneblaer output p? pwm pin 2 p? port D setter p? timer i singelslope mode
    TCA1.SINGLE.CTRLB = TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
    
	TCA0.SINGLE.CMP2 = 0x9F/5; 
	TCA1.SINGLE.CMP2 = 0x9F/5;
    
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm; //sl?r p? tomer med pre-scaler satt til 1
    TCA1.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm; //sl?r p? tomer med pre-scaler satt til 1
    
}