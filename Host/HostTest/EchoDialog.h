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

#ifndef _ECHODIALOG_H
#define _ECHODIALOG_H

#include <qdialog.h>
#include <qtablewidget.h>
#include <qboxlayout.h>

#include "HostTestEcho.h"

#define	ECHODIALOG_COL_PORT             0
#define	ECHODIALOG_COL_ADDRESS          1
#define	ECHODIALOG_COL_IDENTITY         2
#define	ECHODIALOG_COL_ECHOCOUNT        3
#define	ECHODIALOG_COL_ECHORATE         4
#define	ECHODIALOG_COL_TIMEOUTS         5

#define	ECHODIALOG_COL_COUNT            6

typedef struct
{
    QString identity;                                       // identity for this data
    qint64 echoCount;                                       // number of good echos
    qint64 echoTimeouts;                                    // number of timeouts
    qint64 echoTempCount;                                   // temp count for rate calculation
} ECHODIALOG_DATA;

class EchoDialog : public QDialog
{
    Q_OBJECT

public:
    EchoDialog(QWidget *parent = 0);
    ~EchoDialog() {}

public slots:
    void updateEchoStatus(int index, int port, int address, QString&, bool goodEcho);

signals:
    void echoStart();
    void echoStop();

protected:
    void closeEvent(QCloseEvent *event);

    int m_timerID;
    void timerEvent(QTimerEvent *event);


private:
    QTableWidget *m_table;

    ECHODIALOG_DATA m_echoData[HOSTECHO_MAX_SUBSYSTEMS];    // row map array

    bool m_running;
};


#endif // _ECHODIALOG_H
