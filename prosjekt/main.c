////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						/* FRONT PAGE */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define F_CPU 4000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "rpm.h"
#include "RTC.h"
#include "TMP36gz.h"
#include "adc.h"

//Declaring max amount of character in UART and baud rate
#define CMD_MAX_SIZE 15
#define USART_BAUDRATE(BAUD_RATE) ((float)(64*F_CPU / (16*(float)BAUD_RATE))+0.5)


// Formula for calculating baud rate. See chapter 29.3.2.2.1, equation 2 in the datasheet.
#define TWI0_BAUD(F_SCL, T_RISE) ((((((float)F_CPU / (2*(float)F_SCL))) - 5 - ((float)F_CPU * T_RISE))) / 2)

// Defining 100kHZ for the SCL frequency 
#define I2C_SCL_FREQ 100000 

#define DIR_READ 1
#define DIR_WRITE 0

// Defining macros to check for various I2C errors

#define TWI_IS_CLOCKHELD() TWI0.MSTATUS & TWI_CLKHOLD_bm
#define TWI_IS_BUSERR() TWI0.MSTATUS & TWI_BUSERR_bm
#define TWI_IS_ARBLOST() TWI0.MSTATUS & TWI_ARBLOST_bm

//Deciding which messages shall be acknowledged and which not

#define CLIENT_NACK() TWI0.MSTATUS & TWI_RXACK_bm
#define CLIENT_ACK() !(TWI0.MSTATUS & TWI_RXACK_bm)

#define TWI_IS_BUSBUSY() ((TWI0.MSTATUS & TWI_BUSSTATE_BUSY_gc) == TWI_BUSSTATE_BUSY_gc)

//Macro for for ensuring proper timing between operations (essentially waiting for the bus to be OK).
#define TWI_WAIT() while (!((TWI_IS_CLOCKHELD()) || (TWI_IS_BUSERR()) || (TWI_IS_ARBLOST()) || (TWI_IS_BUSBUSY())))

//PWM signal function
void PWM();
void RuntimeStartup();
uint32_t readfanspeed(uint32_t pin);
volatile bool state = false;
volatile uint32_t RPM;

//i2c functions
static void TWI0_M_init(void);
static void I2C0_M_start(uint8_t addr, uint8_t dir);
static void I2C_M_write(uint8_t addr, uint8_t data);
static void I2C_M_read(uint8_t addr, uint8_t* data, uint8_t len);
void runPWM_auto(int temperatur);
void Temp_data(uint8_t * data);
void TEMP_i2c();

//usart menu functions
static void executeCommand(const char* command);
void menu_status(const char* command);
void menu_manual(const char* command);
void menu_auto(const char* command);

//usart functions
int USART_printChar(char c, FILE *stream);
static FILE USART_stream = FDEV_SETUP_STREAM(USART_printChar, NULL, _FDEV_SETUP_WRITE);
static void USART_init(void);
static void USART_sendChar(char c);
static void USART_sendString(const char* str);
uint8_t USART_read(void);

int level = 0;
int fan_lvl = 0;

ISR(TCB0_INT_vect){	//interrupts on caption of PWM signal
    RPM = readRPM();
    state = true;
    TCB0.INTFLAGS = TCB_CAPT_bm;
}

