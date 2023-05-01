/*
*  ADC0
*
*  created: 14.04.23
*  Author: Erik Gilberg
*/
#pragma once

//initializing ADC on interrupt
void ADC0_init(uint8_t port);

//Reads signal from last interrupt
uint16_t ADC0_read(void);

//Choose what analog input should be red
void ADC0_channels(uint8_t port);

