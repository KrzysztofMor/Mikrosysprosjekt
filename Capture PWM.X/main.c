//PWM Capture
//https://www.avrfreaks.net/s/topic/a5C3l000000Ud0WEAS/t163736

#define F_CPU 4000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include<util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "usart.h"
#include "rpm.h"

volatile bool state = false;
volatile uint32_t RPM;
volatile uint32_t RPM2;

ISR(TCB0_INT_vect){
    RPM = readRPM();
    state = true;
    TCB0.INTFLAGS = TCB_CAPT_bm;
}

int main(void) {
    // SYSCLK 20MHz
    USART3_init();
    //RPMpin(5);
    EVSYS.CHANNEL0 = EVSYS_CHANNEL0_PORTB_PIN5_gc;      //PA7   (WOD)
    PORTB.PIN5CTRL = PORT_PULLUPEN_bm;
    EVSYS.USERTCB0CAPT = EVSYS_USER_CHANNEL0_gc;  
    
    countPWM_TCB0();
    sei();                     
    
    while(1){ 
        if(state == true){
            PORTC_OUTSET = PIN7_bm;
            printf("RPM1: %lu\r\n", RPM);
            _delay_ms(500);
            
            state = false; // reset the flag
        } 
    }
}






