/*
*  TMP36gz 
*
*  created: 14.04.23
*  Author: Erik Gilberg
*/

#pragma once

//Converts analogvalue to a temperature value
uint16_t temperaturC(uint16_t temp, uint8_t tempScale);

//converts temperature value to dutycycle values for PWM signal
uint8_t analogTempvalue(uint8_t temperature, uint8_t min, uint8_t max);

//Reads temperature on chosen fanpin
uint16_t read_fantemperature(uint16_t pin);

//Converts a value on one scale over to another scale
long map(long x, long in_min, long in_max, long out_min, long out_max );


