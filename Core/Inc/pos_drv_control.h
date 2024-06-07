/*
 * pos_drv_control.h
 *
 *  Created on: May 28, 2024
 *      Author: floro
 */

#ifndef SRC_POS_DRV_CONTROL_H_
#define SRC_POS_DRV_CONTROL_H_

#define PULSE_DELAY 10

#include <stdint.h>

extern uint16_t actualPosdDeg;

int set_pos_posdrv(uint16_t angle_degree);

int init_home_pos_drive(void);

int home_pos_drive(void);

#endif /* SRC_POS_DRV_CONTROL_H_ */
