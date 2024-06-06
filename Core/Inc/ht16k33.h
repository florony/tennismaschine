/*
 * ht16k33.h
 *
 *  Created on: May 18, 2022
 *  Author: Lennart Koepper
 *  Modified: 04.06.2024
 *  Modified: Florian Ronacher
 */

#ifndef INC_HT16K33_H_
#define INC_HT16K33_H_

#include "stdint.h"
#include "main.h"

// !!! I2C Configuration !!!
#define HT16K33_I2C_PORT  hi2c2
#define SPEED_ADDR        0x70
#define SPIN_ADDR        0x71
#define ANGLE_ADDR        0x72

#define SPEED (uint8_t)(0x1 << 0)
#define SPIN (uint8_t)(0x1 << 1)
#define ANGLE (uint8_t)(0x1 << 2)

// displayable Characters
#define SEG7_0                0
#define SEG7_1                1
#define SEG7_2                2
#define SEG7_3                3
#define SEG7_4                4
#define SEG7_5                5
#define SEG7_6                6
#define SEG7_7                7
#define SEG7_8                8
#define SEG7_9                9
#define SEG7_A                10
#define SEG7_B                11
#define SEG7_C                12
#define SEG7_D                13
#define SEG7_E                14
#define SEG7_F                15
#define SEG7_SPACE            16
#define SEG7_MINUS            17
#define SEG7_I                18		// I & L, e.g. to write "FAIL"
#define SEG7_L                19
#define SEG7_T				  20
#define SEG7_P				  21
#define SEG7_U				  22
#define SEG7_NONE             99

void seg7_init(uint16_t disp_addr);		// initializes 7-segment-display
void seg7_reset(uint16_t disp_addr);		// resets 7-segment-display
void seg7_clearCache(void);	// force clearing of the cache
void seg7_refresh(void); 	// force writing of cache to display

void seg7_displayOn(void);	// enable display
void seg7_displayOff(void);	// disable display, fastest way to darken display

void seg7_setBrightness(uint8_t value);		// 0 .. 15	  0 = off, 15 = max. brightness
void seg7_setBlinkRate(uint8_t value);      // 0 .. 3     0 = off, 1 = 2Hz, 2 = 1Hz, 3 = 0,5Hz
void seg7_setDigits(uint8_t value);			// 0 .. 4	  minimal number of digits shown

void seg7_displayClear(uint16_t disp_addr);									// fills display with spaces
int seg7_displayInt(int n, uint16_t disp_addr);                   					// -999 .. 9999
int seg7_displayTime(uint8_t left, uint8_t right, int colon, uint16_t disp_addr);	// 00:00 .. 99:99, colon can be set to false (e.g. simulate blink)

void seg7_display(uint8_t *array, uint16_t disp_addr);                  	// displays 4 displayable Character values (stored in array)
void seg7_displayPoint(uint8_t *array, uint8_t point);  // displays 4 displayable Character values (stored in array), point = digit with . (0..3)
void seg7_displayColon(uint8_t on);                 	// 0 = off
void seg7_displayRaw(uint8_t *array, int colon);  		// control the 4 Characters and the colon
void seg7_setDispAddr(uint16_t disp_addr);

void seg7_displayOnOffMulti(uint8_t display);

#endif /* INC_HT16K33_H_ */
