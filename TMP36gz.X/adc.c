/*
*  adc0 pin6
*
*  created: 14.04.23
*  Author: Erik Gilberg
*/

#include <avr/io.h>
void ADC0_init(uint8_t port);
uint16_t ADC0_read(void);
void ADC0_channels(uint8_t port);


void ADC0_init(uint8_t port){
    /* Disable digital input buffer */
    ADC0_channels(port);
    ADC0.CTRLC = ADC_PRESC_DIV4_gc | VREF_REFSEL_VDD_gc;   //CLK_PER divided by 4 | Internal referance
    ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_10BIT_gc;      //ADC Enable: enabled | 10-bit mode 
}
    

uint16_t ADC0_read(void) {
    ADC0.COMMAND = ADC_STCONV_bm;   //Start ADC conversion
    while(!(ADC0.INTFLAGS & ADC_RESRDY_bm)) {
        ;
    }
    ADC0.INTFLAGS = ADC_RESRDY_bm;
    return ADC0.RES; 
}

void ADC0_channels(uint8_t port){
    switch(port){
        case 0:
        PORTD.PIN0CTRL &= ~PORT_ISC_gm;
        PORTD.PIN0CTRL |= PORT_ISC_INPUT_DISABLE_gc;
        PORTD.PIN0CTRL &= ~PORT_PULLUPEN_bm;    
        ADC0.MUXPOS = ADC_MUXPOS_AIN0_gc;
        break;
        
        case 1:
        PORTD.PIN1CTRL &= ~PORT_ISC_gm;
        PORTD.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc;
        PORTD.PIN1CTRL &= ~PORT_PULLUPEN_bm;    
        ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc;
        break;
        
        case 2:
        PORTD.PIN2CTRL &= ~PORT_ISC_gm;
        PORTD.PIN2CTRL |= PORT_ISC_INPUT_DISABLE_gc;
        PORTD.PIN2CTRL &= ~PORT_PULLUPEN_bm;    
        ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;
        break;
        
        case 3:
        PORTD.PIN3CTRL &= ~PORT_ISC_gm;
        PORTD.PIN3CTRL |= PORT_ISC_INPUT_DISABLE_gc;
        PORTD.PIN3CTRL &= ~PORT_PULLUPEN_bm;    
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