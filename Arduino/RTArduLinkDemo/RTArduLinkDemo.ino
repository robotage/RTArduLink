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
int servoPins[] = {2, 3};                                    // servo pins

//    Definitions for PWMs

int pwmPins[] = {9, 10, 11};                                // pwm pins

//    Definitions for Input pins

int inputPins[] = {4, 5};                                    // input pins

//    Definitions for Output pins

int outputPins[] = {6, 7};                                    // output pins


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
 
    Serial.begin(115200);                                    // in case it's not being used
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
  
    command = (RTARDULINKDEMO_COMMAND *)message->data;        // this should contain the command
    
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


