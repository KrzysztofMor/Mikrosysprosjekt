/*
*  Uart
*
*  created: 14.04.23
*  Author: Erik Gilberg
*/

#define F_CPU 4000000UL
#define USART3_BAUD_RATE(BAUD_RATE) ((float)(64*F_CPU / (16*(float)BAUD_RATE))+0.5)

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
void USART3_init(void);
static void USART3_sendChar(char character);
static int USART3_printChar(char c, FILE *stream);
void USART3_sendstring(char *str);


static FILE USART_stream = FDEV_SETUP_STREAM(USART3_printChar, NULL, _FDEV_SETUP_WRITE);

void USART3_init(void) { 
    PORTB.DIRSET = PIN0_bm; 
    PORTB.DIRCLR = PIN1_bm;                             //Aktiverer transmitt pin
    USART3.BAUD = (uint16_t)USART3_BAUD_RATE(9600);     //Velger samme baudrate som på pcen
    USART3.CTRLB |= USART_TXEN_bm;                      //Transmitt er ikke benyttet om verdien er 0, dvs den er aktiv når det skal sendes noe.
    stdout = &USART_stream;
}

static void USART3_sendChar (char character) {              //Tar inn en bokstav, tall eller hva det måtte være
while(!(USART3.STATUS & USART_DREIF_bm)) {                  //Venter på mottatt signal 
    ;
}
USART3.TXDATAL = character;                                 //Sender bokstav/tall som er ført inn i funksjonen
}

static int USART3_printChar(char c, FILE *stream) {
    USART3_sendChar(c);
    return 0; 
}

void USART3_sendstring(char *str){
    for(size_t i = 0; i < strlen(str); i++){
        USART3_sendChar(str[i]);
    }
}