/*
 * machine_programs.c
 *
 *  Created on: May 16, 2024
 *      Author: floro
 */
#include "main_drv_control.h"
#include "machine_programs.h"
#include "pos_drv_control.h"
#include "main.h"
#include "ht16k33.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

static uint16_t adc_result[3];		//Array to store the raw ADC results
static uint16_t speed_percent;		//Speed converted to percent
static int16_t spin_percent;		//Spin converted to percent
static uint16_t angle_degree;		//Angle converted to degrees
static uint16_t last_adc[3];		//Array to store the last valid ADC readings
static uint32_t last_rand_tick;		//Timestamp when last random value generation takes place
static uint32_t last_blink_tick;	//Timestamp for blinking LED

uint32_t last_angle_change;

int pgm_stop(void){

	if(mainDrvRunning){

		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);

		mainDrvRunning = RESET;
	}

	uint8_t text_off[] = {SEG7_0, SEG7_F, SEG7_F, SEG7_SPACE};

	seg7_displayOnOffMulti(SPEED);
	seg7_display(text_off, SPEED_ADDR);

	if((HAL_GetTick() - last_blink_tick) > BLINK_INT_MS){

			Toggle_Led_Output(GREEN);

			last_blink_tick = HAL_GetTick();
		}

	return EXIT_SUCCESS;
}

int pgm_manual(void){

	Set_Led_Output(GREEN);
	seg7_displayOnOffMulti(SPEED | SPIN | ANGLE);

	get_adc_values(adc_result);

	speed_percent = adc_result[0]*100/4022;
	spin_percent = (adc_result[1]*100/4022)-50;
	angle_degree = (adc_result[2]*90/4022);

	seg7_displayInt((int16_t)speed_percent, SPEED_ADDR);
	seg7_displayInt((int16_t)spin_percent, SPIN_ADDR);
	seg7_displayInt((int16_t)angle_degree, ANGLE_ADDR);

	if(
		(abs(last_adc[0] - adc_result[0]) > MIN_SPEED_DELTA) |
		(abs(last_adc[1] - adc_result[1]) > MIN_SPEED_DELTA)){

			last_adc[0] = adc_result[0];
			last_adc[1] = adc_result[1];
			set_pwm_maindrv(speed_percent, spin_percent, htim1);
		}

	if(abs(last_adc[2] - adc_result[2]) > MIN_ANGLE_DELTA){
		last_adc[2] = adc_result[2];
		set_pos_posdrv(angle_degree);
		last_angle_change = HAL_GetTick();
	}

	if(!mainDrvRunning){
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);

		mainDrvRunning = SET;
	}

	return EXIT_SUCCESS;
}

int pgm_auto_speed(void){

	Set_Led_Output(GREEN);

	uint8_t text_auto[] = {SEG7_A, SEG7_U, SEG7_T, SEG7_0};

	seg7_displayOnOffMulti(SPEED | SPIN | ANGLE);
	seg7_display(text_auto, SPEED_ADDR);
	seg7_display(text_auto, SPIN_ADDR);

	get_adc_values(adc_result);

	angle_degree = (adc_result[2]*90/4022);

	seg7_displayInt((uint16_t) angle_degree, ANGLE_ADDR);

	if(abs(last_adc[2] - adc_result[2]) > MIN_ANGLE_DELTA){
		last_adc[2] = adc_result[2];
		set_pos_posdrv(angle_degree);
		last_angle_change = HAL_GetTick();
	}

	if((HAL_GetTick() - last_rand_tick) > AUTO_DELAY * 1000){

			uint16_t rand_speed = rand() % 101;
			int16_t rand_spin = (rand() % 101) -50;

			set_pwm_maindrv(rand_speed, rand_spin, htim1);

			last_rand_tick = HAL_GetTick();
		}

	return EXIT_SUCCESS;
}

int pgm_auto(void){

	Set_Led_Output(GREEN);

	uint8_t text_auto[] = {SEG7_A, SEG7_U, SEG7_T, SEG7_0};

	seg7_displayOnOffMulti(SPEED | SPIN | ANGLE);
	seg7_display(text_auto, SPEED_ADDR);
	seg7_display(text_auto, SPIN_ADDR);
	seg7_display(text_auto, ANGLE_ADDR);

	if((HAL_GetTick() - last_rand_tick) > AUTO_DELAY * 1000){

		uint16_t rand_speed = rand() % 101;
		int16_t rand_spin = (rand() % 101) -50;
		uint16_t rand_angle = rand() % 91;

		set_pwm_maindrv(rand_speed, rand_spin, htim1);
		set_pos_posdrv(rand_angle);

		last_rand_tick = HAL_GetTick();
	}

	return EXIT_SUCCESS;
}

int get_adc_values(uint16_t* adc_result){

	for(int i = 0; i < 3; i++){
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 1);
			adc_result[i] = HAL_ADC_GetValue(&hadc1);
		}

	return EXIT_SUCCESS;
}
