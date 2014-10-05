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


