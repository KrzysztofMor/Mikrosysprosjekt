/*
*  adc0 pin6
*
*  created: 14.04.23
*  Author: Erik Gilberg
*/
#pragma once

void ADC0_init(uint8_t port);
uint16_t ADC0_read(void);
void ADC0_channels(uint8_t port);

