/* 
 * File:   rpm.c
 * Author: ERIKG
 *
 * Created on April 19, 2023, 5:36 PM
 */
#define F_CPU 4000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#include<util/delay.h>


volatile uint16_t p_Duty;
volatile uint16_t n_Duty;
volatile uint16_t period;
volatile uint16_t dutycycle_prosent;
#define shortperiod 0.01


void countPWM_TCB0();
void RPMpin(uint8_t pin);
uint32_t readRPM();



void countPWM_TCB0(){
    
    TCB0.INTCTRL = TCB_CAPT_bm;
    TCB0.EVCTRL = TCB_CAPTEI_bm;
    TCB0.CTRLB = TCB_CNTMODE_FRQPW_gc;
    TCB0.CTRLA = TCB_CLKSEL_DIV1_gc | TCB_ENABLE_bm;  
    
    TCB1.INTCTRL = TCB_CAPT_bm;
    TCB1.EVCTRL = TCB_CAPTEI_bm;
    TCB1.CTRLB = TCB_CNTMODE_FRQPW_gc;
    TCB1.CTRLA = TCB_CLKSEL_DIV1_gc | TCB_ENABLE_bm; 

    //TCB0.CCMP = 0x7fff;
    //TCB0.CTRLB = TCB_CNTMODE_CAPT_gc;
    //TCB0.INTCTRL = TCB_CAPT_bm;
    //TCB0.CTRLA = TCB_CLKSEL_DIV1_gc | TCB_ENABLE_bm; 
    //TCB0.EVCTRL =  TCB_CAPTEI_bm | TCB_EDGE_bm;
}

uint32_t readRPM(){
    uint16_t  rpm;
    float T = period/700000.0;//(1 / 20000000.0); 800000.0

    while(1){
        period = TCB0.CNT;
        p_Duty = TCB0_CCMP;        
        n_Duty = period - p_Duty;
        rpm = (60 / T) * (1 - (n_Duty / (float)p_Duty));
    
        if (T > shortperiod){
            break;
        }
        else{
            continue;
        }
    }
    return rpm;
}

void RPMpin(uint8_t pin){
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
    EVSYS.USERTCB1CAPT = EVSYS_USER_CHANNEL1_gc;  
}