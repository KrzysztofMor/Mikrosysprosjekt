/*
*  TMP36gz 
*
*  created: 14.04.23
*  Author: Erik Gilberg
*/

#pragma once

uint16_t temperaturC(uint16_t temp, uint8_t tempScale);
uint8_t analogTempvalue(uint8_t temperature, uint8_t min, uint8_t max);
//uint16_t read_i2c_temp(uint16_t pin, uint16_t minimum, uint16_t maximum);
uint16_t read_fantemperature(uint16_t pin);
uint8_t map(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max );


