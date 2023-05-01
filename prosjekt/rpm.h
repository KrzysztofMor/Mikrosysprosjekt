/* 
 * File:   rpm.h
 * Author: ERIKG
 * Created on April 19, 2023, 5:36 PM
 */

#pragma once

//Activates capture mode on TCB
void readRPM_init();

//Reads values from TCB and calculate RPM
uint32_t readRPM();

//choose which pin should be red
void RPMpin(uint32_t pin);

//compares dutycycle of output signal with input signal
//uint8_t compare_PWM(uint16_t TCA_CMP);

//runs pwm signal with custom duty cycle
void pwm_ut(uint8_t duty);
