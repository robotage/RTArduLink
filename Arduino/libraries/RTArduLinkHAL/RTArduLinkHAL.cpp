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

#include <string.h>
#include "RTArduLinkHAL.h"

//----------------------------------------------------------
//
//	Arduino HAL


//	The global config structure

RTARDULINKHAL_EEPROM RTArduLinkHALConfig;

bool RTArduLinkHALAddHardwarePort(RTARDULINKHAL_PORT *port, long portSpeed, unsigned char hardwarePort);

//	Port speed map array

unsigned long RTArduLinkHALSpeedMap[] = {0, 9600, 19200, 38400, 57600, 115200};


bool RTArduLinkHALConfigurePort(RTARDULINKHAL_PORT *port, int portIndex) 
{
	if (RTArduLinkHALConfig.portSpeed[portIndex] == RTARDULINK_PORT_SPEED_OFF)
		return false;										// port is not enabled

	return RTArduLinkHALAddHardwarePort(port, RTArduLinkHALSpeedMap[RTArduLinkHALConfig.portSpeed[portIndex]], 
					RTArduLinkHALConfig.hardwarePort[portIndex]);
}

int RTArduLinkHALPortAvailable(RTARDULINKHAL_PORT *port)
{
	return port->serialPort->available();
}

unsigned char RTArduLinkHALPortRead(RTARDULINKHAL_PORT *port)
{
	return port->serialPort->read();
}

void RTArduLinkHALPortWrite(RTARDULINKHAL_PORT *port, unsigned char *data, unsigned char length)
{
	port->serialPort->write(data, length);
}


bool RTArduLinkHALAddHardwarePort(RTARDULINKHAL_PORT *port, long portSpeed, unsigned char hardwarePort)
{
	HardwareSerial *hardPort;

	switch (hardwarePort) {
		case 0:
			hardPort = &Serial;
			break;

		case 1:
#if defined(UBRR1H)
			hardPort = &Serial1;
#else
			return false;
#endif
			break;

		case 2:
#if defined(UBRR2H)
			hardPort = &Serial2;
#else
			return false;
#endif
			break;

		case 3:
#if defined(UBRR3H)
			hardPort = &Serial3;
#else
			return false;
#endif
			break;

		default:
			return false;
	}

	port->serialPort = hardPort;
	hardPort->begin(portSpeed);								// start the port
	return true;
}


bool RTArduLinkHALEEPROMValid()
{
	RTArduLinkHALConfig.valid = 0xff;						// preset to invalid
	RTArduLinkHALEEPROMRead();								// see what it really is
	return RTArduLinkHALConfig.valid == 0;
}

void RTArduLinkHALEEPROMDisplay()
{
	Serial.println();
	if (RTArduLinkHALConfig.valid != 0) {
		Serial.println("Invalid config");
		return;
	}
	Serial.print("Identity: ");
	Serial.println(RTArduLinkHALConfig.identity);

	for (int i = 0; i < RTARDULINKHAL_MAX_PORTS; i++)
		RTArduLinkHALEEPROMDisplayPort(i, true);
}

void RTArduLinkHALEEPROMDisplayPort(int index, bool suppress)
{
	if (suppress && (RTArduLinkHALConfig.portSpeed[index] == RTARDULINK_PORT_SPEED_OFF))
		return;
	Serial.print("Port index ");
	Serial.print(index);
	Serial.print(" speed=");
	Serial.print(RTArduLinkHALConfig.portSpeed[index]);
	Serial.print(", ");
	Serial.print("hardware port number=");
	Serial.println(RTArduLinkHALConfig.hardwarePort[index]);
}


void RTArduLinkHALEEPROMDefault()
{
	RTArduLinkHALConfig.valid = 0;
	strcpy(RTArduLinkHALConfig.identity, "RTArduLink_Arduino");

	RTArduLinkHALConfig.portSpeed[0] = RTARDULINK_PORT_SPEED_115200;
	for (int i = 1; i < RTARDULINKHAL_MAX_PORTS; i++)
		RTArduLinkHALConfig.portSpeed[i] = RTARDULINK_PORT_SPEED_OFF;

	for (int i = 0; i < RTARDULINKHAL_MAX_PORTS; i++)
		RTArduLinkHALConfig.hardwarePort[i] = i;

	RTArduLinkHALEEPROMWrite();
}

void RTArduLinkHALEEPROMRead()
{
	unsigned char *data;

	data = (unsigned char *)&RTArduLinkHALConfig;

	for (int i = 0; i < (int)sizeof(RTARDULINKHAL_EEPROM); i++)
		*data++ = EEPROM.read(i);
}

void RTArduLinkHALEEPROMWrite()
{
	unsigned char *data;

	data = (unsigned char *)&RTArduLinkHALConfig;

	for (int i = 0; i < (int)sizeof(RTARDULINKHAL_EEPROM); i++)
		EEPROM.write(i, *data++);
}
