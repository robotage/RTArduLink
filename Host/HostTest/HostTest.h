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

#ifndef RTARDULINKHOSTTEST_H
#define RTARDULINKHOSTTEST_H

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


#define	RTARDULINK_COL_PORT				0					
#define	RTARDULINK_COL_ADDRESS			1					
#define	RTARDULINK_COL_STATUS			2					
#define	RTARDULINK_COL_IDENTITY			3					
#define	RTARDULINK_COL_POLLSIN			4					
#define	RTARDULINK_COL_POLLSOUT			5

#define	RTARDULINK_COL_COUNT			6

#define RTARDULINKHOST_SETTINGS_NAME	"ports"
#define	RTARDULINKHOST_SETTINGS_PORT	"port"
#define	RTARDULINKHOST_SETTINGS_SPEED	"speed"

class RTArduLinkHost;

class HostTest : public QMainWindow
{
	Q_OBJECT

public:
	HostTest(QWidget *parent = 0);
	~HostTest();

	int m_row[RTARDULINKHOST_MAX_PORTS][RTARDULINK_ADDRESSES];	// row map array
	int m_nextRow;											// next row to be allocated

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

#endif // RTARDULINKHOSTTEST_H
