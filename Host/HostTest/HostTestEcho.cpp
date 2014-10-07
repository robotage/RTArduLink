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

#include "HostTestEcho.h"
#include "qdebug.h"

HostTestEcho::HostTestEcho(QObject *parent)
    : RTArduLinkHost(parent)
{
    m_now = 0;
    m_echoRunning = false;
}

HostTestEcho::~HostTestEcho()
{
}

//----------------------------------------------------------
//
//  RTArduLinkHost overrides

void HostTestEcho::processBackground()
{
    int index;
    HOSTECHO_DATA *echoData;

    if (!m_echoRunning)
        return;

    m_now += RTARDULINKHOST_BACKGROUND_INTERVAL;

    echoData = m_echoData;
    for (index = 0; index < HOSTECHO_MAX_SUBSYSTEMS; index++, echoData++) {
        if (!echoData->active)
            continue;
        if ((m_now - echoData->lastSendTime) > HOSTECHO_TIMEOUT) {
            emit updateEchoStatus(echoData->index, echoData->port, echoData->address,
                                    echoData->identity, false);
            sendEcho(echoData);
        }
    }
}

void HostTestEcho::processCustomMessage(RTARDULINKHOST_PORT *portInfo, unsigned int messageAddress,
                 unsigned char messageType, unsigned char /* messageParam */, unsigned char * /* data */, int /* dataLength */)

{
    int port;
    HOSTECHO_DATA *echoData;

    if (!m_echoRunning)
        return;

    port = portInfo->index;

    if (messageType != RTARDULINK_MESSAGE_ECHO) {
        qDebug() << QString("Received unexpected message type %1 from port %2 address %3")
            .arg(messageType).arg(port).arg(messageAddress);
        return;
    }

    //  locate my data entry for this port/address combination

    echoData = findEchoSlot(port, messageAddress);
    if (echoData == NULL) {
        qDebug() << QString("Received message on unused port %1 address %2").arg(port).arg(messageAddress);
        return;
    }
    emit updateEchoStatus(echoData->index, echoData->port, echoData->address, echoData->identity, true);
    sendEcho(echoData);
}

//----------------------------------------------------------
//
//  Echo test specific code

void HostTestEcho::echoStart()
{
    int index, port, address;
    HOSTECHO_DATA *echoData;

    //	clear out echo table

    for (index = 0; index < HOSTECHO_MAX_SUBSYSTEMS; index++) {
        m_echoData[index].active = false;
        m_echoData[index].index = index;
    }

    //	populate table with existing active subsystems

    for (port = 0; port < RTARDULINKHOST_MAX_PORTS; port++) {
        for (address = 0; address < RTARDULINK_ADDRESSES; address++) {
            if (m_subsystem[port][address].active) {
                echoData = getFreeEchoSlot();
                if (echoData == NULL) {
                    qDebug() << "Ran out of echo data slots";
                    continue;
                }
                echoData->active = true;
                echoData->port = port;
                echoData->address = address;
                echoData->identity = QString(m_subsystem[port][address].identity);
                sendEcho(echoData);                         // get echos started
            }
        }
    }
    connect(this, SIGNAL(RTArduLinkStatus(int, int, bool, char *, qint64, qint64)),
                    this, SLOT(RTArduLinkStatus(int, int, bool, char *, qint64, qint64)));

    m_echoRunning = true;
}

void HostTestEcho::echoStop()
{
    disconnect(this, SIGNAL(RTArduLinkStatus(int, int, bool, char *, qint64, qint64)),
                    this, SLOT(RTArduLinkStatus(int, int, bool, char *, qint64, qint64)));
    m_echoRunning = false;
}

void HostTestEcho::RTArduLinkStatus(int port, int address, bool active, char *identity, qint64, qint64)
{
    HOSTECHO_DATA *echoData;

    echoData = findEchoSlot(port, address);
    if (echoData == NULL) {                                 // potential new subsystem
        if (!active)
            return;                                         // not active so don't care
        echoData = getFreeEchoSlot();                       // get a new slot
        if (echoData == NULL) {
            qDebug() << "Ran out of echo slots";
            return;
        }
        echoData->active = true;
        echoData->port = port;
        echoData->address = address;
        echoData->identity = identity;
        sendEcho(echoData);                                 // send a first echo
        return;
    }
    if (!active) {                                          // subsystem has gone away
        echoData->active = false;                           // free the entry
        return;
    }
}

HOSTECHO_DATA *HostTestEcho::findEchoSlot(int port, int address)
{
    int index;
    HOSTECHO_DATA *echoData;

    echoData = m_echoData;
    for (index = 0; index < HOSTECHO_MAX_SUBSYSTEMS; index++, echoData++) {
        if (!echoData->active)
            continue;
        if ((echoData->port == port) && (echoData->address == address))
            return echoData;
    }
    return NULL;
}


HOSTECHO_DATA *HostTestEcho::getFreeEchoSlot()
{
    int index;

    for (index = 0; index < HOSTECHO_MAX_SUBSYSTEMS; index++)
        if (!m_echoData[index].active)
            return m_echoData + index;
    return NULL;
}


void HostTestEcho::sendEcho(HOSTECHO_DATA *echoData)
{
    sendMessage(echoData->port, echoData->address, RTARDULINK_MESSAGE_ECHO,
                        0, m_echoPayload, RTARDULINK_DATA_MAX_LEN);
    echoData->lastSendTime = m_now;
}

