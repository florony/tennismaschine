/*
 * pos_drv_control.c
 *
 *  Created on: May 29, 2024
 *      Author: floro
 */

#include <stdint.h>
#include <stdlib.h>
#include "main.h"

extern uint16_t actualPos;
extern uint16_t targetPos;
extern int8_t posDrvDir;
extern TIM_HandleTypeDef htim2;
extern FlagStatus homingComplete;
extern FlagStatus posDrvRunning;

int set_pos_posdrv(uint16_t angle_degree){

	/*Set PWM frequency to fast for positioning*/
	TIM2->ARR = POS_DRV_PERIOD(POS_DRV_FAST_FREQ, htim2.Init.Prescaler, 7000000);
	TIM2->CCR2 = (TIM2->ARR)/2;

	targetPos = angle_degree*STEPS_PER_DEGREE;
	if(targetPos == actualPos){
		return EXIT_SUCCESS;
	}
	else if(targetPos < actualPos) {
		HAL_GPIO_WritePin(POS_DIR_GPIO_Port, POS_DIR_Pin, POS_DRV_DIR_POLARITY ? GPIO_PIN_SET : GPIO_PIN_RESET);
		posDrvDir = -1;
	}
	else {
		HAL_GPIO_WritePin(POS_DIR_GPIO_Port, POS_DIR_Pin, POS_DRV_DIR_POLARITY ? GPIO_PIN_RESET : GPIO_PIN_SET);
		posDrvDir = 1;
	}

	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_2);
	posDrvRunning = SET;

	return EXIT_SUCCESS;
}

int home_pos_drive(void){

	/*Set PWM frequency to low for homing*/
	TIM2->ARR = POS_DRV_PERIOD(POS_DRV_SLOW_FREQ, htim2.Init.Prescaler, 7000000);
	TIM2->CCR2 = (TIM2->ARR)/2;

	HAL_GPIO_WritePin(POS_DIR_GPIO_Port, POS_DIR_Pin, POS_DRV_DIR_POLARITY ? GPIO_PIN_SET : GPIO_PIN_RESET);

	if(!HAL_GPIO_ReadPin (SW_1_GPIO_Port, SW_1_Pin)){

		HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_2);

		while(HAL_GPIO_ReadPin (SW_1_GPIO_Port, SW_1_Pin)){
			/*Wait until homingpos is reached */
			/*TODO Maybe add some safety stuff here if switch fails*/
		}

		HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_2);
	}
	actualPos = 0;
	homingComplete = SET;

	return EXIT_SUCCESS;
}