int main(void){ 
    USART_init();	//setup usart
    RuntimeStartup();	//setup runtime and reads from memory
    readRPM_init();	//setup capture signal
    TWI0_M_init();	//setup i2c communication
    _delay_ms(5000);	//waiting so user can connect serial monitor on laptop
	
    const int maxVal = 15; 		// Maximum length for input
    char command[CMD_MAX_SIZE] = {0};   // size of character in usart communication
    uint8_t index = 0;
	
    PWM();		//starts fan at low speed for checking that every fan works
    runPWM_auto(50);
    sei();  		//enables interrupts 

    //start menu instructions 
    USART_sendString("...Welcome to the main menu...\r\n");
    USART_sendString("you can choose to enter the STATUS, MANUAL and AUTOMATIC submenus\r\n");
    USART_sendString("a = auto, m = manual and s = status\r\n");
	
	while (1) {
        
        char c = (char)USART_read();
		if (c != '\r' && c != '\n'){
			if (index < maxVal){
				command[index++] = c;
			}
			else{
				USART_sendString("Input too long\r\n");
				index = 0;
			}
		}

		if (c == '\n'){
			command[index] = '\0';
			index = 0;
            
			executeCommand(command);
		}  
	}
}
 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						/* MENU SYSTEM */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Main menu: There is three options. submenu 1, 2 or 3. Each level is triggered by typing on a key and hit enter on keyboard.
Each case take you futher to a submenu. Every submenu has it's own funtion.
Level determine which submenu you are in. if level is 0, program is taking you back to the main menu.
*/
static void executeCommand(const char* command){
	/* level 0: main menu, level 1: submenu 1, level 2: submenu 2 og level 3: submenu 3 */
	switch (level){
			
		default: // main menu	
			if (strcmp(command, "s") == !1) {	
			    level = 1; // switch to submenu 1
			    USART_sendString("Entering status submenu...\r\n");
			    printf("choose Fan 0->7 with <fann>. <resetn> to to reset runtime. <BACK> to return to main menu \r\n");
			}
			
			else if (strcmp(command, "m") == !1) {
			    level = 2;
			    // switch to submenu 2
			    USART_sendString("Entering manual submenu...\r\n");
			    USART_sendString("choose Fan 0->7 with <fann>. <BACK> to return to main menu \r\n");
			}
			
			else if (strcmp(command, "a") == !1) {
			    level = 3;
			    // switch to submenu 3
			    USART_sendString("Entering automatic submenu...\r\n");
			}
			
			else {
			    USART_sendString("Invalid command\r\n");
			}
			break;

		case 1: // STATUS submenu
			if (level == 1) {
			    menu_status(command);
			}
			break;
			
		case 2: //submenu 2 / MANUAL
			if (level == 2) {
                	    menu_manual(command);	
			}
			break;
			
		case 3: //submenu 3 / AUTOMATIC
			if (level == 3) {
			    menu_auto(command);
			}
			break;
	}
}

