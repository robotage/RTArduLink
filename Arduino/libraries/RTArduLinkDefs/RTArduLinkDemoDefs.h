//
//  Copyright (c) 2014 richards-tech
//
//  This file is part of RTArduLink
//
//  RTArduLink is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  RTArduLink is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with RTArduLink.  If not, see <http://www.gnu.org/licenses/>.
//


#ifndef RTARDULINKDEMODEFS_H_
#define RTARDULINKDEMODEFS_H_

#include "RTArduLinkUtils.h"

#define SERVO_COUNT    2                                   // 2 servo channels
#define PWM_COUNT      3                                   // 3 pwm channels
#define INPUT_COUNT    2                                   // 2 inputs
#define OUTPUT_COUNT   2                                   // 2 outputs

#define SERVO_MIN_VALUE   1000                             // min servo value
#define SERVO_CTR_VALUE   1500                             // center servo value
#define SERVO_MAX_VALUE   2000                             // max servo value

#define PWM_MIN_VALUE     0                                // min pwm value
#define PWM_CTR_VALUE     128                              // center pwm value
#define PWM_MAX_VALUE     255                              // max pwm value

//    The command structure is sent from the host to the subsystem

typedef struct
{
    RTARDULINK_UC2 servoPos[SERVO_COUNT];                  // the servo positions
    unsigned char pwmValue[PWM_COUNT];                     // PWM values
    unsigned char outputValue[OUTPUT_COUNT];               // the output pin values (true=high, false=low)
} RTARDULINKDEMO_COMMAND;

//    the response structure is sent from the subsystem to the host

typedef struct
{
    unsigned char inputValue[INPUT_COUNT];                // the input pin values (true=high, false=low)   
} RTARDULINKDEMO_RESPONSE;


#endif /* RTARDULINKDEMODEFS_H_ */
