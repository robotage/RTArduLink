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


#ifndef _ECHODIALOG_H
#define _ECHODIALOG_H

#include <qdialog.h>
#include <qtablewidget.h>
#include <qboxlayout.h>

#include "HostTestEcho.h"

#define	ECHODIALOG_COL_PORT				0					
#define	ECHODIALOG_COL_ADDRESS			1					
#define	ECHODIALOG_COL_IDENTITY			2					
#define	ECHODIALOG_COL_ECHOCOUNT		3					
#define	ECHODIALOG_COL_ECHORATE			4
#define	ECHODIALOG_COL_TIMEOUTS			5

#define	ECHODIALOG_COL_COUNT			6

typedef struct
{
	QString identity;										// identity for this data
	qint64 echoCount;										// number of good echos
	qint64 echoTimeouts;									// number of timeouts
	qint64 echoTempCount;									// temp count for rate calculation
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

	ECHODIALOG_DATA m_echoData[HOSTECHO_MAX_SUBSYSTEMS];	// row map array

	bool m_running;
};


#endif // _ECHODIALOG_H