/*
This function contains status for 8 fans. Each if statement selects one of the 8 fans. You can watch status or reset runtime.
In this function you can call status for every fan without going back in the submenu. This is more effective when looking at
multiple fans.
*/
void menu_status(const char* command) {
    if (strcmp(command, "fan0") == 0){
        uint16_t x = get_runtime_fan(0);
        uint16_t y = read_fantemperature(0);
        uint16_t z = readfanspeed(0);
        USART_sendString("\n");
        USART_sendString("Fan0:");
        USART_sendString("\n");
        printf("RUNTIME: %u\n", x);
        printf("TEMPERATURE: %u\n", y);
        printf("RPM: %u\n", z);
    }
    else if(strcmp(command, "fan1") == 0){
        uint16_t x = get_runtime_fan(1);
        uint16_t y = read_fantemperature(1);
        uint16_t z = readfanspeed(1);
        USART_sendString("\n");
        USART_sendString("Fan1:");
        USART_sendString("\n");
        printf("RUNTIME: %u\n", x);
        printf("TEMPERATURE: %u\n", y);
        printf("RPM: %u\n", z);
    }

    else if(strcmp(command, "fan2") == 0){
        uint16_t x = get_runtime_fan(2);
        uint16_t y = read_fantemperature(2);
        uint16_t z = readfanspeed(2);
        USART_sendString("\n");
        USART_sendString("Fan2:");
        USART_sendString("\n");
        printf("RUNTIME: %u\n", x);
        printf("TEMPERATURE: %u\n", y);
        printf("RPM: %u\n", z);
    }

    else if(strcmp(command, "fan3") == 0){
        uint16_t x = get_runtime_fan(3);
        uint16_t y = read_fantemperature(3);
        uint16_t z = readfanspeed(3);
        USART_sendString("\n");
        USART_sendString("Fan3:");
        USART_sendString("\n");
        printf("RUNTIME: %u\n", x);
        printf("TEMPERATURE: %u\n", y);
        printf("RPM: %u\n", z);
    }

    else if(strcmp(command, "fan4") == 0){
        uint16_t x = get_runtime_fan(4);
        uint16_t y = read_fantemperature(4);
        uint16_t z = readfanspeed(4);
        USART_sendString("\n");
        USART_sendString("Fan4:");
        USART_sendString("\n");
        printf("RUNTIME: %u\n", x);
        printf("TEMPERATURE: %u\n", y);
        printf("RPM: %u\n", z);
    }
    else if(strcmp(command, "fan5") == 0 ){
        uint16_t x = get_runtime_fan(5);
        uint16_t y = read_fantemperature(5);
        uint16_t z = readfanspeed(5);
        USART_sendString("\n");
        USART_sendString("Fan5:");
        USART_sendString("\n");
        printf("RUNTIME: %u\n", x);
        printf("TEMPERATURE: %u\n", y);
        printf("RPM: %u\n", z);
    }
    else if(strcmp(command, "fan6") == 0 ){
        uint16_t x = get_runtime_fan(6);
        uint16_t y = read_fantemperature(6);
        USART_sendString("\n");
        USART_sendString("Fan6:");
        USART_sendString("\n");
        printf("RUNTIME: %u\n", x);
        printf("TEMPERATURE: %u\n", y);
        printf("RPM: %u\n", 0);

    }
    else if(strcmp(command, "fan7") == 0 ){
        uint16_t x = get_runtime_fan(7);
        uint16_t y = read_fantemperature(7);
        USART_sendString("\n");
        USART_sendString("Fan7:");
        USART_sendString("\n");
        printf("RUNTIME: %u\n", x);
        printf("TEMPERATURE: %u\n", y);
        printf("RPM: %u\n", 0);

    }
    else if(strcmp(command, "reset0") == 0 ){
        reset_fan_runtime(0);
        printf("Fan 0 runtime reset\r\n");

    }
    else if(strcmp(command, "reset1") == 0){
        reset_fan_runtime(1);
        printf("Fan 1 runtime reset\r\n");
    }
    else if (strcmp(command, "reset2") == 0){
        reset_fan_runtime(2);
        printf("Fan 2 runtime reset\r\n");
    }
    else if (strcmp(command, "reset3") == 0){
        reset_fan_runtime(3);
        printf("Fan 3 runtime reset\r\n");
    }
    else if(strcmp(command, "reset4") == 0){
        reset_fan_runtime(4);
        printf("Fan 4 runtime reset\r\n");
    }
    else if (strcmp(command, "reset5") == 0){
        reset_fan_runtime(5);
        printf("Fan 5 runtime reset\r\n");
    }
    else if (strcmp(command, "reset6") == 0){
        reset_fan_runtime(6);
        printf("Fan 6 runtime reset\r\n");
    }
    else if (strcmp(command, "reset7") == 0){
        reset_fan_runtime(7);
        printf("Fan 7 runtime reset\r\n");
    }
    else if (strcmp(command, "BACK") == 0)
    {
        level = 0; // go back to main menu
        USART_sendString("Returning to Main Menu...\r\n");
        USART_sendString("Welcome to the main menu, you can choose to enter the STATUS, MANUAL and AUTOMATIC submenus\r\n");
        USART_sendString("a = auto, m = manual and s = status\r\n");
    }
    else
    {
    USART_sendString("Invalid command\r\n");
    }
}

