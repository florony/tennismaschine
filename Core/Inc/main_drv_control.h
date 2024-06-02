/*
 * pwm_main_drv.h
 *
 *  Created on: May 28, 2024
 *      Author: floro
 */

#include <stdint.h>
#include "main.h"

#ifndef INC_PWM_MAIN_DRV_H_
#define INC_PWM_MAIN_DRV_H_

int set_pwm_maindrv(uint16_t speed_percent, int16_t spin_percent, TIM_HandleTypeDef htim);

int calc_drv_dutycycle(
		uint32_t min_speed,
		uint32_t max_speed,
		uint32_t abs_speed,
		int16_t spin_percent,
		uint32_t* top_drv,
		uint32_t* bottom_drv
		);

#endif /* INC_PWM_MAIN_DRV_H_ */
