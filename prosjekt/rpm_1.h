#pragma once

void readRPM_init();
uint32_t readRPM();
void RPMpin(uint8_t pin);
float compare_RPM();
void PWM(uint32_t duty);
float returnRPMfromPWM();