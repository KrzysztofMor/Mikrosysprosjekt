/*
*  ADC0
*
*  created: 14.04.23
*  Author: Erik Gilberg
*/

#include <avr/io.h>
void ADC0_init(uint8_t port);
uint16_t ADC0_read(void);
void ADC0_channels(uint8_t port);

//Initialise ADC to read analog input signal
void ADC0_init(uint8_t port){
    ADC0_channels(port);
    ADC0.CTRLC = ADC_PRESC_DIV4_gc | VREF_REFSEL_VDD_gc;   //CLK_PER divided by 4 | Internal referance
    ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_10BIT_gc;      //ADC Enable: enabled | 10-bit mode 
}
    
//Enable reading on interrupts, and returns saved value
uint16_t ADC0_read(void) {
    ADC0.COMMAND = ADC_STCONV_bm;                       //Start ADC conversion
    while(!(ADC0.INTFLAGS & ADC_RESRDY_bm)) {           //waiting until conversion is complete
        ;
    }
    ADC0.INTFLAGS = ADC_RESRDY_bm;                      //clears the interupt flag after red
    return ADC0.RES;                                    //returns conversion
}

//////////////////used within c file/////////////////////////////

//Connects ADC reading to selected port
void ADC0_channels(uint8_t port){
    switch(port){
        case 0:
        PORTE.PIN0CTRL &= ~PORT_ISC_gm;                 //clears the Interrupt Sense Control (ISC) bits
        PORTE.PIN0CTRL |= PORT_ISC_INPUT_DISABLE_gc;    //Digital input buffer disabled
        PORTE.PIN0CTRL &= ~PORT_PULLUPEN_bm;            //clears input pullup resistor
        ADC0.MUXPOS = ADC_MUXPOS_AIN0_gc;               //Directing mux position to right analog input pin
        break;
        
        case 1:
        PORTE.PIN1CTRL &= ~PORT_ISC_gm;
        PORTE.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc;
        PORTE.PIN1CTRL &= ~PORT_PULLUPEN_bm;    
        ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc;
        break;
        
        case 2:
        PORTE.PIN2CTRL &= ~PORT_ISC_gm;
        PORTE.PIN2CTRL |= PORT_ISC_INPUT_DISABLE_gc;
        PORTE.PIN2CTRL &= ~PORT_PULLUPEN_bm;    
        ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;
        break;
        
        case 3:
        PORTE.PIN3CTRL &= ~PORT_ISC_gm;
        PORTE.PIN3CTRL |= PORT_ISC_INPUT_DISABLE_gc;
        PORTE.PIN3CTRL &= ~PORT_PULLUPEN_bm;    
        ADC0.MUXPOS = ADC_MUXPOS_AIN3_gc;
        break;
        
        case 4:
        PORTD.PIN4CTRL &= ~PORT_ISC_gm;
        PORTD.PIN4CTRL |= PORT_ISC_INPUT_DISABLE_gc;
        PORTD.PIN4CTRL &= ~PORT_PULLUPEN_bm;    
        ADC0.MUXPOS = ADC_MUXPOS_AIN4_gc;
        break;
        
        case 5:
        PORTD.PIN5CTRL &= ~PORT_ISC_gm;
        PORTD.PIN5CTRL |= PORT_ISC_INPUT_DISABLE_gc;
        PORTD.PIN5CTRL &= ~PORT_PULLUPEN_bm;    
        ADC0.MUXPOS = ADC_MUXPOS_AIN5_gc;
        break;
        
        case 6:
        PORTD.PIN6CTRL &= ~PORT_ISC_gm;
        PORTD.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc;
        PORTD.PIN6CTRL &= ~PORT_PULLUPEN_bm;    
        ADC0.MUXPOS = ADC_MUXPOS_AIN6_gc;
        break;
        
        case 7:
        PORTD.PIN7CTRL &= ~PORT_ISC_gm;
        PORTD.PIN7CTRL |= PORT_ISC_INPUT_DISABLE_gc;
        PORTD.PIN7CTRL &= ~PORT_PULLUPEN_bm;    
        ADC0.MUXPOS = ADC_MUXPOS_AIN7_gc;
        break;
    }
}
