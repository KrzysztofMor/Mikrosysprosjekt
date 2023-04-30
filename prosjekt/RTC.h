/* 
 * File:   main.c
 * Author: ERIKG
 *
 * Created on April 16, 2023, 3:44 PM
 */
#pragma once

void RTC_init();
bool delayTime(uint32_t time_in_s);
void enable_fan_runtime();
uint16_t get_runtime_fan(uint8_t fan);
void reset_fan_runtime(uint32_t fan);
void fan_memory();