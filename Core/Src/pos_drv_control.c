/*
 * pos_drv_control.c
 *
 *  Created on: May 29, 2024
 *      Author: floro
 */

//TIM4 = CCW Direction
//TIM2 = CW Direction

#include <stdint.h>
#include <stdlib.h>
#include "main.h"
#include "pos_drv_control.h"

uint16_t actualPosdDeg;	//Actual pos of drive in decaDeg, relative to home
static uint16_t msPerdDegCw;	//Millisecs per degree for clockwise side
static uint16_t msPerdDegCcw;	//Millisecs per degree for counterclockwise side

int set_pos_posdrv(uint16_t angle_degree){

	/*Check if drive is running, if true get current reg state and take it as correction value for actual value
	 * calc actual value by using the correction from register (remaining pulse duration corrected by dir)
	 * Calc the new target val based on target degree value.
	 * Calc new pulse duration based on target value, actual value and direction
	 * fire pulse set moving flag to true
	 * TODO add callback to reset flag
	 */
	int16_t correction_value = 0;

	if(posDrvRunning){
		if(posDrvDir == 1){
			HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_2);
			correction_value = posDrvDir*((TIM2->CNT) - PULSE_DELAY)/msPerdDegCw;
		}
		else{
			HAL_TIM_PWM_Stop_IT(&htim4, TIM_CHANNEL_2);
			correction_value = posDrvDir*((TIM4->CNT) - PULSE_DELAY)/msPerdDegCcw;
		}

		posDrvRunning = RESET;
	}

	actualPosdDeg += correction_value;
	int16_t deltadDeg = (angle_degree*10) - actualPosdDeg;

	if(deltadDeg == 0) return EXIT_SUCCESS;

	if(deltadDeg > 0){
		TIM2->CNT = 0;
		TIM2->CCR2 = PULSE_DELAY;
		TIM2->ARR = (deltadDeg*msPerdDegCw) + PULSE_DELAY;
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
		posDrvDir = 1;
	}
	else {
		TIM4->CNT = 0;
		TIM4->CCR2 = PULSE_DELAY;
		TIM4->ARR = -(deltadDeg*msPerdDegCcw) + PULSE_DELAY;
		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
		posDrvDir = -1;
	}

	actualPosdDeg += deltadDeg;

	posDrvRunning = SET;

	return EXIT_SUCCESS;
}

int init_home_pos_drive(){

	/*Move drive towards SW0 until flag = true, set pos = 0, move towards SW1 until flag = true
	 * stop time from SW0 to SW1, move back to SW0 stop time from SW1 to SW0.
	 * Based on timing calc ms per degree (slower time) and calc correction factor (slow/fast)
	 * TODO test EXTIS for switches SW0 and SW1
	 */
	uint16_t cwTimeMs;
	uint16_t ccwTimeMs;

	home_pos_drive();

	TIM2->ARR = 20000;
	TIM2->CCR2 = PULSE_DELAY;
	uint32_t startTime = HAL_GetTick();
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

	while(!endPos){
		//Wait until SW2 is reached, maybe check SW in here?
	}

	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
	cwTimeMs = HAL_GetTick() - startTime;

	TIM4->ARR = 20000;
	TIM4->CCR2 = PULSE_DELAY;
	startTime = HAL_GetTick();
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);

	while(!startPos){
		//Wait until SW1 is reached, maybe check SW in here?
	}

	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
	ccwTimeMs = HAL_GetTick() - startTime;

	msPerdDegCw = cwTimeMs/900;
	msPerdDegCcw = ccwTimeMs/900;
	initHomingComplete = SET;
	TIM2->CNT = 0;
	TIM4->CNT = 0;

	return EXIT_SUCCESS;
}

int home_pos_drive(void){

	if(startPos) return EXIT_SUCCESS;

	TIM4->ARR = 20000;
	TIM4->CCR2 = PULSE_DELAY;
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);

	while(!startPos){
		//Wait until SW1 is reached, maybe check SW in here?
	}

	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
	actualPosdDeg = 0;
	homingComplete = SET;

	return EXIT_SUCCESS;
}
