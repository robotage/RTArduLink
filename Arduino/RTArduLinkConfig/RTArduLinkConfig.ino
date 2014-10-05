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

#include "RTArduLinkConfig.h"
#include <RTArduLinkDefs.h>
#include <RTArduLinkHAL.h>
#include <stdlib.h>
#include <string.h>
#include <EEPROM.h>

char getInput();
void flushInput();
void setIdentity();
void setDefaults();
void setPortSpeed(int index);
void setHardwarePort(int index);

void setup()
{
    Serial.begin(115200);
    if (!RTArduLinkHALEEPROMValid())
        RTArduLinkHALEEPROMDefault();
}

void loop()
{
    char value;
    
    Serial.println("RTArduLink Configuration");
    
    while (1) {
        flushInput();
        Serial.print("\nEnter command (h for help): ");
        value = getInput();                                          // get the command
        getInput();                                                  // discard the newline
        Serial.println(value);
        switch(value) {
            case 'c':
                Serial.println("");
                RTArduLinkHALEEPROMDisplay();
                break;
                
            case 'd':
                setDefaults();
                break;
                
            case 'h':
            case 'H':
                Serial.println("\n  c - show current config");
                Serial.println("  d - set defaults");
                Serial.println("  h - print help");
                Serial.println("  i - set identity");
                Serial.println("  0 - configure host port");
                Serial.println("  1 - configure subsystem port 1");
                Serial.println("  2 - configure subsystem port 2");
                Serial.println("  3 - configure subsystem port 3");
               break;
                
            case 'i':
            case 'I':
                setIdentity();
                break;
                
            case '0':
                configurePort(0);
                break;
                
            case '1':
                configurePort(1);
                break;
  
            case '2':
               configurePort(2);
               break;
  
            case '3':
               configurePort(3);
               break;
  
        }       
    }
}

void configurePort(int index)
{
    char value;
    
    Serial.println("RTArduLink Port Configuration");
    
    while (1) {
        RTArduLinkHALEEPROMDisplayPort(index, false);
        Serial.println();
        Serial.println("  p - hardware port number");
        Serial.println("  s - port speed");
        Serial.println("  x - return to main menu");
        
        flushInput();
        Serial.print("\nPort ");
        Serial.print(index); 
        Serial.print(" config - enter command: ");
        value = getInput();                                          // get the command
        getInput();                                                  // discard the newline
        Serial.println(value);
        switch(value) {
                
            case 'p':
            case 'P':
                setHardwarePort(index);
                break;
                            
            case 's':
            case 'S':
                setPortSpeed(index);
                break;
                            
            case 'x':
            case 'X':
                Serial.println();
                RTArduLinkHALEEPROMDisplay();
                return;
        }    
    }
}


void setIdentity()
{
    byte buffer[RTARDULINK_DATA_MAX_LEN];
    char value;
    int ptr;
    
    Serial.print("\nEnter new identity: ");
 
    for (ptr = 0; ptr < RTARDULINK_DATA_MAX_LEN-1; ptr++) {
        value = getInput();
        if (value == '\n') {
            break;
        }
        Serial.print(value);
        buffer[ptr] = value;
    }   
    buffer[ptr] = 0;
    
    
    if (ptr == 0) {
        Serial.println("\nUnchanged");
        return;    
    }
    strcpy(RTArduLinkHALConfig.identity, (const char *)buffer);
    RTArduLinkHALEEPROMWrite();
    RTArduLinkHALEEPROMDisplay();
}

void setDefaults()
{
    char value;
    
    flushInput();
    Serial.print("\nSet default config (y/n): ");
    value = getInput();
    if ((value == 'y') || (value == 'Y')) {
        RTArduLinkHALEEPROMDefault();
        Serial.println("\nEEPROM has been restored to default config.\n"); 
    } 
    RTArduLinkHALEEPROMDisplay();
}

void setHardwarePort(int index)
{
    char value;
    
    flushInput();
    Serial.print("\nHardware port (0-3): ");
    value = getInput();
    if ((value < '0') || (value > '3')) {
        Serial.println("\nUnchanged");
        return;   
    }
    Serial.print("\nHardware port set to ");
    Serial.println(value);
    RTArduLinkHALConfig.hardwarePort[index] = value - '0';
    RTArduLinkHALEEPROMWrite();
}

void setPortSpeed(int index)
{
    char value;
    
    flushInput();
    Serial.print("\nEnter speed code (0=off, 1=9600, 2=19200, 3=38400, 4=57600, 5=115200): ");
    value = getInput();
    if ((index == 0) && (value == '0')) {
        Serial.println("\nCan't turn off port 0");
        return;  
    }
    if ((value < '0') || (value > '5')) {
        Serial.println("\nUnchanged");
        return;
    }
    Serial.print("\nSpeed set to ");
    Serial.println(value);
    RTArduLinkHALConfig.portSpeed[index] = value - '0';
    RTArduLinkHALEEPROMWrite();
}

void flushInput()
{
    delay(10);
    while (Serial.available()) {
        Serial.read();    
        delay(1);
    }
}

char getInput()
{
     while (!Serial.available())
        ;   
     return Serial.read();
}
