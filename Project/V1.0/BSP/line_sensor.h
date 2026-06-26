/*
 * line_sensor.h
 *
 * 7-Channel Grayscale Line Sensor Interface
 *
 * Sensor layout (left to right, facing forward):
 *   S1  S2  S3  S4  S5  S6  S7
 *   -3  -2  -1   0  +1  +2  +3   (position weights)
 *
 * Pin mapping:
 *   S1→PB8, S2→PA12, S3→PA18, S4→PB3, S5→PB9, S6→PA9, S7→PB19
 *
 * All channels are digital (GPIO input with pull-up):
 *   HIGH = black line, LOW = white surface
 *   Read returns bit=1 for black.
 */

#ifndef BSP_LINE_SENSOR_H_
#define BSP_LINE_SENSOR_H_

#include <stdint.h>

void  LineSensor_Init(void);
uint8_t LineSensor_Read(void);
uint8_t LineSensor_ReadDebounced(void);
float LineSensor_GetPosition(void);
int   LineSensor_IsOnLine(void);
int   LineSensor_IsAllBlack(void);

#endif /* BSP_LINE_SENSOR_H_ */