/*
This function contains a switch case with 8 fans. You can controll only one fan at a time. But it is possible to go back, inside the function and 
select a new fan to controll. That way you can run multiple fans with different speed.
*/
void menu_manual(const char* command){
    PWM();
    switch(fan_lvl){
	default: 
		if (strcmp(command, "fan0") == 0) {
			printf("Choose manually the strength of fan 0 using the following commands: <high<, <medium>, <low> and BACK if you want back to the main menu\r\n");
			fan_lvl = 1;
		}
		else if (strcmp(command, "fan1") == 0) {
			printf("Choose manually the strength of fan 1 using the following commands: <high<, <medium>, <low> and BACK if you want back to the main menu\r\n");
			fan_lvl = 2;
		}
		else if (strcmp(command, "fan2") == 0) {
			printf("Choose manually the strength of fan 2 using the following commands: <high<, <medium>, <low> and BACK if you want back to the main menu\r\n");
			fan_lvl = 3;
		}
		else if (strcmp(command, "fan3") == 0) {
			printf("Choose manually the strength of fan 3 using the following commands: <high<, <medium>, <low> and BACK if you want back to the main menu\r\n");
			fan_lvl = 4;
		}
		else if (strcmp(command, "fan4") == 0) {
			printf("Choose manually the strength of fan 4 using the following commands: <high<, <medium>, <low> and BACK if you want back to the main menu\r\n");
			fan_lvl = 5;
		}
		else if (strcmp(command, "fan5") == 0) {
			printf("Choose manually the strength of fan 5 using the following commands: <high<, <medium>, <low> and BACK if you want back to the main menu\r\n");
			fan_lvl = 6;
		}
		else if (strcmp(command, "BACK") == 0) {
			level = 0;
			USART_sendString("Returning to Main Menu...\r\n");
			USART_sendString("...Welcome to the main menu...\r\n");
			USART_sendString("you can choose to enter the STATUS, MANUAL and AUTOMATIC submenus\r\n");
			USART_sendString("a = auto, m = manual and s = status\r\n");
		}
		else{
			USART_sendString("Invalid command\r\n");
		}
	case 1: 
		if (fan_lvl == 1) {
			if (strcmp(command, "high") == 0) {
				TCA0.SINGLE.CMP2 = 0x9F; // Duty cycle on PD2
				USART_sendString("Fan 0 Power HIGH\r\n");	
			}
			else if (strcmp(command, "medium") == 0) {
				TCA0.SINGLE.CMP2 = 0x9F/2; // Duty cycle on PD2
				USART_sendString("Fan 0 Power MEDIUM\r\n");
			}
			else if (strcmp(command, "low") == 0) {
				TCA0.SINGLE.CMP2 = 0x9F/5; // Duty cycle on PD2
				USART_sendString("Fan 0 Power LOW\r\n");
			}
            		else if (strcmp(command, "BACK") == 0) {
				fan_lvl = 0;
				USART_sendString("Choose which fan you want to control, commando <fann> eks: fan0\r\n");
            		}
			else{
				USART_sendString("Invalid command\r\n");
			}
       		}
		break;
		
		case 2:
		if (fan_lvl == 2) {
			if (strcmp(command, "high") == 0) {
				TCA0.SINGLE.CMP1 = 0x9F; // Duty cycle on PD1
				USART_sendString("Fan 1 Power HIGH\r\n");
			}
			else if (strcmp(command, "medium") == 0) {
				TCA0.SINGLE.CMP1 = 0x9F/2; // Duty cycle on PD1
				USART_sendString("Fan 1 Power MEDIUM\r\n");
			}
			else if (strcmp(command, "low") == 0) {
				TCA0.SINGLE.CMP1 = 0x9F/8; // Duty cycle on PD1
				USART_sendString("Fan 1 Power LOW\r\n");
			}
            		else if (strcmp(command, "BACK") == 0) {
                		fan_lvl = 0;
       				USART_sendString("Choose which fan you want to control, commando <fann> eks: fan0\r\n");

           		}
			else {
				USART_sendString("Invalid command\r\n");
			}
        	}
		break;
		
		case 3:
		if (fan_lvl == 3) {
			if (strcmp(command, "high") == 0) {
				TCA0.SINGLE.CMP0 = 0x9F; // Duty cycle on PD0
				USART_sendString("Fan 2 Power HIGH\r\n");
			}
			else if (strcmp(command, "medium") == 0) {
				TCA0.SINGLE.CMP0 = 0x9F/2; // Duty cycle on PD0
				USART_sendString("Fan 2 Power MEDIUM\r\n");
			}
			else if (strcmp(command, "low") == 0) {
				TCA0.SINGLE.CMP0 = 0x9F/8; // Duty cycle on PD0
				USART_sendString("Fan 2 Power LOW\r\n");
			}
            		else if (strcmp(command, "BACK") == 0) {
                		fan_lvl = 0;
       				USART_sendString("Choose which fan you want to control, commando <fann> eks: fan0\r\n");
           		}
			else {
				USART_sendString("Invalid command\r\n");
			}
        	}
		break;
        
		
		case 4:
		if (fan_lvl == 4) {
			if (strcmp(command, "high") == 0) {
				TCA1.SINGLE.CMP0BUF = 0x9F; // Duty cycle on PC4
				USART_sendString("Fan 3 Power HIGH\r\n");
			}
			else if (strcmp(command, "medium") == 0) {
				TCA0.SINGLE.CMP0BUF = 0x9F/2; // Duty cycle on PC4
				USART_sendString("Fan 3 Power MEDIUM\r\n");
			}
			else if (strcmp(command, "low") == 0) {
				TCA0.SINGLE.CMP0BUF = 0x9F/8; // Duty cycle on PC4
				USART_sendString("Fan 3 Power LOW\r\n");
			}
            		else if (strcmp(command, "BACK") == 0) {
               			fan_lvl = 0;
       				USART_sendString("Choose which fan you want to control, commando <fann> eks: fan0\r\n");
            		}
			else {
				USART_sendString("Invalid command\r\n");
			}
        	}
		break;
		
		case 5:
		if (fan_lvl == 5) {
			if (strcmp(command, "high") == 0) {
				TCA1.SINGLE.CMP1BUF = 0x9F; // Duty cycle on PC5
				USART_sendString("Fan 4 Power HIGH\r\n");
			}
			else if (strcmp(command, "medium") == 0) {
				TCA0.SINGLE.CMP1BUF = 0x9F/2; // Duty cycle on PC5
				USART_sendString("Fan 4 Power MEDIUM\r\n");
			}
			else if (strcmp(command, "low") == 0) {
				TCA0.SINGLE.CMP1BUF = 0x9F/8; // Duty cycle on PC5
				USART_sendString("Fan 4 Power LOW\r\n");
			}
            		else if (strcmp(command, "BACK") == 0) {
                		fan_lvl = 0;
       				USART_sendString("Choose which fan you want to control, commando <fann> eks: fan0\r\n");
            		}
			else {
				USART_sendString("Invalid command\r\n");
			}
        	}
		break;
		
		case 6:
		if (fan_lvl == 6) {
			if (strcmp(command, "high") == 0) {
				TCA1.SINGLE.CMP2BUF = 0x9F; // Duty cycle on PC6
				USART_sendString("Fan 5 Power HIGH\r\n");
			}
			else if (strcmp(command, "medium") == 0) {
				TCA0.SINGLE.CMP2BUF = 0x9F/2; // Duty cycle on PC6
				USART_sendString("Fan 5 Power MEDIUM\r\n");
			}
			else if (strcmp(command, "low") == 0) {
				TCA0.SINGLE.CMP2BUF = 0x9F/8; // Duty cycle on PC6
				USART_sendString("Fan 5 Power LOW\r\n");
			}
            		else if (strcmp(command, "BACK") == 0) {
                		fan_lvl = 0;
       				USART_sendString("Choose which fan you want to control, commando <fann> eks: fan0\r\n");
            		}
			else {
				USART_sendString("Invalid command\r\n");
			}
        	}
            	break;
	}
}

