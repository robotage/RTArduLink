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
	QSlider slider;
	QLabel sliderLabel;
	int sliderPos;
} SERVO_CHANNEL;

typedef struct
{
	QSlider slider;
	QLabel sliderLabel;
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
	void RTArduLinkStatus(int port, int address, bool active, char *identity, qint64 pollsIn, qint64 pollsOut);
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

	QString m_identity;										// identity of subsystem in use
	int m_address;											// address of subsystem in use

	bool m_running;
	int m_timerID;

};

#endif // HOSTDEMO_H
