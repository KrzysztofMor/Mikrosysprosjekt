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
#include "TWI.h"

#define DATA_SIZE 8
volatile uint8_t data_buffer[DATA_SIZE];
volatile uint8_t buffer_index = 0;
volatile bool was_read = false;
volatile bool data_ready = false;
volatile uint8_t current_cnt;

void write_handler(uint8_t data_w) {
	if(buffer_index < DATA_SIZE) {
		data_buffer[buffer_index] = data_w;
		buffer_index++;
	}
}

uint8_t read_handler(void) {
	was_read = true;
	
	uint8_t data_rd = current_cnt;
	
	return data_rd;
}

void stop_handler(void) {
	buffer_index = 0;	
	
	if(!was_read) {
		data_ready = true;
	} 
	else
	{
		was_read = false;	 
	}
}

int main(void) {
    ADC0_init(3);                            //Aktiverer ADC for portD. Velg hvilken inngang mux skal tilkobles til
    USART3_init();                          //Aktiverer Serial connection over PB0 og PB1 
    
	TWI_initPins();
	
	TWI_initClient(0x6);
	
	//Assign handlers
	TWI_assignByteWriteHandler(&write_handler);
	TWI_assignByteReadHandler(&read_handler);
	TWI_assignStopHandler(&stop_handler);
	
	sei();
	
	uint8_t data[DATA_SIZE+1];
    
    while(1){
        uint16_t adcVal = ADC0_read();
        uint16_t TemperaturRead = temperaturC(adcVal, 0);
        printf("Grader:%d\r\n",TemperaturRead);
        uint16_t analogValue = analogTempvalue(TemperaturRead, 0, 23);        //Temperatur, minimum, maximum
        printf("PWM:%d\r\n",analogValue);
        _delay_ms(1000);                          //Litt venting før neste verdi blir sendt 
        
        if(data_ready) {
			cli(); /* Clear interrupts while handling data_buffer */
			memcpy(data, (const uint8_t *)data_buffer, DATA_SIZE);
			sei(); /* Re-enable interrupts */
			
			data[DATA_SIZE] = '\0'; /* Zero termination of C-string */
			printf("Received: %s\n",data); 
			
			data_ready = false;
		}
		current_cnt++;	
		_delay_ms(1000);

    }
}


