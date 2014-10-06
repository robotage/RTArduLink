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

#ifndef HOSTTEST_H
#define HOSTTEST_H

#include <QMainWindow>
#include "ui_HostTest.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qlabel.h"
#include "qextserialport.h"
#include "qcombobox.h"
#include "qtablewidget.h"
#include "qsettings.h"

#include "HostTestEcho.h"

#define	RTARDULINK_COL_PORT             0
#define	RTARDULINK_COL_ADDRESS          1
#define	RTARDULINK_COL_STATUS           2
#define	RTARDULINK_COL_IDENTITY         3
#define	RTARDULINK_COL_POLLSIN          4
#define	RTARDULINK_COL_POLLSOUT         5

#define	RTARDULINK_COL_COUNT            6

#define RTARDULINKHOST_SETTINGS_NAME    "ports"
#define	RTARDULINKHOST_SETTINGS_PORT    "port"
#define	RTARDULINKHOST_SETTINGS_SPEED   "speed"

class RTArduLinkHost;

class HostTest : public QMainWindow
{
    Q_OBJECT

public:
    HostTest(QWidget *parent = 0);
    ~HostTest();

    int m_row[RTARDULINKHOST_MAX_PORTS][RTARDULINK_ADDRESSES];  // row map array
    int m_nextRow;                                          // next row to be allocated

public slots:
    void startButton();
    void stopButton();
    void echoButton();
    void RTArduLinkStatus(int port, int address, bool active, char *identity, qint64 pollsIn, qint64 pollsOut);
    void RTArduLinkPortOpen(int port);
    void RTArduLinkPortClosed(int port);
    void RTArduLinkPortTX(int port);
    void RTArduLinkPortRX(int port);

protected:
    void timerEvent(QTimerEvent *event);
    void populateComPorts(int port);

private:
    HostTestEcho *m_link;

    Ui::HostTestClass ui;

    void layoutWindow();
    void updateComState(int index, bool open);
    void updateComRXTX(QLabel *label, bool active);
    void loadSettings();
    void saveSettings();

    QSettings *m_settings;
    QLabel *m_comLabel[RTARDULINKHOST_MAX_PORTS];
    QLabel *m_comRXLabel[RTARDULINKHOST_MAX_PORTS];
    QLabel *m_comTXLabel[RTARDULINKHOST_MAX_PORTS];
    QComboBox *m_comPort[RTARDULINKHOST_MAX_PORTS];
    QComboBox *m_comSpeed[RTARDULINKHOST_MAX_PORTS];
    QPushButton *m_startButton;
    QPushButton *m_stopButton;
    QPushButton *m_echoButton;
    QTableWidget *m_subsystemStatus;

    bool m_running;
    int m_timerID;

};

#endif // HOSTTEST_H
