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

/*
 * @brief: this function stops the main Drives and sets the machine in a waiting state
 *
 * @param: none
 *
 * @returns: int 0 = success
 */

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

/*
 * @brief: this function handles the manual mode. Speed,Spin an Angle are controlled via potis.
 * the function reads the ADC values, converts them in real world values and trigger setup functions if needed.
 * HMI related functions are placed here.
 *
 * @param: none
 *
 * @returns: int 0 if success
 */

int pgm_manual(void){

	Set_Led_Output(GREEN);
	seg7_displayOnOffMulti(SPEED | SPIN | ANGLE);

	get_adc_values(adc_result);

	speed_percent = adc_result[0]*100/MAX_ADC_VALUE;
	spin_percent = (adc_result[1]*100/MAX_ADC_VALUE)-50;
	angle_degree = (adc_result[2]*90/MAX_ADC_VALUE);

	if(
		(abs(last_adc[0] - adc_result[0]) > MIN_SPEED_DELTA) |
		(abs(last_adc[1] - adc_result[1]) > MIN_SPEED_DELTA)){

			last_adc[0] = adc_result[0];
			last_adc[1] = adc_result[1];
			seg7_displayInt((int16_t)speed_percent, SPEED_ADDR);
			seg7_displayInt(spin_percent, SPIN_ADDR);
			set_pwm_maindrv(speed_percent, spin_percent, htim1);
		}

	handle_angle_change(adc_result[2], &last_adc[2]);

	if(!mainDrvRunning){
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);

		mainDrvRunning = SET;
	}

	return EXIT_SUCCESS;
}

/*
 * @brief: this function sets the machine to auto speed mode. Speed an Spin are changed based on a random
 * number. Angle is controlled manually. All HMI related function are located here.
 *
 * @parm: none
 *
 * @returns: int 0 if success
 */

int pgm_auto_speed(void){

	Set_Led_Output(GREEN);

	uint8_t text_auto[] = {SEG7_A, SEG7_U, SEG7_T, SEG7_0};

	seg7_displayOnOffMulti(SPEED | SPIN | ANGLE);
	seg7_display(text_auto, SPEED_ADDR);
	seg7_display(text_auto, SPIN_ADDR);

	get_adc_values(adc_result);

	angle_degree = (adc_result[2]*90/MAX_ADC_VALUE);

	handle_angle_change(adc_result[2], &last_adc[2]);

	if((HAL_GetTick() - last_rand_tick) > AUTO_DELAY * 1000){

			uint16_t rand_speed = (rand() % (101 - AUTO_SPEED_MIN)) + AUTO_SPEED_MIN;
			int16_t rand_spin = (rand() % 101) -50;

			set_pwm_maindrv(rand_speed, rand_spin, htim1);

			last_rand_tick = HAL_GetTick();
		}

	return EXIT_SUCCESS;
}

/*
 * @brief: this function sets the machine to full auto mode. All parameters are set randomly.
 *
 * @param: none
 *
 * @returns: int 0 if success
 */

int pgm_auto(void){

	Set_Led_Output(GREEN);

	uint8_t text_auto[] = {SEG7_A, SEG7_U, SEG7_T, SEG7_0};

	seg7_displayOnOffMulti(SPEED | SPIN | ANGLE);
	seg7_display(text_auto, SPEED_ADDR);
	seg7_display(text_auto, SPIN_ADDR);
	seg7_display(text_auto, ANGLE_ADDR);

	if((HAL_GetTick() - last_rand_tick) > AUTO_DELAY * 1000){

		uint16_t rand_speed = (rand() % (101 - AUTO_SPEED_MIN)) + AUTO_SPEED_MIN;
		int16_t rand_spin = (rand() % 101) -50;
		uint16_t rand_angle = rand() % 91;

		set_pwm_maindrv(rand_speed, rand_spin, htim1);
		set_pos_posdrv(rand_angle);

		last_rand_tick = HAL_GetTick();
	}

	return EXIT_SUCCESS;
}

/*
 * @brief: read the adc channels 1-3 and writes the value to array
 *
 * @param: uint16_t* array size 3 to store ADC readings
 *
 * @returns: int 0 on success
 */

int get_adc_values(uint16_t* adc_result){

	for(int i = 0; i < 3; i++){
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 1);
			adc_result[i] = HAL_ADC_GetValue(&hadc1);
		}

	return EXIT_SUCCESS;
}

/*
 * @brief: handling the angle value if set manually. Repositioning is only triggered when a minimum value change
 * is exceeded. Afer every change of the value a delay is set until the position changes.
 *
 * @param: uint16_t current ADC result
 *
 * @param: uint16_t* last set position
 *
 * @returns: int 0 on success
 */
int handle_angle_change(uint16_t adc_result, uint16_t* last_adc){

	static uint32_t last_angle_change = 0;	//Timestamp for last change of angle target value
	static FlagStatus AngleChanged = RESET; 	//Set if the target value has changed

	if(abs(*last_adc - adc_result) > MIN_ANGLE_DELTA){
			*last_adc = adc_result;
			seg7_displayInt((int16_t)angle_degree, ANGLE_ADDR);
			seg7_setDispAddr(ANGLE_ADDR);
			seg7_setBlinkRate(2);
			AngleChanged = SET;
			last_angle_change = HAL_GetTick();
	}

	if(((HAL_GetTick() - last_angle_change) > ANGLE_SET_DELAY) & AngleChanged){
		seg7_setDispAddr(ANGLE_ADDR);
		seg7_setBlinkRate(0);
		set_pos_posdrv(angle_degree);
		AngleChanged = RESET;
	}

	return EXIT_SUCCESS;
}
