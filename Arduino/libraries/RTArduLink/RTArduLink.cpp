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

#include "RTArduLink.h"
#include "RTArduLinkHAL.h"
#include "RTArduLinkUtils.h"

#include <string.h>

RTArduLink::RTArduLink()
{
}

RTArduLink::~RTArduLink()
{
}

void RTArduLink::begin(const char *identitySuffix)
{
	RTARDULINK_PORT *portInfo;

	if (!RTArduLinkHALEEPROMValid())
		RTArduLinkHALEEPROMDefault();

	m_identitySuffix = identitySuffix;

	// now set up host and subsystem ports based on EEPROM configuration

	for (int i = 0; i < RTARDULINKHAL_MAX_PORTS; i++) {
		portInfo = m_ports + i;
		portInfo->index = i;
		portInfo->inUse = RTArduLinkHALConfigurePort(&(portInfo->portHAL), i);
  		RTArduLinkRXFrameInit(&(portInfo->RXFrame), &(portInfo->RXFrameBuffer));	
	}
	m_hostPort = m_ports;
}


void RTArduLink::background()
{
	unsigned char index;
	RTARDULINK_PORT *portInfo;

	for (index = 0; index < RTARDULINKHAL_MAX_PORTS; index++) {
		portInfo = m_ports + index;
		if (!portInfo->inUse)
			continue;

		while (RTArduLinkHALPortAvailable(&(portInfo->portHAL))) {
			RTArduLinkReassemble(&(portInfo->RXFrame), RTArduLinkHALPortRead(&(portInfo->portHAL)));
    		if (portInfo->RXFrame.complete) {
    			processReceivedMessage(portInfo);
    			RTArduLinkRXFrameInit(&(portInfo->RXFrame), &(portInfo->RXFrameBuffer));
    		}
		}
  	}
}

void RTArduLink::processReceivedMessage(RTARDULINK_PORT *portInfo)
{
  	RTARDULINK_MESSAGE *message;							// a pointer to the message part of the frame
	unsigned char address;

	message = &(portInfo->RXFrameBuffer.message);			// get the message pointer
	address = message->messageAddress;

	switch (portInfo->index) {
		case  RTARDULINK_HOST_PORT:
			processHostMessage();							// came from this upstream link
			return;

		case RTARDULINK_DAISY_PORT:							// came from dasiy chain port
			if (address != RTARDULINK_HOST_PORT) 			// true if it came from a daisy chained subsystem, not a directly connected subsystem
				message->messageAddress += RTARDULINKHAL_MAX_PORTS; // add on the offset
			else 
				message->messageAddress = RTARDULINK_DAISY_PORT;	// set message address to be the port index
			break;

		default:
			message->messageAddress += portInfo->index;		// the port index is the address
			break;
	}

	// if get here, need to forward to host port

	sendFrame(m_hostPort, &(portInfo->RXFrameBuffer), portInfo->RXFrameBuffer.messageLength);
}


