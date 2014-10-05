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

#include "EchoDialog.h"
#include "qheaderview.h"
#include "RTArduLinkDefs.h"

EchoDialog::EchoDialog(QWidget *parent)
	: QDialog(parent)
{
	ECHODIALOG_DATA *echoData;
	QTableWidgetItem *item;

	setWindowTitle("Echo test status display");

	QVBoxLayout *vertLayout = new QVBoxLayout(this);
	m_table = new QTableWidget();
	vertLayout->addWidget(m_table);

	m_table->setColumnCount(ECHODIALOG_COL_COUNT);
	m_table->setColumnWidth(ECHODIALOG_COL_PORT, 50);
	m_table->setColumnWidth(ECHODIALOG_COL_ADDRESS, 60);
	m_table->setColumnWidth(ECHODIALOG_COL_IDENTITY, 200);
	m_table->setColumnWidth(ECHODIALOG_COL_ECHOCOUNT, 100);
	m_table->setColumnWidth(ECHODIALOG_COL_ECHORATE, 100);
	m_table->setColumnWidth(ECHODIALOG_COL_TIMEOUTS, 100);

	m_table->verticalHeader()->setDefaultSectionSize(20);
	m_table->verticalHeader()->stretchLastSection();

    m_table->setHorizontalHeaderLabels(QStringList() << "Port" << "Address" << "Identity" 
		 << tr("Echoed msgs") << tr("Echo rate") << tr("Timeouts"));

    m_table->setSelectionMode(QAbstractItemView::NoSelection);

	for (int row = 0; row < HOSTECHO_MAX_SUBSYSTEMS; row++) {
		m_table->insertRow(row);
		m_table->setRowHeight(row, 20);

		for (int col = 0; col < ECHODIALOG_COL_COUNT; col++) {
			item = new QTableWidgetItem();
			item->setTextAlignment(Qt::AlignLeft | Qt::AlignBottom);
			item->setText("");
			item->setFlags(Qt::ItemIsEnabled);
			m_table->setItem(row, col, item);
		}
	}
	resize(700, 400);

	echoData = m_echoData;
	for (int index = 0; index < HOSTECHO_MAX_SUBSYSTEMS; index++, echoData++) {
		echoData->echoCount = 0;
		echoData->echoTempCount = 0;
		echoData->echoTimeouts = 0;
	}
	m_timerID = startTimer(1000);
	m_running = false;
}

void EchoDialog::closeEvent(QCloseEvent *)
{
	m_running = false;
	emit echoStop();
}

void EchoDialog::timerEvent(QTimerEvent *)
{
	ECHODIALOG_DATA *echoData;

	if (!m_running) {
		emit echoStart();
		m_running = true;
	}

	echoData = m_echoData;
	for (int index = 0; index < HOSTECHO_MAX_SUBSYSTEMS; index++, echoData++) {
		if (echoData->identity == "")
			continue;
		(m_table->item(index, ECHODIALOG_COL_ECHORATE))->setText(QString::number(echoData->echoTempCount));
		echoData->echoTempCount = 0;
	}

}


void EchoDialog::updateEchoStatus(int index, int port, int address, QString& identity, bool goodEcho)
{
	ECHODIALOG_DATA *echoData;

	echoData = m_echoData + index;
	if (echoData->identity != identity) {					// need to clear data
		echoData->echoCount = 0;
		echoData->echoTempCount = 0;
		echoData->echoTimeouts = 0;
		echoData->identity = identity;
	}
	if (goodEcho) {
		echoData->echoCount++;
		echoData->echoTempCount++;
	} else {
		echoData->echoTimeouts++;
	}

	(m_table->item(index, ECHODIALOG_COL_PORT))->setText(QString::number(port));
	(m_table->item(index, ECHODIALOG_COL_ADDRESS))->setText(QString::number(address));
	(m_table->item(index, ECHODIALOG_COL_IDENTITY))->setText(identity);
	(m_table->item(index, ECHODIALOG_COL_ECHOCOUNT))->setText(QString::number(echoData->echoCount));
	(m_table->item(index, ECHODIALOG_COL_TIMEOUTS))->setText(QString::number(echoData->echoTimeouts));
}
