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

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern ADC_HandleTypeDef hadc1;
extern FlagStatus mainDrvRunning;

static uint16_t adc_result[3];
static uint16_t speed_percent;
static int16_t spin_percent;
static uint16_t angle_degree;
static uint16_t last_adc[3];
static uint32_t last_rand_tick;
static uint32_t last_blink_tick;

uint32_t last_angle_change;

int pgm_stop(void){

	if(mainDrvRunning){

		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);

		mainDrvRunning = RESET;
	}

	uint8_t text_off[] = {SEG7_0, SEG7_F, SEG7_F};

	seg7_displayOnOffMulti(SPIN | ANGLE, 0);
	seg7_displayOnOffMulti(SPEED, 1);
	seg7_setDispAddr(SPEED);
	seg7_display(text_off);

	if((HAL_GetTick() - last_blink_tick) > BLINK_INT_MS){

			Toggle_Led_Output(GREEN);

			last_blink_tick = HAL_GetTick();
		}

	return EXIT_SUCCESS;
}

int pgm_manual(void){

	Set_Led_Output(GREEN);
	seg7_displayOnOffMulti(SPEED | SPIN | ANGLE, 1);

	get_adc_values(adc_result);

	speed_percent = adc_result[0]*100/0xFFF;
	spin_percent = (adc_result[1]*100/0xFFF)-50;
	angle_degree = (adc_result[2]*90/0xFFF);

	seg7_displayIntMulti(speed_percent, spin_percent, angle_degree);

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
	seg7_displayOnOffMulti(SPEED | SPIN, 0);
	seg7_displayOnOffMulti(ANGLE, 1);

	get_adc_values(adc_result);

	angle_degree = (adc_result[2]*90/0xFFF);

	seg7_setDispAddr(ANGLE_ADDR);
	seg7_displayInt((int) angle_degree);

	if(abs(last_adc[2] - adc_result[2]) > MIN_ANGLE_DELTA){
		last_adc[2] = adc_result[2];
		set_pos_posdrv(angle_degree);
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
	seg7_displayOnOffMulti(SPEED | SPIN | ANGLE, 0);

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
