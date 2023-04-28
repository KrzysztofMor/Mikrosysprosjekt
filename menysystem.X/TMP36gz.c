/*
*  TMP36gz 
*
*  created: 14.04.23
*  Author: Erik Gilberg
*/

#include <avr/io.h>
#include <stdbool.h>
uint16_t temperaturC(uint16_t temp, uint8_t tempScale);
uint16_t analogTempvalue(uint16_t temperature, uint16_t min, uint16_t max);
long map ( long x , long in_min , long in_max , long out_min , long out_max );
uint16_t read_i2c_temp(uint16_t pin, uint16_t minimum, uint16_t maximum);
uint16_t read_fantemperature(uint16_t pin);

//case 0 is Celsius, case 1 is Fahrenheit, case 2 is Kelvin
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
 maximum value.
 */
uint16_t analogTempvalue(uint16_t temperature, uint16_t min, uint16_t max){
        if(temperature > min){
            if(temperature > max) {
                temperature = max;
            }
            return map(temperature, min, max, 0, 255);      
        }
        else{
            return 0;
        }
}

/*
 map converts a value from one scale, over to a different scale.
 */
long map ( long x , long in_min , long in_max , long out_min , long out_max ) {
    return (x - in_min ) * ( out_max - out_min ) / ( in_max - in_min ) + out_min ;
}  

uint16_t read_i2c_temp(uint16_t pin, uint16_t minimum, uint16_t maximum){
    ADC0_init(pin); //Activates ADC on port D with initialized pin
    uint16_t adcVal = ADC0_read();  //Reads Analog value on input
    uint16_t TemperaturRead = temperaturC(adcVal, 0);   //converts A-value to temperature
    uint16_t analogValue = analogTempvalue(TemperaturRead, minimum, maximum);   //Converts Temperatur to analog value with minimum and maximum range. 
    return analogValue;
}

uint16_t read_fantemperature(uint16_t pin){
    ADC0_init(pin); //Activates ADC on port D with initialized pin
    uint16_t adcVal = ADC0_read();  //Reads Analog value on input
    uint16_t TemperaturRead = temperaturC(adcVal, 0);   //converts A-value to temperature
    return TemperaturRead;
}