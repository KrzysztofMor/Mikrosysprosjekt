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

/*
 Initialize ADC to chosen pin on PORT D. For each time a analog signal should be
 red, this function need to be called.
 */
void ADC0_init(uint8_t port){
    /* Disable digital input buffer */
    ADC0_channels(port);
    ADC0.CTRLC = ADC_PRESC_DIV4_gc | VREF_REFSEL_VDD_gc;   //CLK_PER divided by 4 | Internal referance
    ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_10BIT_gc;      //ADC Enable: enabled | 10-bit mode 
}
    
/*
 Reads value from pin initialized in ADC_init(). Returns a value if ADC made a interrupt.
 */
uint16_t ADC0_read(void) {
    ADC0.COMMAND = ADC_STCONV_bm;   //Start ADC conversion
    while(!(ADC0.INTFLAGS & ADC_RESRDY_bm)) {   //checks when a result is ready
        ;
    }
    ADC0.INTFLAGS = ADC_RESRDY_bm;  
    return ADC0.RES;    //returns reading result on ADC
}

/*
 Choose which Pin should be connected to mux on ADC. Up to 8 temperature sensors
 can be red individually, but not at the same time. 
 */
void ADC0_channels(uint8_t port){
    switch(port){
        case 0:
        PORTD.PIN0CTRL &= ~PORT_ISC_gm; //Input/Sense Configuration
        PORTD.PIN0CTRL |= PORT_ISC_INPUT_DISABLE_gc;    //Digital Input disabled
        PORTD.PIN0CTRL &= ~PORT_PULLUPEN_bm;    //Disable pullup resistor
        ADC0.MUXPOS = ADC_MUXPOS_AIN0_gc;   //mux to Ain0 which is PORTD pin0
        break;
        
        case 1:
        PORTD.PIN1CTRL &= ~PORT_ISC_gm;
        PORTD.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc;
        PORTD.PIN1CTRL &= ~PORT_PULLUPEN_bm;    
        ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc;   //mux to Ain1 which is PORTD pin1
        break;
        
        case 2:
        PORTD.PIN2CTRL &= ~PORT_ISC_gm;
        PORTD.PIN2CTRL |= PORT_ISC_INPUT_DISABLE_gc;
        PORTD.PIN2CTRL &= ~PORT_PULLUPEN_bm;    
        ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;   //mux to Ain2 which is PORTD pin2
        break;
        
        case 3:
        PORTD.PIN3CTRL &= ~PORT_ISC_gm;
        PORTD.PIN3CTRL |= PORT_ISC_INPUT_DISABLE_gc;
        PORTD.PIN3CTRL &= ~PORT_PULLUPEN_bm;    
        ADC0.MUXPOS = ADC_MUXPOS_AIN3_gc;   //mux to Ain3 which is PORTD pin3
        break;
        
        case 4:
        PORTD.PIN4CTRL &= ~PORT_ISC_gm;
        PORTD.PIN4CTRL |= PORT_ISC_INPUT_DISABLE_gc;
        PORTD.PIN4CTRL &= ~PORT_PULLUPEN_bm;    
        ADC0.MUXPOS = ADC_MUXPOS_AIN4_gc;   //mux to Ain4 which is PORTD pin4
        break;
        
        case 5:
        PORTD.PIN5CTRL &= ~PORT_ISC_gm;
        PORTD.PIN5CTRL |= PORT_ISC_INPUT_DISABLE_gc;
        PORTD.PIN5CTRL &= ~PORT_PULLUPEN_bm;    
        ADC0.MUXPOS = ADC_MUXPOS_AIN5_gc;   //mux to Ain5 which is PORTD pin5
        break;
        
        case 6:
        PORTD.PIN6CTRL &= ~PORT_ISC_gm;
        PORTD.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc;
        PORTD.PIN6CTRL &= ~PORT_PULLUPEN_bm;    
        ADC0.MUXPOS = ADC_MUXPOS_AIN6_gc;   //mux to Ain6 which is PORTD pin6
        break;
        
        case 7:
        PORTD.PIN7CTRL &= ~PORT_ISC_gm;
        PORTD.PIN7CTRL |= PORT_ISC_INPUT_DISABLE_gc;
        PORTD.PIN7CTRL &= ~PORT_PULLUPEN_bm;    
        ADC0.MUXPOS = ADC_MUXPOS_AIN7_gc;   //mux to Ain7 which is PORTD pin7
        break;
    }
}
