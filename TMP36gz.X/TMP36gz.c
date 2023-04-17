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

//case 0 is Celsius, case 1 is Fahrenheit, case 2 is Kelvin

uint16_t temperaturC(uint16_t temp, uint8_t tempScale){
    uint8_t c = (temp-500)/10;
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

long map ( long x , long in_min , long in_max , long out_min , long out_max ) {
    return (x - in_min ) * ( out_max - out_min ) / ( in_max - in_min ) + out_min ;
}  