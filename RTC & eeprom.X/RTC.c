#define F_CPU 4000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdbool.h>
#include "RTC.h"

volatile uint64_t runtime_seconds = 0;
volatile uint64_t pretime = 0;
#define one_second 0x3ff
int hour = 3600;
#define Fancount 8
uint16_t Fantime[Fancount];

ISR(RTC_CNT_vect){
    ++runtime_seconds;
    enable_fan_runtime();
    RTC.INTFLAGS = RTC_OVF_bm;
}

void RTC_init(){
    RTC.CTRLA = 1 << RTC_RTCEN_bp;     
    RTC.PER = one_second; 
    RTC.CLKSEL = RTC_CLKSEL_OSC1K_gc;
    RTC.INTCTRL = 1 << RTC_OVF_bp;
}

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

void enable_fan_runtime(){
    if(delayTime(10) == true){ 
        for(int i=0; i < Fancount; ++i){
            if(eeprom_is_ready()){
                Fantime[i]++;
                eeprom_write_byte((uint8_t*)i, Fantime[i]);
            }
        }
    }
    
}

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

void reset_fan_runtime(uint8_t fan){
    Fantime[fan] = 0;
}
