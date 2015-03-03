#include "espino.h"
#include "stdlib.h"

uint32_t pwm_mask = 0;
uint16_t pwm_values[17] = {0,};

uint32_t pwm_multiplier = 0;
uint16_t pwm_steps[17];
uint8_t pwm_steps_len = 0;
uint32_t pwm_steps_mask[17];

int pwm_sort_asc(const void* a, const void* b){
     return (*((uint16_t*)a) > *((uint16_t*)b)) - (*((uint16_t*)a) < *((uint16_t*)b));
}

int pwm_sort_array(uint16_t a[], uint16_t al){
	qsort(a, al, sizeof(uint16_t), pwm_sort_asc);
	int i;
	int bl = 1;
	for(i = 1; i < al; i++){
		if(a[i] != a[i-1]) a[bl++] = a[i];
	}
	return bl;
}

uint32_t pwm_get_mask(uint16_t value){
	uint32_t mask = 0;
	int i;
	for(i=0; i<17; i++){
		if((pwm_mask & (1 << i)) != 0 && pwm_values[i] == value) mask |= (1 << i);
	}
	return mask;
}

void prep_pwm_steps(){
	if(pwm_mask == 0){
		pwm_steps_len = 0;
		return;
	}

	int pwm_temp_steps_len = 0;
	uint16_t pwm_temp_steps[17];
	uint32_t pwm_temp_masks[17];

	int i;
	for(i=0; i<17; i++){
		if((pwm_mask & (1 << i)) != 0 && pwm_values[i] != 0) pwm_temp_steps[pwm_temp_steps_len++] = pwm_values[i];
	}
	pwm_temp_steps[pwm_temp_steps_len++] = PWMRANGE;
	pwm_temp_steps_len = pwm_sort_array(pwm_temp_steps, pwm_temp_steps_len) - 1;
	for(i=0; i<pwm_temp_steps_len; i++){
		pwm_temp_masks[i] = pwm_get_mask(pwm_temp_steps[i]);
	}
	for(i=pwm_temp_steps_len; i>0; i--){
		pwm_temp_steps[i] = pwm_temp_steps[i] - pwm_temp_steps[i-1];
	}
	ETS_FRC1_INTR_DISABLE();
	pwm_steps_len = pwm_temp_steps_len;
	os_memcpy(pwm_steps, pwm_temp_steps, (pwm_temp_steps_len + 1) * 2);
	os_memcpy(pwm_steps_mask, pwm_temp_masks, pwm_temp_steps_len * 4);
	pwm_multiplier = 80000000/(PWMRANGE * PWMFREQ);
	ETS_FRC1_INTR_ENABLE();
}

void pwm_timer_isr(){
	static uint8_t current_step = 0;
	static uint8_t stepcount = 0;
	static uint16_t steps[17];
	static uint32_t masks[17];
	if(current_step < stepcount){
		GPOC = masks[current_step] & 0xFFFF;
		if(masks[current_step] & 0x10000) GP16O &= ~1;
		current_step++;
		timer1_write(pwm_steps[current_step] * pwm_multiplier);
	} else {

		current_step = 0;
		stepcount = 0;
		if(pwm_mask == 0) return;
		GPOS = pwm_mask & 0xFFFF;
		if(pwm_mask & 0x10000) GP16O |= 1;
		timer1_write(pwm_steps[0] * pwm_multiplier);
		stepcount = pwm_steps_len;
		memcpy(steps, pwm_steps, (stepcount + 1) * 2);
		memcpy(masks, pwm_steps_mask, stepcount * 4);
	}
}

void pwm_start_timer(){
	timer1_disable();
	timer1_attachInterrupt(pwm_timer_isr);
	timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
	timer1_write(1);
}

void analogWrite(uint8_t pin, uint16_t value){
	bool start_timer = false;
	if(value == 0){
		pwm_mask &= ~(1 << pin);
		prep_pwm_steps();
		digitalWrite(pin, LOW);
		if(pwm_mask == 0) timer1_disable();
		return;
	}
	if((pwm_mask & (1 << pin)) == 0){
		if(pwm_mask == 0) start_timer = true;
		pwm_mask |= (1 << pin);
		pinMode(pin, OUTPUT);
		digitalWrite(pin, LOW);
	}
	pwm_values[pin] = value % (PWMRANGE + 1);
	prep_pwm_steps();
	if(start_timer){
		pwm_start_timer();
	}
}