/*
This function starts automatic control of fans. It reads values from i2c temperature sensor, and controls pwm output.
Every time function TEMP_i2c runs, temperature and fanspeed in prosent are shown in the menu.
*/
void menu_auto(const char* command){
    
	if(strcmp(command, "auto") == 0){ 					
		USART_sendString("Fan is now going automatically\r\n");
		TEMP_i2c(); //running fans from temperature 
	}
    
	else if (strcmp(command, "BACK") == 0) {
		level = 0; // go back to main menu
		USART_sendString("Returning to Main Menu...\r\n");
		USART_sendString("...Welcome to the main menu...\r\n");
		USART_sendString("you can choose to enter the STATUS, MANUAL and AUTOMATIC submenus\r\n");
		USART_sendString("a = auto, m = manual and s = status\r\n");
	}
	
    	else {
		USART_sendString("Invalid command\r\n");
	}   
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						/* PWM functions */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 
/*
This function declares which pins shall be used for PWM
It also uses TCA to make the function create a PWM signal
*/
void PWM(){
	 //Pinout
    PORTD.DIRSET = PIN0_bm | PIN1_bm | PIN2_bm; // Enabling PD0, PD1 and PD2 as outputs
    PORTC.DIRSET = PIN6_bm | PIN5_bm | PIN4_bm; // Enabling PC6, PC5 and PC4 as outputs
    PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTD_gc | PORTMUX_TCA1_PORTC_gc;  // Enabling PWM output on Port D and C.
    TCA0.SINGLE.PER = 0x9F;
    TCA1.SINGLE.PER = 0x9F;
    
    // Enabling output on PWM pin 2 on Port D and PWM pin 0 and 1 on Port D, sets the timer to single slope mode.
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
    TCA1.SINGLE.CTRLB = TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
    //Formula: F_CPU/(TCA_SINGLE_CLKSEL_DIVn_gc*(TCA0.SINGLE.PER + 1)) = PWM frequency
	
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm; // Turning the timer on with pre-scaler set to 1
	TCA1.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;// Turning the timer on with pre-scaler set to 1
	
}
void runPWM_auto(int temperatur){
    
    uint8_t pwm_out = analogTempvalue(temperatur, 30, 100);        //Temperature, minimum, maximum
    printf("fanProsent: %u\n", map(pwm_out, 0, 159, 0, 100));
    pwm_ut(pwm_out);//TemperaturRead); 
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						/* I2c functions */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void TWI0_M_init(void) {
	
	 PORTA.DIRSET = PIN2_bm | PIN3_bm;
	 PORTA.PINCONFIG = PORT_PULLUPEN_bm;
	 PORTA.PINCTRLUPD = PIN2_bm | PIN3_bm;
	
	//TWI0.CTRLA = TWI_SDAHOLD_50NS_gc; //Setting hold time to 50 NS.
	
	TWI0.MCTRLA = TWI_ENABLE_bm; //Enable TWI/I2C as controller.
	
	TWI0.MBAUD	= (uint8_t)TWI0_BAUD(I2C_SCL_FREQ,0); //Configure TWI baud rate.
	
	TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc; //Setting the I2C bus to idle.
}

/*
   This function starts a data transfer with another device on the specified address.
   addr - is the address of the device on the bus.
   dir  - specifies direction of transfer. 1 is read, 0 is write.
 */
static void I2C0_M_start(uint8_t addr, uint8_t dir) {
	
	/*Specify read or write of client */
	TWI0.MADDR = (addr << 1) | dir;
	
	TWI_WAIT();
}

//Fuction writes one byte to the specified device
static void I2C_M_write(uint8_t addr, uint8_t data) {
	
	I2C0_M_start(addr, DIR_WRITE);
	
	TWI0.MDATA = data;
	
	TWI_WAIT();
	
	//Check for NACK
	if(TWI0.MSTATUS & TWI_RXACK_bm) {
		//target is full
		printf("target NACK\n");
	}
	
	//Issue a stop condition
	TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}

/*
  This function reads len bytes to the device on the specified address.
  addr - is the address of the device on the bus.
  data - is the pointer to data array. 
  len  - is the number of bytes to be read.
 */
static void I2C_M_read(uint8_t addr, uint8_t* data, uint8_t len) {
	I2C0_M_start(addr, DIR_READ);
	uint8_t byte_count = 0;
    
	while(byte_count < len) {
		TWI_WAIT();
		
		data[byte_count] = TWI0.MDATA;
		
		byte_count++;
		
		if(byte_count != len) {
			TWI0.MCTRLB = TWI_ACKACT_ACK_gc 
						| TWI_MCMD_RECVTRANS_gc;
		}
	}
	/*Finish transaction with a NACK and stop condition */
	TWI0.MCTRLB = TWI_ACKACT_NACK_gc | TWI_MCMD_STOP_gc;
     
}

/*
This function uses an array to collect data from i2c sensor.
When communication with the sensor is established, and array is filled with data.
Temp_data funcion is collecting the right data amount. 
*/
void TEMP_i2c(){ 
    uint8_t data[7];
    I2C_M_write(0x38,0xAC);
    _delay_ms(1000);
    I2C_M_read(0x38, data, 0xAC);
	data[6] = '\0';
    _delay_ms(1000);

    Temp_data(data);
}
/*
From the data array a defined bitplacement is used for temperature.  This function collects temperature from those bits, and
calculate the temperature.
equation "temperature" is found in AHT10 datasheet
temperatur is sent to 
*/
void Temp_data(uint8_t * data){ 
	uint8_t high = (data[5]>>4) & 0x0F;
	uint8_t low = data[3] & 0x0F;
    
	uint16_t raaTemperatur = (low<<12| data[4] << 4 |high);
	float temperatur = (((float)raaTemperatur/65536)*200)-50;
    printf("temp: %.2f\n", temperatur);
    runPWM_auto((int)temperatur);    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						/* USART functions */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/*
This function configures the baud rate
it also declares RX and TX.
*/
static void USART_init(void){
	PORTB.DIRCLR = PIN1_bm;
	PORTB.DIRSET = PIN0_bm;
	USART3.BAUD = (uint16_t)USART_BAUDRATE(9600);
	USART3.CTRLB |= USART_TXEN_bm | USART_RXEN_bm;
	stdout = &USART_stream;
}

/*
This function makes it possible to send a char over USART
*/
static void USART_sendChar(char c){
	while (!(USART3.STATUS & USART_DREIF_bm))
	{
		;
	}

	USART3.TXDATAL = c;
}

/*
This function makes it possible to send a string over USART
*/
static void USART_sendString(const char* str){
	for (size_t i = 0; i < strlen(str); i++)
	{
		USART_sendChar(str[i]);
	}
}
/*
This function reads recieved char from USART
*/
uint8_t USART_read(void){
	while (!(USART3.STATUS & USART_RXCIF_bm))
	{
		;
	}
	return USART3.RXDATAL;
}

// Wrapping USART_sendChar in a function computable with FDEV_SETUP_STREAM
int USART_printChar(char c, FILE *stream) {
    USART_sendChar(c);
    return 0; 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						/* Other functions */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
This function reads 
*/
uint32_t readfanspeed(uint32_t pin){
    RPMpin(pin); 
    uint32_t speed;
    if(state == true){
        speed = RPM;
        state = false;
    }
    else{
        speed = 0;
    }
    return speed;
}

/*
This function starts the runtime of the fans
*/
void RuntimeStartup(){
    RTC_init(); //Starts counter
    fan_memory();   //reads stored fan runtime from eeprom
}
