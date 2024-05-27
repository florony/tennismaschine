/*
 * machine_programs.c
 *
 *  Created on: May 16, 2024
 *      Author: floro
 */
#include "machine_programs.h"
#include "main.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

static TIM_HandleTypeDef htim1;
static ADC_HandleTypeDef hadc1;

volatile uint16_t adc_result[3];
uint8_t adc_conv_complete_flag = 0;

int home_pos_drive(void){
	//TODO Homing the Position drive
	return EXIT_SUCCESS;
}

void pgm_stop(void){

	htim1.Instance = TIM1;
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);

	return;
}

void pgm_manual(void){

	hadc1.Instance = ADC1;
	for(int i = 0; i < 3; i++){
		printf("Start\n");
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1);
		adc_result[i] = HAL_ADC_GetValue(&hadc1);
	}

	uint16_t speed = adc_result[0];
	uint16_t spin = adc_result[1];
	uint16_t angle = adc_result[2];

	printf("speed: %d \n", speed);
	printf("spin: %d \n", spin);
	printf("angle: %d \n", angle);

	htim1.Instance = TIM1;
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);

	return;
}

void pgm_auto_speed(void){
	//TODO: generate random values for speed, set outputs

	return;
}

void pgm_auto(void){
	//TODO: generate random values for all, set outputs

	return;
}
