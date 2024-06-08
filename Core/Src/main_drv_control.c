/*
 * pwm_main_drv.c
 *
 *  Created on: May 28, 2024
 *      Author: floro
 */

#include "main_drv_control.h"
#include "main.h"
#include <stdint.h>
#include <stdlib.h>

/*
 * @brief: Set the PWM duty cycle for the main drives based on the speed and spin setting
 *
 * @param: 	speed_percent: speed from 0 to 100%
 * 			spin_percent: spin from -50 to +50%
 * 			htim: timer handle type def
 *
 * @return: int 1 on success
 *
 * @detail: Calculate the range from min to max duty cycle. Calculate the absolute value for min duty cycle = offset.
 *	 	 	Calculate absolute value for max duty cycle. Calculate the absolute target speed value based on input argument
 *	 	 	speed_percent. Set the compare registers for the main drives.
 */


int set_pwm_maindrv(uint16_t speed_percent, int16_t spin_percent, TIM_HandleTypeDef htim){

	/*Calculate the range from min to max duty cycle. Calculate the absolute value for min duty cycle = offset.
	 * Calculate absolute value for max duty cycle. Calculate the absolute target speed value based on input argument
	 * speed_percent. Set the compare registers for the main drives.*/

	uint32_t speed_range = htim.Init.Period*(MAIN_DRV_MAX_DUTY - MAIN_DRV_MIN_DUTY)/100;
	uint32_t speed_offset = htim.Init.Period*MAIN_DRV_MIN_DUTY/100;
	uint32_t speed_max = speed_offset + speed_range;
	uint32_t speed_absolute = speed_offset+(speed_range*speed_percent/100);

	uint32_t speed_top = 0;
	uint32_t speed_bottom = 0;

	calc_drv_dutycycle(speed_offset, speed_max, speed_absolute, spin_percent, &speed_top, &speed_bottom);

	TIM1->CCR1 = speed_top;
	TIM1->CCR2 = speed_bottom;

	return EXIT_SUCCESS;
}

/*
 * @brief: Calculate the compare register values for the main drives based on speed an spin value.
 *
 * @param: 	min_speed: minimum absolute value for register
 * 			max_speed: maximum absolute value for register
 * 			abs_speed: target absolute speed value
 * 			spin_percent: target spin value in percent
 * 			top_drv: pointer to register value for top drive
 * 			bottom_drv: pointer to register value for bottom drive
 *
 * @return: int 1 on success
 *
 * @detail: First the speed difference between both drives is calculated bases on the absolute spin value.
 * 			Top and bottom margin from absolute speed value to max and min possible value is calculated.
 * 			Then the register pointers are defined as slower and faster drive. If the top margin is smaller
 * 			than the absolute speed + the required speed difference, the faster drive is set to max speed
 * 			and the slower drive to max speed - 2 time the difference for each drive. Same procedure for the
 * 			slower drive with bottom margin.
 * 			If the margin limits are not reached, the register value is the absolute speed +/- the difference.
 * */
int calc_drv_dutycycle(
		uint32_t min_speed,
		uint32_t max_speed,
		uint32_t abs_speed,
		int16_t spin_percent,
		uint32_t* top_drv,
		uint32_t* bottom_drv
		){

	uint32_t speed_dif = (max_speed-min_speed)*abs(spin_percent)/100;
	uint32_t top_margin = max_speed - abs_speed;
	uint32_t bottom_margin = abs_speed - min_speed;

	uint32_t* faster_drv;
	uint32_t* slower_drv;

	(spin_percent > 0) ? (faster_drv = top_drv) : (faster_drv = bottom_drv);
	(spin_percent < 0) ? (slower_drv = top_drv) : (slower_drv = bottom_drv);

	if(speed_dif == 0){
		*top_drv = *bottom_drv = abs_speed;
		return EXIT_SUCCESS;
	}

	if(top_margin < speed_dif){
		*faster_drv = max_speed;
		*slower_drv = max_speed - 2*speed_dif;
	}
	else if(bottom_margin < speed_dif){
		*slower_drv = min_speed;
		*faster_drv = min_speed + 2*speed_dif;
	}
	else {
		*faster_drv = abs_speed + speed_dif;
		*slower_drv = abs_speed - speed_dif;
	}

	return EXIT_SUCCESS;

}
