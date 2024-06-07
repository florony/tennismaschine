/*
 * machine_programs.h
 *
 *  Created on: May 16, 2024
 *      Author: floro
 */

#ifndef INC_MACHINE_PROGRAMS_H_
#define INC_MACHINE_PROGRAMS_H_

#define ANGLE_SET_DELAY 2000

extern uint32_t last_angle_change;

int home_pos_drive(void);

int pgm_stop(void);

int pgm_manual(void);

int pgm_auto_speed(void);

int pgm_auto(void);

int get_adc_values(uint16_t*);

#endif /* INC_MACHINE_PROGRAMS_H_ */
