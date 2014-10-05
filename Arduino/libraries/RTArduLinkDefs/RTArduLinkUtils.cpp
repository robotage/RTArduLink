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

#include "RTArduLinkUtils.h"

//	RTArduLinkRXFrameInit initializes the structure for a new frame using frameBuffer for storage

void RTArduLinkRXFrameInit(RTARDULINK_RXFRAME *RXFrame, RTARDULINK_FRAME *frameBuffer)
{
	RXFrame->complete = false;
	RXFrame->length = 0;
	RXFrame->bytesLeft = 0;
	RXFrame->frameBuffer = frameBuffer;
}

//	RTArduLinkReassemble takes a sequence of received bytes and tries to complete a frame. It returns true if ok
//	false if error. The caller can determine if the frame is complete by checking the complete flag.

bool RTArduLinkReassemble(RTARDULINK_RXFRAME *RXFrame, unsigned char data)
{
	bool flag = true;

	((unsigned char *)(RXFrame->frameBuffer))[RXFrame->length] = data;	// save byte in correct place
   	switch (RXFrame->length) {
		case 0:											// waiting for sync0
    		if (RXFrame->frameBuffer->sync0 == RTARDULINK_MESSAGE_SYNC0) {
    			RXFrame->length = 1;
    		}
    		break;

       	case 1:											// waiting for sync1
           	if (RXFrame->frameBuffer->sync1 == RTARDULINK_MESSAGE_SYNC1) {
            	RXFrame->length = 2;
            } else {
           		RXFrame->length = 0;					// try again if not correct two byte sequence
           	}
           	break;

       	case 2:											// should be message length
           	if (RXFrame->frameBuffer->messageLength <= RTARDULINK_MESSAGE_MAX_LEN) {
            	RXFrame->length = 3;
            	RXFrame->bytesLeft = RXFrame->frameBuffer->messageLength + 1;	// +1 to allow for the checksum
            } else {
           		RXFrame->length = 0;					// discard this and resync frame
				flag = false;
           	}
           	break;

    	default:
    		RXFrame->length++;
    		RXFrame->bytesLeft--;
    		if (RXFrame->bytesLeft == 0) {				// a complete frame!
       			if (!RTArduLinkCheckChecksum(RXFrame->frameBuffer)) {
    				RTArduLinkRXFrameInit(RXFrame, RXFrame->frameBuffer);
    				flag = false;						// flag the error
    			}
    			//	this is a valid frame (so far)
       			RXFrame->complete = true;
    		}
    		break;
    }
   	return flag;
}

//	RTArduLinkSetChecksum correctly sets the checksum field on an RCP frame prior to transmission
//

void RTArduLinkSetChecksum(RTARDULINK_FRAME *frame)
{
	int cksm;
	int i;
	unsigned char *data;

	for (i = 0, cksm = 0, data = (unsigned char *)&(frame->message); i < frame->messageLength; i++) 
		cksm += *data++;									// add up checksum
	frame->frameChecksum = (255 - cksm) + 1;				// 2s complement
}


//	RTArduLinkCheckChecksum checks a received frame's checksum.
//
//	It adds up all the bytes from the nFrameCksm byte to the end of the frame. The result should be 0.

bool RTArduLinkCheckChecksum(RTARDULINK_FRAME *frame)
{
	int length;
	int i;
	unsigned char *data;
	unsigned char cksm;

	length = frame->messageLength + 1;
	cksm = 0;
	data = (unsigned char *)&(frame->frameChecksum);

	for (i = 0; i < length; i++)
		cksm += *data++;

	return cksm == 0;
}

//	UC2 and UC4 Conversion routines
//

long RTArduLinkConvertUC4ToLong(RTARDULINK_UC4 UC4)
{
	long val;

	val = UC4[3];
	val += (long)UC4[2] << 8;
	val += (long)UC4[1] << 16;
	val += (long)UC4[0] << 24;
	return val;
}

void RTArduLinkConvertLongToUC4(long val, RTARDULINK_UC4 UC4)
{
	UC4[3] = val & 0xff;
	UC4[2] = (val >> 8) & 0xff;
	UC4[1] = (val >> 16) & 0xff;
	UC4[0] = (val >> 24) & 0xff;
}

int RTArduLinkConvertUC2ToInt(RTARDULINK_UC2 UC2)
{
	int val;

	val = UC2[1];
	val += (int)UC2[0] << 8;
	return val;
}

unsigned int RTArduLinkConvertUC2ToUInt(RTARDULINK_UC2 UC2)
{
	int val;

	val = UC2[1];
	val += (unsigned int)UC2[0] << 8;
	return val;
}


void RTArduLinkConvertIntToUC2(int val, RTARDULINK_UC2 UC2)
{
	UC2[1] = val & 0xff;
	UC2[0] = (val >> 8) & 0xff;
}


void RTArduLinkCopyUC2(RTARDULINK_UC2 destUC2, RTARDULINK_UC2 sourceUC2)
{
	destUC2[0] = sourceUC2[0];
	destUC2[1] = sourceUC2[1];
}
