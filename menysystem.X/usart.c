/*
*  usart.c
*  created: 14.04.23
*  Author: Erik Gilberg
*/
/*
#define F_CPU 4000000UL
#include <avr/io.h>
#include <stdio.h>
#include <string.h>

#define USART3_BAUD_RATE(BAUD_RATE) ((float)(64*F_CPU / (16*(float)BAUD_RATE))+0.5)

int USART_printChar(char c, FILE *stream);
static FILE USART_stream = FDEV_SETUP_STREAM(USART_printChar, NULL, _FDEV_SETUP_WRITE);

void USART_init(void);
static void USART_sendChar(char c);
uint8_t USART_read(void);
void USART_sendString(char* str);



void USART_init(void){
    PORTB.DIRSET = PIN0_bm; 
    PORTB.DIRCLR = PIN1_bm;                             //Aktiverer transmitt pin
    USART3.BAUD = (uint16_t)USART3_BAUD_RATE(9600);     //Velger samme baudrate som på pcen
    USART3.CTRLB |= USART_TXEN_bm;                      //Transmitt er ikke benyttet om verdien er 0, dvs den er aktiv når det skal sendes noe.
    stdout = &USART_stream;
}

static void USART_sendChar(char c){             //Tar inn en bokstav, tall eller hva det måtte være
	while (!(USART3.STATUS & USART_DREIF_bm)){  //Venter på mottatt signal 
		;
	}
	USART3.TXDATAL = c;
}

void USART_sendString(char* str) {
	for (size_t i = 0; i < strlen(str); i++)
	{
		USART_sendChar(str[i]);
	}
}

uint8_t USART_read(void){
	while (!(USART3.STATUS & USART_RXCIF_bm))
	{
		;
	}
	return USART3.RXDATAL;
}


int USART_printChar(char c, FILE *stream) {
    USART_sendChar(c);
    return 0; 
}
*/