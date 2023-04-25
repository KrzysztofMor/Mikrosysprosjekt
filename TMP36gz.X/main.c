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

#define DATA_SIZE 8
volatile uint8_t data_buffer[DATA_SIZE];
volatile uint8_t buffer_index = 0;
volatile bool was_read = false;
volatile bool data_ready = false;
volatile uint8_t current_cnt;


int main(void) {
    ADC0_init(3);                            //Aktiverer ADC for portD. Velg hvilken inngang mux skal tilkobles til
    USART3_init();                          //Aktiverer Serial connection over PB0 og PB1 
	
    sei();
    
    while(1){
        uint16_t adcVal = ADC0_read();
        uint16_t TemperaturRead = temperaturC(adcVal, 0);
        printf("Grader:%d\r\n",TemperaturRead);
        uint16_t analogValue = analogTempvalue(TemperaturRead, 0, 23);        //Temperatur, minimum, maximum
        printf("PWM:%d\r\n",analogValue);
        _delay_ms(1000);                          //Litt venting før neste verdi blir sendt 
        
}


