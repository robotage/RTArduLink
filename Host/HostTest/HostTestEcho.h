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

#ifndef _HOSTTESTECHO_H
#define _HOSTTESTECHO_H

#include "RTArduLinkHost.h"

#define	HOSTECHO_TIMEOUT	2000							// 2 second timeout

#define	HOSTECHO_MAX_SUBSYSTEMS		256						// a reasonable number of subsystems to support

typedef struct
{
	bool active;											// if this is an active subsystem
	int index;												// index of this entry
	QString identity;										// the identity string from the subsystem
	int port;												// the subsystem's port index
	int address;											// the subsystem's address
	qint64 lastSendTime;									// the last time an echo was sent
} HOSTECHO_DATA;

class HostTestEcho : public RTArduLinkHost
{
	Q_OBJECT

public:
	HostTestEcho(QObject *parent);
	~HostTestEcho();

public slots:
	void echoStart();
	void echoStop();
	void RTArduLinkStatus(int port, int address, bool active, char *identity, qint64 pollsIn, qint64 pollsOut);

signals:
	void updateEchoStatus(int index, int port, int address, QString&, bool goodEcho);

protected:
	void processCustomMessage(RTARDULINKHOST_PORT *portInfo, int messageAddress, 
						int messageType, unsigned char *data, int dataLength);
	void processBackground();

private:
	void sendEcho(HOSTECHO_DATA *echoData);
	HOSTECHO_DATA *findEchoSlot(int port, int address);
	HOSTECHO_DATA *getFreeEchoSlot();
	qint64 m_now;
	unsigned char m_echoPayload[RTARDULINK_DATA_MAX_LEN];
	bool m_echoRunning;

	HOSTECHO_DATA m_echoData[HOSTECHO_MAX_SUBSYSTEMS];	// the echo data array

};


#endif // _HOSTTESTECHO_H
