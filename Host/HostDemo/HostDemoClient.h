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

#ifndef _RTARDULINKHOSTCLIENT_H
#define _RTARDULINKHOSTCLIENT_H

#include "RTArduLinkHost.h"

#define	HOSTCLIENT_TIMEOUT	2000							// 2 second timeout

class HostDemoClient : public RTArduLinkHost
{
	Q_OBJECT

public:
	HostDemoClient(QObject *parent);
	~HostDemoClient();

signals:
	void updateInputStatus(int inputNumber, bool value);

protected:
	void processCustomMessage(RTARDULINKHOST_PORT *portInfo, int messageAddress, 
						int messageType, unsigned char *data, int dataLength);

private:

};


#endif // _RTARDULINKHOSTECHO_H
