/*
*  TMP36gz 
*
*  created: 14.04.23
*  Author: Erik Gilberg
*/

#define F_CPU 4000000UL
//#define RTC_PERIOD      //RTC periode
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include <stdbool.h>
#include "usart.h"
#include "adc.h"
#include "TMP36gz.h"

int main(void) {  
    USART3_init();  //Activates serial connection over PB0 and PB1	
	sei();
    
    while(1){
        uint16_t temp = readADC(0, 20, 100) //PD pin, min temp, max temp   
        printf("PWM:%d\r\n",temp);
        _delay_ms(1000); 
        
    }
}

