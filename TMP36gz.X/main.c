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


int main(void) {
    ADC0_init(3);   //Activates ADC on port D with initialized pin
    USART3_init();  //Activates serial connection over PB0 and PB1
	
	sei();
    
    while(1){
        uint16_t adcVal = ADC0_read();  //Reads Analog value on input
        uint16_t TemperaturRead = temperaturC(adcVal, 0);   //converts A-value to temperature
        //printf("Grader:%d\r\n",TemperaturRead);
        uint16_t analogValue = analogTempvalue(TemperaturRead, 0, 23);  //Converts Temperatur to analog value with minimum and maximum range.
        printf("PWM:%d\r\n",analogValue);
        _delay_ms(1000);                          //Litt venting før neste verdi blir sendt 
        
    }
}


