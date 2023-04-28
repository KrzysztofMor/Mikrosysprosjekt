/*
*  TMP36gz 
*
*  created: 14.04.23
*  Author: Erik Gilberg
*/

#pragma once

uint16_t temperaturC(uint16_t temp, uint8_t tempScale);
uint16_t analogTempvalue(uint16_t temperature, uint16_t min, uint16_t max);
uint16_t read_i2c_temp(uint16_t pin, uint16_t minimum, uint16_t maximum);
uint16_t read_fantemperature(uint16_t pin);


