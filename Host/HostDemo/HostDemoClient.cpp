///////////////////////////////////////////////////////////
//
//  This file is part of RTArduLink
//
//  Copyright (c) 2014 richards-tech
//
//  Permission is hereby granted, free of charge,
//  to any person obtaining a copy of
//  this software and associated documentation files
//  (the "Software"), to deal in the Software without
//  restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute,
//  sublicense, and/or sell copies of the Software, and
//  to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice
//  shall be included in all copies or substantial portions
//  of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
//  ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
//  TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
//  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.

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
//  RTArduLinkHost overrides


//  This functions processes response from the subsystem. It emits a series of signals
//  to update the input states.

void HostDemoClient::processCustomMessage(RTARDULINKHOST_PORT *portInfo, unsigned int messageAddress,
                        unsigned char messageType, unsigned char /* messageParam */, unsigned char *data, int dataLength)
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
