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

#ifndef HOSTDEMO_H
#define HOSTDEMO_H

#include <QMainWindow>
#include "ui_HostDemo.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qcheckbox.h"
#include "qlabel.h"
#include "qextserialport.h"
#include "qcombobox.h"
#include "qtablewidget.h"
#include "qsettings.h"
#include "qboxlayout.h"
#include "HostDemo.h"
#include "RTArduLinkDemoDefs.h"


typedef struct
{
    QSlider *slider;
    QLabel *sliderLabel;
    int sliderPos;
} SERVO_CHANNEL;

typedef struct
{
    QSlider *slider;
    QLabel *sliderLabel;
    int sliderPos;
} PWM_CHANNEL;


class RTArduLinkHost;
class HostDemoClient;

class HostDemo : public QMainWindow
{
    Q_OBJECT

public:
    HostDemo(QWidget *parent = 0);
    ~HostDemo();

public slots:
    void RTArduLinkStatus(int port, int address, bool active, QString identity, qint64 pollsIn, qint64 pollsOut);
    void RTArduLinkPortOpen(int port);
    void RTArduLinkPortClosed(int port);
    void RTArduLinkPortTX(int port);
    void RTArduLinkPortRX(int port);
    void startButton();
    void stopButton();
    void sliderMoved(int);
    void outputChanged(int);
    void subsystemSelect(int index);
    void updateInputStatus(int inputNumber, bool value);

protected:
    void timerEvent(QTimerEvent *event);
    void populateComPorts();

private:
    HostDemoClient *m_link;

    Ui::HostDemoClass ui;

    void sendCommand();

    void layoutWindow();
    void layoutServoChannel(int channel, QVBoxLayout *vLayout);
    void layoutPWMChannel(int channel, QVBoxLayout *vLayout);
    void updateComState(bool open);
    void updateComRXTX(QLabel *label, bool active);

    QSettings *m_settings;
    QLabel *m_comLabel;
    QLabel *m_comRXLabel;
    QLabel *m_comTXLabel;
    QComboBox *m_comPort;
    QComboBox *m_comSpeed;
    QComboBox *m_subsystem;
    QPushButton *m_startButton;
    QPushButton *m_stopButton;
    QCheckBox *m_output[OUTPUT_COUNT];
    QCheckBox *m_input[INPUT_COUNT];

    SERVO_CHANNEL m_servo[SERVO_COUNT];
    PWM_CHANNEL m_pwm[PWM_COUNT];

    QString m_identity;                                     // identity of subsystem in use
    int m_address;                                          // address of subsystem in use

    bool m_running;
    int m_timerID;

};

#endif // HOSTDEMO_H
