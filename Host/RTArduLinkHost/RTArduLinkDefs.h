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


//	The RTArduLink communications protocol works by exchanging frames across the host <-> subsystem interface.
//	The structure RTARDULINK_FRAME defines the frame structure. There is a 4 byte header for frame level control
//	while the remainder is available for higher level message. Note that the structure implies fixed length
//	buffers which works well with the subsystem however only the number of bytes actually used are transferred
//	across the interface.
//
//	Note that there is no flow control at the frame level - it is assumed that the higher level interactions host -> subsystem
//	are window 1 acknowledged transfers so that the maximum possible unprocessed frames is equal to the number
//	of higher level services. Subsystem -> host transfers are always either responses to host commands or else regular
//	status updates so the rate from subsystem to host is controlled by configuration.
//
//	The frame integrity is protected by a single byte checksum. To keep things very simple, it is the 2s complement
//	of the 8 bit sum of all the bytes in the message array. This is used in conjunction with 0xAA and 0x55 bytes
//	to determine correct sync (in case of lost bytes which should not really happen!).
//
//	Frame sync is obtained by reading bytes until the 0xAA pattern is seen. If the next byte is not 0x55, keep
//	scanning. If it is, assume this byte is messageLength and the next one is the frameCksm value. Read in the
//	message array based on messageLength and then calculate the checksum. If the checksum is correct, sync has been
//	obtained and the message is valid. Otherwise, start looking for an 0xAA value again.

#ifndef _RTARDULINKDEFS_H
#define _RTARDULINKDEFS_H

//	Some defines to cope with compiler differences

#ifndef __cplusplus
#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

typedef unsigned char bool;
#endif

#ifndef NULL
#define NULL 0
#endif


//	Some general purpose typedefs - used especially for transferring values greater than
//	8 bits across the link and avoids endian issues. Assumes processor has 32 bit ints!

typedef	unsigned char RTARDULINK_UC2[2];					// an array of two unsigned chars
typedef	unsigned char RTARDULINK_UC4[4];					// an array of four unsigned chars

//	Port speed codes

#define	RTARDULINK_PORT_SPEED_OFF		0					// port is unused
#define	RTARDULINK_PORT_SPEED_9600		1					// 9600 baud
#define	RTARDULINK_PORT_SPEED_19200		2					// 19200 baud
#define	RTARDULINK_PORT_SPEED_38400		3					// 38400 baud
#define	RTARDULINK_PORT_SPEED_57600		4					// 57600 baud
#define	RTARDULINK_PORT_SPEED_115200	5					// 115200 baud

#define	RTARDULINK_PORT_SPEED_COUNT		6					// six codes total

extern unsigned long RTArduLinkSpeedMap[];

//------------------------------------------------------------------------------------------------------
//
//	Frame level defs and structure

#define	RTARDULINK_FRAME_MAX_LEN		64					// maximum possible length of a frame
#define	RTARDULINK_FRAME_HEADER_LEN		4					// 4 bytes in frame header (must correspond with the structure below!)
#define	RTARDULINK_MESSAGE_HEADER_LEN	2					// 2 bytes in message header (must correspond with the structure below!)
#define	RTARDULINK_MESSAGE_MAX_LEN		(RTARDULINK_FRAME_MAX_LEN - RTARDULINK_FRAME_HEADER_LEN)	// max length of message
#define	RTARDULINK_DATA_MAX_LEN			(RTARDULINK_MESSAGE_MAX_LEN - RTARDULINK_MESSAGE_HEADER_LEN)// max length of data field

#define	RTARDULINK_MESSAGE_SYNC0		0xAA
#define	RTARDULINK_MESSAGE_SYNC1		0x55

#define	RTARDULINK_MY_ADDRESS			0					// the subsystem address for local processing
#define	RTARDULINK_BROADCAST_ADDRESS	0xff				// the subsystem address for all subsystems
#define	RTARDULINK_ADDRESSES			0xff				// number of addresses (0 to 0xfe)

//	RTARDULINK_MESSAGE is carried in the RTARDULINK_FRAME
//
//  The messageAddress field allows subsystems to be daisy-chained. Valid addresses are 0 to 254.
//  Address 255 is a broadcast and goes to all subsystems.
//	Every message has the messageType and messageParam bytes but there can be from 0 to 58 bytes of data

typedef struct
{
	unsigned char messageAddress;							// subsystem message address
	unsigned char messageType;								// message type code
	unsigned char data[RTARDULINK_DATA_MAX_LEN];			// the actual data! Length is computed from messageLength.
} RTARDULINK_MESSAGE;

//	RTARDULINK_FRAME is the lowest level structure used across the RTArduLink

typedef struct
{
	unsigned char sync0;									// sync0 code
	unsigned char sync1;									// sync1 code
	unsigned char messageLength;							// the length of the message in the message field - between 2 and 60 bytes
	unsigned char frameChecksum;							// checksum for frame
	RTARDULINK_MESSAGE message;								// the actual message
} RTARDULINK_FRAME;

//	RTARDULINK_RXFRAME is a type that is used to reassemble a frame from a stream of bytes in conjunction with RCPReassemble()

typedef struct
{
	RTARDULINK_FRAME *frameBuffer;							// the frame buffer pointer
	int length;												// current length of frame
	int bytesLeft;											// number of bytes needed to complete
	bool complete;											// true if frame is complete and correct (as far as checksum goes)
} RTARDULINK_RXFRAME;

//	Message types

//	RTARDULINK_MESSAGE_POLL
//
//	The host should poll the RTArduLink at every RTARDULINK_POLL_INTERVAL.
//	The subsystem will respond by echoing the poll message as received.

#define	RTARDULINK_MESSAGE_POLL			0					// poll message

//	RTARDULINK_MESSAGE_IDENTIFY
//
//	The host can send this message to request an identity string from the subsystem.
//	Only the messageType field is used in the request host -> subsystem. The subsystem
//	responds with an identity string in the data field. 

#define	RTARDULINK_MESSAGE_IDENTITY		1					// identity message

//	RTARDULINK_MESSAGE_ILLEGAL_TYPE
//
//	This code is returned by the subsystem if it received a message with an illegal message type
//	The first byte of the data is the error code. The rest of the data field depends on the error.

#define	RTARDULINK_MESSAGE_ERROR		8					// illegal message type response

//	RTARDULINK_MESSAGE_DEBUG
//
//	This can be used to send a debug message up to the host. The data field contains a debug message

#define	RTARDULINK_MESSAGE_DEBUG		8					// debug message

//	RTARDULINK_MESSAGE_CUSTOM
//
//	This is the first message code that should be used for custom messages 16-255 are available.

#define	RTARDULINK_MESSAGE_CUSTOM		16					// start of custom messages

//	RTArduLink response codes

#define	RTARDULINK_RESPONSE_OK						0		// means things worked
#define	RTARDULINK_RESPONSE_ILLEGAL_COMMAND			1		// not a supported message type, data[1] has offending type		


#endif	// _RTARDULINKDEFS_H