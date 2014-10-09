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

#ifndef _HOSTTESTECHO_H
#define _HOSTTESTECHO_H

#include "RTArduLinkHost.h"

#define	HOSTECHO_TIMEOUT	2000                            // 2 second timeout

#define	HOSTECHO_MAX_SUBSYSTEMS		256                     // a reasonable number of subsystems to support

typedef struct
{
    bool active;                                            // if this is an active subsystem
    int index;                                              // index of this entry
    QString identity;                                       // the identity string from the subsystem
    int port;                                               // the subsystem's port index
    int address;                                            // the subsystem's address
    qint64 lastSendTime;                                    // the last time an echo was sent
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
    void RTArduLinkStatus(int port, int address, bool active, QString identity, qint64 pollsIn, qint64 pollsOut);

signals:
    void updateEchoStatus(int index, int port, int address, QString&, bool goodEcho);

protected:
    virtual void processCustomMessage(RTARDULINKHOST_PORT *portInfo, unsigned int messageAddress,
                        unsigned char messageType, unsigned char messageParam, unsigned char *data, int dataLength);
    void processBackground();

private:
    void sendEcho(HOSTECHO_DATA *echoData);
    HOSTECHO_DATA *findEchoSlot(int port, int address);
    HOSTECHO_DATA *getFreeEchoSlot();
    qint64 m_now;
    unsigned char m_echoPayload[RTARDULINK_DATA_MAX_LEN];
    bool m_echoRunning;

    HOSTECHO_DATA m_echoData[HOSTECHO_MAX_SUBSYSTEMS];      // the echo data array

};


#endif // _HOSTTESTECHO_H
