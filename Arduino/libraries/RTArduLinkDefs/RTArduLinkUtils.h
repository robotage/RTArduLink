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

#ifndef _RTARDULINKUTILS_H
#define _RTARDULINKUTILS_H

#include "RTArduLinkDefs.h"

//		Function defs

void	RTArduLinkRXFrameInit(RTARDULINK_RXFRAME *RXFrame, RTARDULINK_FRAME *frameBuffer);	// initializes RTARDULINK_RXFRAME for a new frame
bool	RTArduLinkReassemble(RTARDULINK_RXFRAME *RXFrame, unsigned char data);	// adds a byte to the reassembly, returns false if error

//		Checksum utilities

void	RTArduLinkSetChecksum(RTARDULINK_FRAME *frame);	// sets the checksum field prior to transmission
bool	RTArduLinkCheckChecksum(RTARDULINK_FRAME *frame);	// checks the checksum field after reception - returns true if ok, false if error

//		Type conversion utilities

int		RTArduLinkConvertUC4ToInt(RTARDULINK_UC4 uc4);		// converts a 4 byte array to a signed long
void	RTArduLinkConvertLongToUC4(long val, RTARDULINK_UC4 uc4);	// converts a long to a four byte array
int		RTArduLinkConvertUC2ToInt(RTARDULINK_UC2 uc2);		// converts a 2 byte array to a signed integer
unsigned int RTArduLinkConvertUC2ToUInt(RTARDULINK_UC2 uc2);		// converts a 2 byte array to an unsigned integer
void	RTArduLinkConvertIntToUC2(int val, RTARDULINK_UC2 uc2);	// converts an integer to a two byte array
void	RTArduLinkCopyUC2(RTARDULINK_UC2 destUC2, RTARDULINK_UC2 sourceUC2); // copies a UC2

#endif	// _RTARDULINKUTILS_H
