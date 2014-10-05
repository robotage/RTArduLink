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

#ifndef _RTARDULINKHAL_H
#define _RTARDULINKHAL_H


//----------------------------------------------------------
//	Target-specific includes
//
//
//	Arduino HAL

#include <RTArduLinkDefs.h>
#include <HardwareSerial.h>
#include <EEPROM.h>

#define	RTARDULINKHAL_MAX_SUBSYSTEM_PORTS	3				// maximum number of subsystem ports
#define RTARDULINKHAL_MAX_PORTS				(RTARDULINKHAL_MAX_SUBSYSTEM_PORTS + 1)	// max total ports (including host)

//	RTARDULINKHAL_PORT should be modified as appropriate for the target.
//	There is one copy of this per port. It contains all state needed about
//	a serial port.

typedef struct
{
	HardwareSerial *serialPort;										// the serial port structure
} RTARDULINKHAL_PORT;

//	RTARDULINKHAL_EEPROM is the target-specific structure used to
//	store configs in EEPROM

typedef struct
{
	char valid;												// 0xff = invalid, 0 = valid
	char identity[RTARDULINK_DATA_MAX_LEN];					// identity string
	unsigned char portSpeed[RTARDULINKHAL_MAX_PORTS];		// port speed codes
	unsigned char hardwarePort[RTARDULINKHAL_MAX_PORTS];	// port number for hardware serial
} RTARDULINKHAL_EEPROM;

//	The global config structure

extern RTARDULINKHAL_EEPROM RTArduLinkHALConfig;



//----------------------------------------------------------
//
// These functions must be provided the RTArduLinkHAL for all implementations

//	RTArduLinkHALConfigurePort() activates the specified port configuration specified by portIndex in port structure port.

	bool RTArduLinkHALConfigurePort(RTARDULINKHAL_PORT *port, int portIndex);


//	RTArduLinkHALPortAvailable() returns the number of bytes availabel on the specified port.

	int RTArduLinkHALPortAvailable(RTARDULINKHAL_PORT *port);


//	RTArduLinkHALPortRead() returns the next available byte from a port. Always check available bytes first

	unsigned char RTArduLinkHALPortRead(RTARDULINKHAL_PORT *port);	


//	RTArduLinkHALPortWrite() writes length bytes of the block pointed to by data to the specified port.

	void RTArduLinkHALPortWrite(RTARDULINKHAL_PORT *port, unsigned char *data, unsigned char length);


//	RTArduLinkHALEEPROMValid() returns true if the EEPROM contains a valid configuration, 
//	false otherwise.

	bool RTArduLinkHALEEPROMValid();						// returns true if a valid config	


//	RTArduLinkHALEEPROMDisplay() displays the current configuration

	void RTArduLinkHALEEPROMDisplay();						// display the config


//	RTArduLinkHALEEPROMDisplayPort() displays the configuration for a single port
//	If suppress is true, nothing is displayed if the port is not enabled. If false
//	the port's data will be displayed regardless.

	void RTArduLinkHALEEPROMDisplayPort(int port, bool suppress);	// display the port config


//	RTArduLinkHALEEPROMDefault() writes a default config to EEPROM

	void RTArduLinkHALEEPROMDefault();						// write and load default settings


//	RTArduLinkHALEEPROMRead() loads the EEPROM config into the RTArduLinkHALConfig
//	global structure.

	void RTArduLinkHALEEPROMRead();							// to load the config


//	RTArduLinkHALEEPROMWrite() writes the config in the RTArduLinkHALConfig
//	global structure back to EEPROM.

	void RTArduLinkHALEEPROMWrite();						// to write the config


#endif // _RTARDULINKHAL_H

