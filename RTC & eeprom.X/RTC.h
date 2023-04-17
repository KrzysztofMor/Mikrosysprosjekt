#pragma once

void RTC_init();
bool delayTime(uint8_t time_in_s);
void enable_fan_runtime();
uint16_t get_runtime_fan(uint8_t fan);
void reset_fan_runtime(uint8_t fan);
void fan_memory();
