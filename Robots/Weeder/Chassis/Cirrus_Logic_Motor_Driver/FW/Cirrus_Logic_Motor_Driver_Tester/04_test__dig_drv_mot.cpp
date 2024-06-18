
#include "common.h"


int main(void) {

	dig_drv.ddr_out0 = 0xff;
	dig_drv.ddr_out1 = 0xff;
	dig_drv.ddr_in0 = 0;
	dig_drv.ddr_in1 = 0;

	dig_drv.n_en = 0; // #EN active.
	
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
		if(v){
			dig_drv.vh = 1;
		}else{
			dig_drv.vl = 1;
		}
		if(w){
			dig_drv.wh = 1;
		}else{
			dig_drv.wl = 1;
		}
	};

	while(1){ //infinite loop

		_delay_ms(6); //off
		u = 1;
		edge();
		_delay_ms(2); //on
		u = 0;
		edge();
		

		
	}

}
