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


#ifndef _RTARDULINK_H
#define _RTARDULINK_H

#include "RTArduLinkDefs.h"
#include "RTArduLinkHAL.h"

#define	RTARDULINK_HOST_PORT				0				// host port is always 0
#define	RTARDULINK_DAISY_PORT				1				// daisy chain port is always 1

typedef struct
{
	int index;												// port index
	bool inUse;												// true if in use
	RTARDULINK_RXFRAME RXFrame;								// structure to maintain receive frame state
	RTARDULINK_FRAME RXFrameBuffer;							// used to assemble received frames
	RTARDULINKHAL_PORT portHAL;								// the actual hardware port interface						
} RTARDULINK_PORT;

class RTArduLink
{
public:
	RTArduLink();
	virtual ~RTArduLink();

	void begin(const char *identitySuffix);					// should be called in setup() code
	void background();										// should be called once per loop()
	void sendDebugMessage(const char *debugMesssage);		// sends a debug message to the host port
	void sendMessage(RTARDULINK_MESSAGE *message, int length); // sends a message to the host port

protected:
//	These are functions that can be overridden

	virtual void processCustomMessage(RTARDULINK_MESSAGE *message, int length) {};

	RTARDULINK_PORT m_ports[RTARDULINKHAL_MAX_PORTS];		// port array
	RTARDULINK_PORT *m_hostPort;							// a link to the entry for the host port


private:
	void processReceivedMessage(RTARDULINK_PORT *port);		// process a completed message
	void processHostMessage();								// special case for stuff received from the host port
	void sendFrame(RTARDULINK_PORT *portInfo, RTARDULINK_FRAME *frame, int length);	// send a frame to the host. length is length of data field

	const char *m_identitySuffix;							// what to add to the EEPROM identity string

};

#endif // _RTARDULINK_H

