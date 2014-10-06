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
#include <EEPROM.h>

#include "RTArduLinkTest.h"

RTArduLinkEcho rtarduEcho;

void setup()
{
    Serial.begin(115200);                                    // in case it's not being used
    rtarduEcho.begin(":RTArduLinkTest");
    Serial.println("RTArduLinkTest starting");
    RTArduLinkHALEEPROMDisplay();
    rtarduEcho.sendDebugMessage("Echo test starting");
}

void loop()
{
    rtarduEcho.background();
}

void RTArduLinkEcho::processCustomMessage(RTARDULINK_MESSAGE *message, int length)
{
    message->messageType = RTARDULINK_MESSAGE_CUSTOM + 1;
    sendMessage(message, length);
}


