/*
 * pos_drv_control.h
 *
 *  Created on: May 28, 2024
 *      Author: floro
 */

#ifndef SRC_POS_DRV_CONTROL_H_
#define SRC_POS_DRV_CONTROL_H_

#include <stdint.h>

int set_pos_posdrv(uint16_t angle_degree);

int home_pos_drive(void);

#endif /* SRC_POS_DRV_CONTROL_H_ */
