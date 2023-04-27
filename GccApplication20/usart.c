#define F_CPU 4000000UL // changed from 3333333#include "usart.h"
#define USART3_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 *(float)BAUD_RATE)) + 0.5)




#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include "usart.h"



// Prototypes
void USART3_init(void);
void USART3_sendChar(char c);
static int USART3_printChar(char c, FILE *stream);
void USART3_sendString(char *str);


// Defining USART_stream
static FILE USART_stream = FDEV_SETUP_STREAM(USART3_printChar, NULL, _FDEV_SETUP_WRITE);




void USART3_init(void)
{
	PORTB.DIRSET = PIN0_bm; //TX-pin as  output, changed from PORTC.DIR |= PIN0_bm;
	PORTB.DIRCLR = PIN1_bm; //RX-pin as  input, changed from PORTC.DIR &= ~PIN1_bm;

	USART3.BAUD = (uint16_t)USART3_BAUD_RATE(9600);
	USART3.CTRLB |= USART_TXEN_bm;
	
	stdout = &USART_stream; // replace the standard output stream with the user-defined stream

}

void USART3_sendChar(char c)
{
	while (!(USART3.STATUS & USART_DREIF_bm))	//wait while USART3 Data Register not empty
	{
		;
	}
	USART3.TXDATAL = c;	// Send c
}

static int USART3_printChar(char c, FILE *stream)	// Wrapping USART3_sendChar in a function computable with FDEV_SETUP_STREAM
{
	USART3_sendChar(c);
	return 0;
}


void USART3_sendString(char *str)
{
	for(size_t i = 0; i < strlen(str); i++)	//Send each cahr in str using USART3_sendString()
	{
		USART3_sendChar(str[i]);
	}
}

void Print_Binary(uint8_t motas) { //inspirert av gruppe 2 debugg
	int size = sizeof(motas)*8;
	
	printf("Binary %d: ", motas);
	for (int i = size -1; i>=0; i--){
		int bit = (motas >> i) & 1;
		printf("%d",bit);
	}
}






void Temp_data(uint8_t * Kong){ // inspirert av gruppe 2
	
	uint8_t high = (Kong[5]>>4) & 0x0F;
	uint8_t low = Kong[3] & 0x0F;
	uint16_t raaTemperatur = (low<<12| Kong[4] << 4 |high);
	float temperatur = (((float)raaTemperatur/65536)*200)-50;
	printf("temperatur %3.0f: ", temperatur);
	printf("tem %i", (int)temperatur);
}