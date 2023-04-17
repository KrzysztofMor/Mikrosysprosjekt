/* 
 * File:   main.c
 * Author: ERIKG
 *
 * Created on April 16, 2023, 3:44 PM
 */
#define F_CPU 4000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include "RTC.h"





int main() {
    USART3_init(); 
    RTC_init();
    sei();
    char x = 0;
    char y = 0;
    char z = 0;
    while(1){
        if(delayTime(1) == true){
        printf("T: %u\n", z);    
        printf("M: %u\n", y);
        printf("S: %u", x);
        x++;
        if(x > 59){
            y++;
            x = 0;
        }
        if(y > 59){
            z++;
            y = 0;
            x = 0;
        }
        }    
    }
}