/*
 * pos_drv_control.c
 *
 *  Created on: May 29, 2024
 *      Author: floro
 */

#include <stdint.h>
#include <stdlib.h>
#include "main.h"

int set_pos_posdrv(uint16_t angle_degree){

	/*Check if drive is running, if true get current reg state and take it as correction value for actual value
	 * calc actual value by using the correction from register (remaining pulse duration corrected by dir)
	 * Calc the new target val based on target degree value.
	 * Calc new pulse duration based on target value, actual value and direction
	 * fire pulse set moving flag to true
	 * TODO add callback to reset flag
	 */

	return EXIT_SUCCESS;
}

int init_home_pos_drive(){

	/*Move drive towards SW0 until flag = true, set pos = 0, move towards SW1 until flag = true
	 * stop time from SW0 to SW1, move back to SW0 stop time from SW1 to SW0.
	 * Based on timing calc ms per degree (slower time) and calc correction factor (slow/fast)
	 * TODO add EXTIS for switches SW0 and SW1
	 */


	return EXIT_SUCCESS;
}

int home_pos_drive(void){

	/*Move towards SW0 and set actual POS to 0*/

	return EXIT_SUCCESS;
}
