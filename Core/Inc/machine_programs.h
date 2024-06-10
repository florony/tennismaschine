/*
 * machine_programs.h
 *
 *  Created on: May 16, 2024
 *      Author: floro
 */

#ifndef INC_MACHINE_PROGRAMS_H_
#define INC_MACHINE_PROGRAMS_H_

#define ANGLE_SET_DELAY 2000
#define MIN_ANGLE_DELTA 70
#define MIN_SPEED_DELTA 50
#define MAX_ADC_VALUE 4022
#define AUTO_DELAY 10
#define AUTO_SPEED_MIN 20

int home_pos_drive(void);

int pgm_stop(void);

int pgm_manual(void);

int pgm_auto_speed(void);

int pgm_auto(void);

int get_adc_values(uint16_t*);

int handle_angle_change(uint16_t adc_result, uint16_t* last_adc);

#endif /* INC_MACHINE_PROGRAMS_H_ */
