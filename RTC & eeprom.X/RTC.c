#define F_CPU 4000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

volatile uint64_t runtime_seconds = 0;
volatile uint64_t pretime = 0;
#define one_second 0x3ff

ISR(RTC_CNT_vect){
    ++runtime_seconds;
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