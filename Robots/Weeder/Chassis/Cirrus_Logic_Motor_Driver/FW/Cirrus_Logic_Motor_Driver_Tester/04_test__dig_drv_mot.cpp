
#include "common.h"


#define PWM_PERIOD 10000 //nanoseconds
#define DUTY 0.9
#define GUARD 50

int counterR = 0;
int counterL = 0;
int angleL = 0;
int angleR = 0;

ISR(PCINT1_vect) {
    bool bela = (PINC & (1 << PC1));
    bool zelena = (PINC & (1 << PC0));  

    if (bela) {
        if (zelena) {
			if(angleL >= GUARD) angleR = 0;
			angleL++;
			if(angleL >= 1000) {
				counterL++;
				angleL = 0;
				Serial.println(counterL);
			}
        } else {
            if(angleR >= GUARD) angleL = 0;
			angleR++;
			if(angleR >= 1000) {
				counterR++;
				angleR = 0;
				Serial.println(counterR);
			}
        }
    }
}


int main(void) {
	Serial.begin(115200);

	dig_drv.ddr_out0 = 0xff;
	dig_drv.ddr_out1 = 0xff;
	dig_drv.ddr_in0 = 0;
	dig_drv.ddr_in1 = 0;

	dig_drv.pc = 0x03; // input pullup

	dig_drv.n_en = 0; // #EN active.

	PCICR |= (1 << PCIE1);
    PCMSK1 |= (1 << PCINT9);
    sei();
	
	uint8_t u = 0;
	uint8_t v = 0;
	uint8_t w = 0;

	auto edge = [&](){
		// Turn off all transistors.
		dig_drv.ul = 0;
		dig_drv.uh = 0;
		dig_drv.vl = 0;
		dig_drv.vh = 0;
		dig_drv.wl = 0;
		dig_drv.wh = 0;
		// Dead time.
		_delay_us(1); // 1 us.
		// Turn low or high transistor;
		if(u){
			dig_drv.uh = 1;
		}else{
			dig_drv.ul = 1;
		}
		
	}; 

	while(1){ //infinite loop
		_delay_us(PWM_PERIOD * DUTY); 
		u = 1;
		edge(); 
		_delay_us(PWM_PERIOD * (1 - DUTY));
		u = 0;
		edge();
	}
}
