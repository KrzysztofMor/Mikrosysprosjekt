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

uint16_t readADC(uint16_t pin, uint16_t minimum, uint16_t maximum);

int main(void) {  
    USART3_init();  //Activates serial connection over PB0 and PB1	
	sei();
    
    while(1){
        uint16_t temp = readADC(0, 20, 100) //PD pin, min temp, max temp   
        printf("PWM:%d\r\n",temp);
        _delay_ms(1000); 
        
    }
}

uint16_t readADC(uint16_t pin, uint16_t minimum, uint16_t maximum){
    ADC0.init(pin); //Activates ADC on port D with initialized pin
    uint16_t adcVal = ADC0_read();  //Reads Analog value on input
    uint16_t TemperaturRead = temperaturC(adcVal, 0);   //converts A-value to temperature
    uint16_t analogValue = analogTempvalue(TemperaturRead, minimum, maximum);   //Converts Temperatur to analog value with minimum and maximum range. 
    return analogValue;
}

