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

enum drvDir {
	stop = 0,
	cw = -1,
	ccw = 1
};

int16_t actualPosdDeg;			//Actual position of drive in decaDeg, relative to home
static uint16_t msPerdDegCw;	//Milliseconds per decaDeg for clockwise side
static uint16_t msPerdDegCcw;	//Milliseconds per decaDeg for counterclockwise side

/**
 * @brief: Set the position drive to the given value.
 *
 * @param: uint16_t angle_degree: target position in degrees
 *
 * @return: int 0 on success 1 on failure
 *
 * @detail: if drive is running, the drive is stopped and the remaining time of the pulse is calculated.
 * Based on the direction, the actual position is corrected. Pulse duration is calculated and written to the register.
 * Drive is started in desired direction an new actual position is set.
 */

int set_pos_posdrv(uint16_t angle_degree){

	static enum drvDir posDrvDir = stop;
	int16_t correction_value = 0;

	if(posDrvDir == cw){
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
		if(TIM2->CNT != 0){
			correction_value = posDrvDir*(int16_t)((TIM2->ARR - TIM2->CNT)/msPerdDegCw);
			TIM2->CNT = 0;
		}
	}
	else if(posDrvDir == ccw){
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
		if(TIM4->CNT != 0){
			correction_value = posDrvDir*(int16_t)((TIM4->ARR - TIM4->CNT)/msPerdDegCcw);
			TIM4->CNT = 0;
		}
	}

	actualPosdDeg += correction_value;
	int16_t deltadDeg = (angle_degree*10) - actualPosdDeg;

	if(deltadDeg == 0) return EXIT_SUCCESS;

	if(deltadDeg > 0){
		if(endPos) return EXIT_FAILURE;
		TIM2->CNT = 0;
		TIM2->CCR2 = PULSE_DELAY;
		TIM2->ARR = (deltadDeg*msPerdDegCw) + PULSE_DELAY;
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
		posDrvDir = cw;
	}
	else {
		if(startPos) return EXIT_FAILURE;
		TIM4->CNT = 0;
		TIM4->CCR2 = PULSE_DELAY;
		TIM4->ARR = abs(deltadDeg)*msPerdDegCcw + PULSE_DELAY;
		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
		posDrvDir = ccw;
	}

	actualPosdDeg += deltadDeg;

	return EXIT_SUCCESS;
}


/**
 * @brief: Initial homing to get timing values.
 *
 * @param: none
 *
 * @return: int 0 on success
 *
 * @detail: Simple homing is performed first. Drive moves clockwise and then counterclockwise.
 * Time of both movements is stopped then the movement time for one tenth of a degree is calulated.
 */
int init_home_pos_drive(){

	uint16_t cwTimeMs;
	uint16_t ccwTimeMs;

	home_pos_drive();

	TIM2->ARR = HOME_TIMEOUT;
	TIM2->CCR2 = PULSE_DELAY;
	uint32_t startTime = HAL_GetTick();
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

	while(!endPos){
		//Wait until SW2 is reached, maybe check SW in here?
	}

	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
	cwTimeMs = HAL_GetTick() - startTime;

	TIM4->ARR = HOME_TIMEOUT;
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

/**
 * @brief: Simple homing to get 0 position.
 *
 * @param: none
 *
 * @return: int 0 on success
 *
 * @detail: Move drive counterclockwise to start position. Used to get the initial position of the drive
 * either after power up as part of initial homing or after a emergency stop event happened.
 */
int home_pos_drive(void){

	if(startPos){
		actualPosdDeg = 0;
		homingComplete = SET;
		return EXIT_SUCCESS;
	}

	TIM4->ARR = HOME_TIMEOUT;
	TIM4->CCR2 = PULSE_DELAY;
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);

	while(!startPos){
		//Wait until SW1 is reached, maybe check SW in here?
	}

	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
	TIM4->CNT = 0;
	actualPosdDeg = 0;
	homingComplete = SET;

	return EXIT_SUCCESS;
}
