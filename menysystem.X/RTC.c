/* 
 * File:   main.c
 * Author: ERIKG
 *
 * Created on April 16, 2023, 3:44 PM
 */

#define F_CPU 4000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdbool.h>
#include "RTC.h"

#define one_second 0x3ff
#define Fancount 8
int hour = 3600;
uint16_t Fantime[Fancount];
volatile uint64_t runtime_seconds = 0;
volatile uint64_t pretime = 0;


void RTC_init();
uint16_t get_runtime_fan(uint8_t fan);
void fan_memory();
void enable_fan_runtime();
bool delayTime(uint8_t time_in_s);
void reset_fan_runtime(uint8_t fan);


ISR(RTC_CNT_vect){
    ++runtime_seconds;
    enable_fan_runtime();
    RTC.INTFLAGS = RTC_OVF_bm;
}

//////////////////Files that need to be used in main/////////////////////////

/*
 Initialisate RTC timer so that a interrupt occurs every secound 
 */
void RTC_init(){
    RTC.CTRLA = 1 << RTC_RTCEN_bp;     
    RTC.PER = one_second; 
    RTC.CLKSEL = RTC_CLKSEL_OSC1K_gc;
    RTC.INTCTRL = 1 << RTC_OVF_bp;
}

/*
 Returns stored timervalue from a array. Every hour this value is changed.
 Choose which fan you want to read runtime by initialize a number from 0 to 7. 
 */
uint16_t get_runtime_fan(uint8_t fan){
    uint16_t f = 0;
    switch(fan){
        case 0:
            f = Fantime[0];
            break;
        case 1:
            f = Fantime[1];
            break;
        case 2:
            f = Fantime[2];
            break;
        case 3:
            f = Fantime[3];
            break;
        case 4:
            f = Fantime[4];
            break;
        case 5:
            f = Fantime[5];
            break;
        case 6:
            f = Fantime[6];
            break;
        case 7:
            f = Fantime[7];
            break;
    }   
    return f;
}

/*
 Reads stored value from EEprom and writes into Fantime[]. This should be run
 on startup to restore timer values.
 */
void fan_memory(){
    for(int i=0; i < Fancount; ++i){
            Fantime[i] = 0;
    }
    for(int i=0; i < Fancount; ++i){
        if(eeprom_is_ready()){
           Fantime[i] = eeprom_read_byte((uint8_t*)i); 
        }
    }
}

/////////////Files that are only called in this C-file///////////////////////

/*
 Writes new values to Fancount[] every hour, and store these values to eeprom.
 This function is run when a interrupt occurs. 
 */
void enable_fan_runtime(){
    if(delayTime(hour) == true){
        eeprom_is_ready();
        for(int i=0; i < Fancount; ++i){
                Fantime[i]++;
                eeprom_write_byte((uint8_t*)i, Fantime[i]);
            }
        }   
}

/*
 Simple timer function that returns a boolean value. Initialize when called, and
 if statement will return true when time is reached.
 */
bool delayTime(uint8_t time_in_s){
    uint64_t meantime = runtime_seconds;
    if((meantime - pretime) > time_in_s){        
        pretime = runtime_seconds;
        return true;
    }
    else{
        return false;
    }
}

/*
 For every maintenence or switch of fans, the runtime should be reset. Call this
 function and initialize which fan runtime should be reset. Takes values 0 to 7.
 */
void reset_fan_runtime(uint8_t fan){
    Fantime[fan] = 0;
}