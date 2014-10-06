///////////////////////////////////////////////////////////
//
//  This file is part of RTArduLink
//
//  Copyright (c) 2014 richards-tech
//
//  Permission is hereby granted, free of charge,
//  to any person obtaining a copy of
//  this software and associated documentation files
//  (the "Software"), to deal in the Software without
//  restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute,
//  sublicense, and/or sell copies of the Software, and
//  to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice
//  shall be included in all copies or substantial portions
//  of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
//  ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
//  TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
//  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.

#include <RTArduLinkDefs.h>
#include <RTArduLinkHAL.h>
#include <RTArduLink.h>
#include <RTArduLinkUtils.h>
#include "RTArduLinkDemoDefs.h"
#include <EEPROM.h>
#include <Servo.h>

#include "RTArduLinkDemo.h"
#include "RTArduLinkDemoDefs.h"

RTArduLinkDemo rtarduDemo;

//    Definitions for servos

Servo servos[SERVO_COUNT];
int servoPins[] = {2, 3};                                   // servo pins

//    Definitions for PWMs

int pwmPins[] = {9, 10, 11};                                // pwm pins

//    Definitions for Input pins

int inputPins[] = {4, 5};                                   // input pins

//    Definitions for Output pins

int outputPins[] = {6, 7};                                   // output pins


void setup()
{
    // Set up servo channels

    for (int i = 0; i < SERVO_COUNT; i++) {
        servos[i].attach(servoPins[i]);
    }

    // Set up PWM channels

    for (int i = 0; i < PWM_COUNT; i++) {
        pinMode(pwmPins[i], OUTPUT);
    }

    // Set up Input channels

    for (int i = 0; i < INPUT_COUNT; i++) {
        pinMode(inputPins[i], INPUT);
    }

    // Set up Output channels

    for (int i = 0; i < OUTPUT_COUNT; i++) {
        pinMode(outputPins[i], OUTPUT);
    }

    Serial.begin(115200);                                   // in case it's not being used
    rtarduDemo.begin(":RTArduLinkDemo");
    Serial.println("RTArduLinkDemo starting");
    RTArduLinkHALEEPROMDisplay();
    rtarduDemo.sendDebugMessage("Demo starting");
}

void loop()
{
    rtarduDemo.background();
}

//    Process a received message - should be a command

void RTArduLinkDemo::processCustomMessage(RTARDULINK_MESSAGE *message, int length)
{
    RTARDULINKDEMO_COMMAND *command;
    RTARDULINKDEMO_RESPONSE *response;
    int i;

    if (message->messageType != RTARDULINK_MESSAGE_CUSTOM) {
        sendDebugMessage("Wrong command type");
        return;
    }

    if (length != ((int)sizeof(RTARDULINKDEMO_COMMAND) + RTARDULINK_MESSAGE_HEADER_LEN)) {
        sendDebugMessage("Command wrong length");
        return;
    }

    command = (RTARDULINKDEMO_COMMAND *)message->data;      // this should contain the command

    //    set all peripherals to correct values

    for (i = 0; i < SERVO_COUNT; i++) {
        servos[i].writeMicroseconds(RTArduLinkConvertUC2ToInt(command->servoPos[i]));
    }

    for (i = 0; i < PWM_COUNT; i++) {
        analogWrite(pwmPins[i], command->pwmValue[i]);
    }

    for (i = 0; i < OUTPUT_COUNT; i++) {
        if (command->outputValue[i])
            digitalWrite(outputPins[i], HIGH);
        else
            digitalWrite(outputPins[i], LOW);
    }

    //    Now build response message

    response = (RTARDULINKDEMO_RESPONSE *)message->data;

    for (i = 0; i < OUTPUT_COUNT; i++) {
        if (digitalRead(inputPins[i]) == HIGH)
            response->inputValue[i] = true;
        else
            response->inputValue[i] = false;
    }

    message->messageType = RTARDULINK_MESSAGE_CUSTOM + 1;    // respond with this type
    sendMessage(message, sizeof(RTARDULINKDEMO_RESPONSE) + RTARDULINK_MESSAGE_HEADER_LEN);
}


