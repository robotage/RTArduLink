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

#include "HostDemoClient.h"
#include "RTArduLinkDemoDefs.h"

#include "qdebug.h"

HostDemoClient::HostDemoClient(QObject *parent)
	: RTArduLinkHost(parent)
{
}

HostDemoClient::~HostDemoClient()
{
}

//----------------------------------------------------------
//
//	RTArduLinkHost overrides


//	This functions processes response from the subsystem. It emits a series of signals
//	to update the input states.

void HostDemoClient::processCustomMessage(RTARDULINKHOST_PORT *portInfo, int messageAddress, 
						int messageType, unsigned char *data, int dataLength)
{
	int port;
	RTARDULINKDEMO_RESPONSE *response;

	port = portInfo->index;

	if (messageType != RTARDULINK_MESSAGE_CUSTOM + 1) {
		qDebug() << QString("Received unexpected message type %1 from port %2 address %3")
			.arg(messageType).arg(port).arg(messageAddress);
		return;
	}
	if (dataLength != sizeof(RTARDULINKDEMO_RESPONSE)) {
		qDebug() << QString("Received message with incorrect length %1 from port %2 address %3")
			.arg(dataLength).arg(port).arg(messageAddress);
		return;
	}
	response = (RTARDULINKDEMO_RESPONSE *)data;

	for (int input = 0; input < INPUT_COUNT; input++)
		emit updateInputStatus(input, response->inputValue[input]);
}
