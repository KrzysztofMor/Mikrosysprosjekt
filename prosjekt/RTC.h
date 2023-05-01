/* 
 * File:   main.c
 * Author: ERIKG
 *
 * Created on April 16, 2023, 3:44 PM
 */
#pragma once

//Initialisate RTC as a timer with interrupt every second
void RTC_init();  

//Simple timer that counts seconds, and return true when timevalue is reached
bool delayTime(uint32_t time_in_s);

//Returns runtime value for chosen fan. fan0 to fan7 is available
uint16_t get_runtime_fan(uint8_t fan);

//Resets runtime value for chosen fan. fan0 to fan7 is available
void reset_fan_runtime(uint32_t fan);

//Reads stored runtime from eeprom savings
void fan_memory();