void RTArduLink::processHostMessage()
{
 	RTARDULINK_MESSAGE *message;							// a pointer to the message part of the frame
	int identityLength;
	int suffixLength;
	unsigned char address;

	message = &(m_hostPort->RXFrameBuffer.message);			// get the message pointer
	address = message->messageAddress;

	if (address == RTARDULINK_BROADCAST_ADDRESS) {			// need to forward to downstream ports also
		for (int i = RTARDULINK_HOST_PORT + 1; i < RTARDULINKHAL_MAX_PORTS; i++) {
			if (m_ports[i].inUse)
				sendFrame(m_ports + i, &(m_hostPort->RXFrameBuffer), m_hostPort->RXFrameBuffer.messageLength);
		}
	}
	if ((address == RTARDULINK_MY_ADDRESS) || (address == RTARDULINK_BROADCAST_ADDRESS)) {	// it's for me
		switch (message->messageType)
		{
			case RTARDULINK_MESSAGE_POLL:
				message->messageAddress = RTARDULINK_MY_ADDRESS;
				sendFrame(m_hostPort, &(m_hostPort->RXFrameBuffer), m_hostPort->RXFrameBuffer.messageLength);	// just send the frame back as received
				break;

			case RTARDULINK_MESSAGE_IDENTITY:
				identityLength = strlen(RTArduLinkHALConfig.identity);
				suffixLength = strlen(m_identitySuffix);
	
				memcpy(message->data, RTArduLinkHALConfig.identity, identityLength + 1);	// copy in identity

				if ((identityLength + suffixLength) < RTARDULINK_DATA_MAX_LEN - 1) {
					memcpy(message->data + identityLength, m_identitySuffix, suffixLength + 1); // copy in suffix
				} else {
					suffixLength = 0;
				}
				message->messageAddress = RTARDULINK_MY_ADDRESS;
				message->data[RTARDULINK_DATA_MAX_LEN - 1] = 0;		// make sure zero terminated if it was truncated
				sendFrame(m_hostPort, &(m_hostPort->RXFrameBuffer), RTARDULINK_MESSAGE_HEADER_LEN + identityLength + suffixLength + 1);					
				break;

			default:
				if (message->messageType < RTARDULINK_MESSAGE_CUSTOM) {	// illegal code
					message->data[0] = RTARDULINK_RESPONSE_ILLEGAL_COMMAND;
					message->data[1] = message->messageType;		// this is the offending code
					message->messageType = RTARDULINK_MESSAGE_ERROR;
					message->messageAddress = RTARDULINK_MY_ADDRESS;
					sendFrame(m_hostPort, &(m_hostPort->RXFrameBuffer), RTARDULINK_MESSAGE_HEADER_LEN + 2);
					break;
				}
				processCustomMessage(message, m_hostPort->RXFrameBuffer.messageLength);	// see if anyone wants to process it
				break;
		}
		return;
	}

	if (address >= RTARDULINKHAL_MAX_PORTS) {				// need to pass it to the first subsystem
		if (!m_ports[RTARDULINK_DAISY_PORT].inUse)
			return;											// there is no daisy chain port
		message->messageAddress = address - RTARDULINKHAL_MAX_PORTS; // adjust the address
		sendFrame(m_ports +RTARDULINK_DAISY_PORT, &(m_hostPort->RXFrameBuffer), m_hostPort->RXFrameBuffer.messageLength);
		return;
	}

	// if get here, needs to go to a local subsystem port

	if (m_ports[address].inUse) {
		message->messageAddress = 0;						// indicates that the target should process it
		sendFrame(m_ports + address, &(m_hostPort->RXFrameBuffer), m_hostPort->RXFrameBuffer.messageLength);
	}
}

void RTArduLink::sendDebugMessage(const char *debugMessage)
{
	RTARDULINK_FRAME frame;
	int stringLength;

	stringLength = strlen(debugMessage);
	if (stringLength >= RTARDULINK_DATA_MAX_LEN)
		stringLength = RTARDULINK_DATA_MAX_LEN-1;
	memcpy(frame.message.data, debugMessage, stringLength);
	frame.message.data[stringLength] = 0;
	frame.message.messageType = RTARDULINK_MESSAGE_DEBUG;
	frame.message.messageAddress = RTARDULINK_MY_ADDRESS;
	sendFrame(m_hostPort, &frame, RTARDULINK_MESSAGE_HEADER_LEN + stringLength + 1);
}

void RTArduLink::sendMessage(RTARDULINK_MESSAGE *message, int length)
{
	RTARDULINK_FRAME frame;

    message->messageAddress = RTARDULINK_MY_ADDRESS;
	if (length > RTARDULINK_MESSAGE_MAX_LEN)
		length = RTARDULINK_MESSAGE_MAX_LEN;
	frame.message = *message;

	sendFrame(m_hostPort, &frame, length);
}

void RTArduLink::sendFrame(RTARDULINK_PORT *portInfo, RTARDULINK_FRAME *frame, int length)
{
	frame->sync0 = RTARDULINK_MESSAGE_SYNC0;
	frame->sync1 = RTARDULINK_MESSAGE_SYNC1;
	frame->messageLength = length;							// set length
	RTArduLinkSetChecksum(frame);							// compute checksum
	RTArduLinkHALPortWrite(&(portInfo->portHAL), (unsigned char *)frame, frame->messageLength + RTARDULINK_FRAME_HEADER_LEN);
}
