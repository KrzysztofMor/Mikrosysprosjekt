/* 
 * File:   main.c
 * Author: ERIKG
 *
 * Created on April 16, 2023, 3:44 PM
 */
#define F_CPU 4000000UL
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "usart.h"
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include "RTC.h"




int main() {
    USART3_init();
    PORTB_DIRCLR = PIN2_bm;
    RTC_init();
    fan_memory();
    sei();

    while(1){
        uint8_t x = get_runtime_fan(0);
        printf("fan1: %u\n", x);
        _delay_ms(1000);
        
        //reset_fan_runtime(0);         //Reset fancount when fan is replaced
        
        if (!(PORTC.IN & (PIN2_bm))) {
        //    reset_fan_runtime(0);
        }
    }
    
    
}
