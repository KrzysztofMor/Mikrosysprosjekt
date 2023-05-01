/*
*  TMP36gz 
*
*  created: 14.04.23
*  Author: Erik Gilberg
*/

#include <avr/io.h>
#include <stdbool.h>
uint16_t temperaturC(uint16_t temp, uint8_t tempScale);
uint8_t analogTempvalue(uint8_t temperature, uint8_t min, uint8_t max);
uint8_t map(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max );
uint16_t read_fantemperature(uint16_t pin);

/*
Reads value from analog pin and returns value as a temperature.
Tempscale set to celsius
*/
uint16_t read_fantemperature(uint16_t pin){
    ADC0_init(pin); //Activates ADC on port D and C with initialized pin
    uint16_t adcVal = ADC0_read();  //Reads Analog value on input
    uint16_t TemperaturRead = temperaturC(adcVal, 0);   //converts A-value to temperature
    return TemperaturRead;
}

/*
Takes in analog value from TMP36gz sensor, and calculate temperature.
For international use, you are free to choose whatever scale you want.
case 0 is Celsius, case 1 is Fahrenheit, case 2 is Kelvin
*/
uint16_t temperaturC(uint16_t temp, uint8_t tempScale){
    uint8_t c = (temp-500)/10;  //equation for TMP36gz sensor. Value is celcius.
    switch(tempScale){
        case 0:
        temp = c;
        break;
        
        case 1:
        temp = 30 + (c * 9)/5;
        break;
            
        case 2:
        temp = c + 273; 
        break;
    }
    
    return temp;
}

/*
 This function takes in temperature value and convert it to a analog value.
 Returns the temperature in analog scale, if temperature is above minimum or
 belove maximum value.
 */
uint8_t analogTempvalue(uint8_t temperature, uint8_t min, uint8_t max){
        uint8_t t;
        if(temperature > min){
            if(temperature > max) {
                temperature = max;
            }
            t = map(temperature, min, max, 0, 159);
        }
        else{
            t = 0;    
        }       
        return t;
}

/*
 map converts a value from one scale, over to a different scale.
 */
long map(long x , long in_min , long in_max , long out_min , long out_max ) {
    return (x - in_min ) * ( out_max - out_min ) / ( in_max - in_min ) + out_min ;
}  

