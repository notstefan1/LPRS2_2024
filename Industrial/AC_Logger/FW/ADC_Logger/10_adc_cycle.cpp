// - Board: Arduino Uno
// - Shield: AC logger

#include <Arduino.h>

#include "avr_io_bitfields.h"

#define MAGIC 0xBABADEDA

struct sample_packet
{
	uint32_t magic;
	uint32_t id;
	uint16_t val_array[6];
	uint8_t err;

} __attribute__((packed));

volatile bool to_send = false;
volatile u8 mux_select = 0;
volatile sample_packet pack = {
	.magic = MAGIC,
	.id = 0,
	.val_array = {0},
	.err = 0
};

static void print_reg(const char *name, u8 r) {
	Serial.print(name);
	Serial.print(" = 0b");
	for (int i = 7; i >= 0; i--)
	{
		if ((r >> i) & 1)
		{
			Serial.print('1');
		}
		else
		{
			Serial.print('0');
		}
		if (i == 4)
		{
			Serial.print('_');
		}
	}
	Serial.println();
}

static void set_adc_mux(u8 mux)
{
	set_bf(adc.g.admux, {
		.mux = mux,
		.adlar = 0,	 // Left align ADC value to 8 bits from ADCH register.
		.refs = 0b01 // Vcc TODO
	});
}

ISR(TIMER1_COMPA_vect) {
	// Take prev sample.
	u16 sample = adc.r.adcw;

	pack.val_array[mux_select] = sample;

	// Steer to next.
	mux_select++;
	set_adc_mux(mux_select);
	if (mux_select == 6){
		mux_select = 0;
		to_send = true;
	}

	// Start new sample.
	adc.f.adsc = 1;
}

void setup() {

	Serial.begin(1000000);

	// ADC setup.

	// Disable digital buffer on analog input.
	adc.r.didr = 0xff;

	set_adc_mux(mux_select);

	adc.g.adcsrb.adts = 0; // Free Running mode

	set_bf(adc.g.adcsra, {
		// f_adc_in = 38.5 kHz
		// f_smpl_max = 77 kHz
		// f_smpl = 9.6MHz/16/13 = 46.154 kHz
		// f_in = 46.154/2 = 23 kHz
		.adps = 0b100, // 16 prescaler.
		.adie  = 0, // No IRQ.
		.adif  = 0, // Clear IRQ.
		.adate = 0, // Auto trigger disabled.
		.adsc = 1, // Warm up ADC, because first sampling is 25 CLKs.
		.aden = 1 // Enable ADC.
	});


#if 0
	Serial.begin(1000000);
	print_reg("ADCSRA", ADCSRA);
	print_reg("ADMUX", ADMUX);
	print_reg("ADCSRB", ADCSRB);
	//print_reg("ADCSRA", ADCSRA);
	print_reg("ADCL", ADCL);
	print_reg("ADCH", ADCH);
	Serial.println();
#endif


	// TODO Nicer: use structs. <- No.
	cli();
	TCCR1A = 0;		// 0 - 0xffff
	TCCR1B = 0b001; // clk/no prescaler

	TCCR1B |= (1 << WGM12); // CTC mode.

	// output compare register, max Timer/Counter value
	// TODO Defines and explanation for these numbers.
	OCR1A = F_CPU / (50 * 32 * 2) * 8;

	TIMSK1 |= (1 << OCIE1A); // IRQ      //TIMSK1- timer interrupt mask register
	sei();					 // set global interrupt enabled
}

void loop() {
	if(to_send){
		to_send = false;

		pack.err = false;

		if(to_send){
			pack.err = true;
		}

		pack.id++;
		Serial.write((uint8_t *)&pack, sizeof(sample_packet));
	}
}
