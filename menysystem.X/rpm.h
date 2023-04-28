#pragma once

void readRPM_init();
uint32_t readRPM();
void RPMpin(uint32_t pin);
//uint8_t compare_PWM(uint16_t TCA_CMP);
void pwm_ut(uint8_t duty);
//uint32_t readfanspeed(uint32_t pin);