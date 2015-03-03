/*
 * rc_outputs.c
 *
 *  Created on: Feb 26, 2015
 *      Author: ficeto
 */
#include "rc_outputs.h"

#define RC_OUTNUM 4
static uint16_t rc_outputs[4] = {1500,1500,1500,1500};
static uint8_t rc_out_pins[4] = {13,12,14,16};

void onRcTimer(){
	static uint8_t rc_current_chan = 0;
	static uint16_t rc_loop_time = 0;
	if(rc_current_chan != RC_OUTNUM){
		uint16_t period = rc_outputs[rc_current_chan];
		timer1_write(period * 5);
		if(rc_current_chan != 0) digitalWrite(rc_out_pins[rc_current_chan - 1], LOW);
		digitalWrite(rc_out_pins[rc_current_chan], HIGH);
		rc_loop_time += period;
		rc_current_chan++;
	} else {
		timer1_write((10000 - rc_loop_time) * 5);
		digitalWrite(rc_out_pins[3], LOW);
		rc_loop_time = 0;
		rc_current_chan = 0;
	}
}

void rcOutputsInit(){
	int i;
	for(i=0; i<RC_OUTNUM; i++){
		pinMode(rc_out_pins[i], OUTPUT);
		digitalWrite(rc_out_pins[i], LOW);
	}
	//updates the screen every second
	timer1_disable();
	timer1_attachInterrupt(onRcTimer);
	timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
	timer1_write(5000000);//1 second
}

void rcOutputsWrite(uint8_t chan, uint16_t value){
	if(value < 700) value = 700;
	else if(value > 2200) value = 2200;
	rc_outputs[chan&3] = value;
}
